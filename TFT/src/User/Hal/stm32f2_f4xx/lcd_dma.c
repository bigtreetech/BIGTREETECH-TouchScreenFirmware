#include "lcd_dma.h"
#include "variants.h"  // for STM32_HAS_FSMC etc...
#include "lcd.h"
#include "spi.h"
#include "LCD_Init.h"
#include "w25qxx.h"
#include "delay.h"

#ifdef STM32_HAS_FSMC
// Config for SPI Channel
// SPI1 RX DMA2 Channel3 Steam0/2
// SPI2 RX DMA1 Channel0 Steam3
// SPI3 RX DMA1 Channel0 Steam0/2
#if W25Qxx_SPI == _SPI1
  #define W25QXX_SPI_NUM              SPI1
  #define W25QXX_SPI_DMA_RCC_AHB      RCC_AHB1Periph_DMA2
  #define W25QXX_SPI_DMA_STREAM       DMA2_Stream0
  #define W25QXX_SPI_DMA_CHANNEL      3
  #define W25QXX_SPI_DMA_READING()    (DMA2->LISR & (1<<5)) == 0
  #define W25QXX_SPI_DMA_CLEAR_FLAG() DMA2->LIFCR = 0x3F           // bit:0-5
#elif W25Qxx_SPI == _SPI2
  #define W25QXX_SPI_NUM              SPI2
  #define W25QXX_SPI_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define W25QXX_SPI_DMA_STREAM       DMA1_Stream3
  #define W25QXX_SPI_DMA_CHANNEL      0
  #define W25QXX_SPI_DMA_READING()    (DMA1->LISR & (1<<27)) == 0
  #define W25QXX_SPI_DMA_CLEAR_FLAG() DMA1->LIFCR = (0xFC<<20)     // bit:22-27
#elif W25Qxx_SPI == _SPI3
  #define W25QXX_SPI_NUM              SPI3
  #define W25QXX_SPI_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define W25QXX_SPI_DMA_STREAM       DMA1_Stream0
  #define W25QXX_SPI_DMA_CHANNEL      0
  #define W25QXX_SPI_DMA_READING()   (DMA1->LISR & (1<<5)) == 0
  #define W25QXX_SPI_DMA_CLEAR_FLAG() DMA1->LIFCR = 0x3F           // bit:0-5
#endif

// SPI --> FSMC DMA (LCD_RAM)
// 16bits, SPI_RX to LCD_RAM.
void LCD_DMA_Config(void)
{
  RCC->AHB1ENR |= W25QXX_SPI_DMA_RCC_AHB;                      // Turn on the DMA clock
  Delay_ms(5);                                                 // Wait for the DMA clock to stabilize
  W25QXX_SPI_DMA_STREAM->PAR = (uint32_t)&W25QXX_SPI_NUM->DR;  // The peripheral address is: SPI-> DR
  W25QXX_SPI_DMA_STREAM->M0AR = (uint32_t)&LCD->LCD_RAM;       // The target address is LCD_RAM
  //W25QXX_SPI_DMA_STREAM->M1AR = 0;                           // (used in case of Double buffer mode)
  //W25QXX_SPI_DMA_CHANNEL->CMAR =
  W25QXX_SPI_DMA_STREAM->NDTR = 0;                             // DMA, the amount of data transferred, temporarily set to 0

  W25QXX_SPI_DMA_STREAM->CR = W25QXX_SPI_DMA_CHANNEL << 25;
  W25QXX_SPI_DMA_STREAM->CR |= 1<<16;                          // Priority level: Medium
  W25QXX_SPI_DMA_STREAM->CR |= LCD_DATA_16BIT<<13;             // Memory data width 16 bits
  W25QXX_SPI_DMA_STREAM->CR |= LCD_DATA_16BIT<<11;             // Peripheral data width is 16 bits
  W25QXX_SPI_DMA_STREAM->CR |= 0<<10;                          // Memory non-incremental mode
  W25QXX_SPI_DMA_STREAM->CR |= 0<<9;                           // Peripheral address non-incremental mode
  W25QXX_SPI_DMA_STREAM->CR |= 0<<6;                           // Non-memory to memory mode
}

#define LCD_DMA_MAX_TRANS 65535  // DMA 65535 bytes one frame

// start DMA transfer from SPI->DR to FSMC
// the max bytes of one frame is LCD_DMA_MAX_TRANS 65535
void lcd_frame_segment_display(uint16_t size, uint32_t addr)
{
  W25QXX_SPI_DMA_STREAM->NDTR = size;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_FAST_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((uint8_t)((addr)>>16));
  W25Qxx_SPI_Read_Write_Byte((uint8_t)((addr)>>8));
  W25Qxx_SPI_Read_Write_Byte((uint8_t)addr);
  W25Qxx_SPI_Read_Write_Byte(0XFF);  // 8 dummy clock

  //set SPI to 16bit DMA rx only mode
  W25QXX_SPI_NUM->CR1 &= ~(1<<6);                    // disable SPI
  W25QXX_SPI_NUM->CR2 |= 1<<0;                       // enable SPI rx DMA
  W25QXX_SPI_NUM->CR1 |= LCD_DATA_16BIT<<11;         // 16bit data frame
  W25QXX_SPI_NUM->CR1 |= 1<<10;                      // rx only

  W25QXX_SPI_DMA_STREAM->CR |= 1<<0;                 // enable dma channel
  W25QXX_SPI_NUM->CR1 |= 1<<6;                       // enable SPI

  while (W25QXX_SPI_DMA_READING());                  // wait for rx complete
  W25QXX_SPI_DMA_STREAM->CR &= (uint32_t)(~(1<<0));
  W25QXX_SPI_DMA_CLEAR_FLAG();                       // clear ISR for rx complete
  W25Qxx_SPI_CS_Set(1);

  SPI_Protocol_Init(W25Qxx_SPI, W25Qxx_SPEED);       // Reset SPI clock and config again
}

void lcd_frame_display(uint16_t sx,uint16_t sy,uint16_t w,uint16_t h, uint32_t addr)
{
  uint32_t cur=0;
  uint32_t segmentSize;
  uint32_t totalSize = w*h*(2-LCD_DATA_16BIT);

  LCD_SetWindow(sx,sy,sx+w-1,sy+h-1);

  for (cur = 0; cur < totalSize; cur += LCD_DMA_MAX_TRANS)
  {
    segmentSize = (cur+LCD_DMA_MAX_TRANS)<=totalSize ? LCD_DMA_MAX_TRANS : totalSize-cur;
    lcd_frame_segment_display(segmentSize, addr+cur*(LCD_DATA_16BIT + 1));
  }
}
#endif
