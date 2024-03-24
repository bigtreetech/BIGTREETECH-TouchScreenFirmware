#include "lcd_dma.h"
#include "variants.h"  // for STM32_HAS_FSMC etc.
#include "lcd.h"
#include "spi.h"
#include "LCD_Init.h"
#include "w25qxx.h"
#include "delay.h"
#include "debug.h"

#ifdef STM32_HAS_FSMC

#if W25Qxx_SPI == _SPI1
  #define W25QXX_SPI_NUM          SPI0
  #define W25QXX_SPI_DMA          DMA0
  #define W25QXX_SPI_DMA_RCU      RCU_DMA0
  #define W25QXX_SPI_DMA_CHANNEL  DMA_CH1
  #define W25QXX_SPI_DMA_IFCR_BIT 5
#elif W25Qxx_SPI == _SPI2
  #define W25QXX_SPI_NUM          SPI1
  #define W25QXX_SPI_DMA          DMA0
  #define W25QXX_SPI_DMA_RCU      RCU_DMA0
  #define W25QXX_SPI_DMA_CHANNEL  DMA_CH3
  #define W25QXX_SPI_DMA_IFCR_BIT 13
#elif W25Qxx_SPI == _SPI3
  #define W25QXX_SPI_NUM          SPI2
  #define W25QXX_SPI_DMA          DMA1
  #define W25QXX_SPI_DMA_RCU      RCU_DMA1
  #define W25QXX_SPI_DMA_CHANNEL  DMA_CH0
  #define W25QXX_SPI_DMA_IFCR_BIT 1
#endif

// SPI --> FSMC DMA (LCD_RAM)
// 16bits, SPI_RX to LCD_RAM
void LCD_DMA_Config(void)
{
  rcu_periph_clock_enable(W25QXX_SPI_DMA_RCU);                                                // turn on the DMA clock
  Delay_ms(5);                                                                                // wait for the DMA clock to stabilize
  DMA_CHPADDR(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) = (uint32_t)&SPI_DATA(W25QXX_SPI_NUM);  // the peripheral address is: SPI-> DR
  DMA_CHMADDR(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) = (uint32_t)&LCD->LCD_RAM;              // the target address is LCD_RAM
  DMA_CHCNT(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) = 0;                    // DMA1, the amount of data transferred, temporarily set to 0
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) = 0x00000000;           // reset
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 0<<4;                // read from peripheral
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 0<<5;                // normal mode
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 0<<6;                // peripheral address non-incremental mode
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 0<<7;                // memory non-incremental mode
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= LCD_DATA_16BIT<<8;   // peripheral data width is 16 bits
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= LCD_DATA_16BIT<<10;  // memory data width 16 bits
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 1<<12;               // medium priority
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 0<<14;               // non-memory to memory mode
}

#define LCD_DMA_MAX_TRANS 65535  // DMA 65535 bytes one frame

// start DMA transfer from SPI->DR to FSMC
// the max bytes of one frame is LCD_DMA_MAX_TRANS 65535
static inline void lcd_frame_segment_display(uint16_t size, uint32_t addr)
{
  DMA_CHCNT(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) = size;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_FAST_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((uint8_t)((addr)>>16));
  W25Qxx_SPI_Read_Write_Byte((uint8_t)((addr)>>8));
  W25Qxx_SPI_Read_Write_Byte((uint8_t)addr);
  W25Qxx_SPI_Read_Write_Byte(0XFF);  // 8 dummy clock

  // set SPI to 16bit DMA rx only mode
  SPI_CTL0(W25QXX_SPI_NUM) &= ~(1<<6);                                       // disable SPI
  SPI_CTL1(W25QXX_SPI_NUM) |= 1<<0;                                          // enable SPI rx DMA
  SPI_CTL0(W25QXX_SPI_NUM) |= LCD_DATA_16BIT<<11;                            // 16bit data frame
  SPI_CTL0(W25QXX_SPI_NUM) |= 1<<10;                                         // rx only

  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) |= 1<<0;                 // enable dma channel
  SPI_CTL0(W25QXX_SPI_NUM) |= 1<<6;                                          // enable SPI

  while (((DMA_INTF(W25QXX_SPI_DMA) & (1<<W25QXX_SPI_DMA_IFCR_BIT)) == 0));  // wait for rx complete
  DMA_CHCTL(W25QXX_SPI_DMA, W25QXX_SPI_DMA_CHANNEL) &= (uint32_t)(~(1<<0));
  DMA_INTC(W25QXX_SPI_DMA) |= (uint32_t)(1<<W25QXX_SPI_DMA_IFCR_BIT);        // clear ISR for rx complete
  W25Qxx_SPI_CS_Set(1);

  SPI_Protocol_Init(W25Qxx_SPI, W25Qxx_SPEED);                               // reset SPI clock and config again
}

void lcd_frame_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint32_t addr)
{
  uint32_t cur = 0;
  uint32_t segmentSize;
  uint32_t totalSize = w * h * (2 - LCD_DATA_16BIT);

  LCD_SetWindow(sx, sy, sx + w - 1, sy + h - 1);

  for (cur = 0; cur < totalSize; cur += LCD_DMA_MAX_TRANS)
  {
    segmentSize = (cur + LCD_DMA_MAX_TRANS) <= totalSize ? LCD_DMA_MAX_TRANS : totalSize - cur;
    lcd_frame_segment_display(segmentSize, addr + cur * (LCD_DATA_16BIT + 1));
  }
}

#endif  // STM32_HAS_FSMC
