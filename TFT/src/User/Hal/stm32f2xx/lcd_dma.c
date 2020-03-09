#include "lcd_dma.h"
#include "variants.h"
#include "lcd.h"
#include "GUI.h"
#include "delay.h"
#include "w25qxx.h"

#ifdef STM32_HAS_FSMC
//Config for SPI Channel
//SPI1 RX DMA2 Channel3 Steam0/2
//SPI2 RX DMA1 Channel0 Steam3
//SPI3 RX DMA1 Channel0 Steam0/2
#if W25Qxx_SPI == _SPI1
  #define W25QXX_SPI_NUM              SPI1
  #define W25QXX_SPI_DMA_RCC_AHB      RCC_AHB1Periph_DMA2
  #define W25QXX_SPI_DMA_STREAM       DMA2_Stream0
  #define W25QXX_SPI_DMA_CHANNEL      3
  #define W25QXX_SPI_DMA_READING()    (DMA1->LISR & (1<<5)) == 0
  #define W25QXX_SPI_DMA_CLEAR_FLAG() DMA1->LIFCR = 0x3F //bit:0-5
#elif W25Qxx_SPI == _SPI2
  #define W25QXX_SPI_NUM              SPI2
  #define W25QXX_SPI_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define W25QXX_SPI_DMA_STREAM       DMA1_Stream3
  #define W25QXX_SPI_DMA_CHANNEL      0
  #define W25QXX_SPI_DMA_READING()    (DMA1->LISR & (1<<27)) == 0
  #define W25QXX_SPI_DMA_CLEAR_FLAG() DMA1->LIFCR = (0xFC<<20) //bit:22-27
#elif W25Qxx_SPI == _SPI3
  #define W25QXX_SPI_NUM              SPI3
  #define W25QXX_SPI_DMA_RCC_AHB      RCC_AHB1Periph_DMA1
  #define W25QXX_SPI_DMA_STREAM       DMA1_Stream0
  #define W25QXX_SPI_DMA_CHANNEL      0
  #define W25QXX_SPI_DMA_READING()   (DMA1->LISR & (1<<5)) == 0
  #define W25QXX_SPI_DMA_CLEAR_FLAG() DMA1->LIFCR = 0x3F //bit:0-5
#endif

//SPI --> FSMC DMA (LCD_RAM)
//16bits, SPI_RX to LCD_RAM.
void LCD_DMA_Config(void)
{
  RCC->AHB1ENR |= W25QXX_SPI_DMA_RCC_AHB;				//开启DMA时钟
  Delay_ms(5);					//等待DMA时钟稳定
  W25QXX_SPI_DMA_STREAM->PAR = (u32)&W25QXX_SPI_NUM->DR;			//外设地址为:SPI->DR
  W25QXX_SPI_DMA_STREAM->M0AR = (u32)&LCD->LCD_RAM;//目标地址为LCD_RAM
//  W25QXX_SPI_DMA_STREAM->M1AR = 0; // (used in case of Double buffer mode)
//  W25QXX_SPI_DMA_CHANNEL->CMAR =
  W25QXX_SPI_DMA_STREAM->NDTR = 0;			// DMA,传输数据量,暂时设置为0

  W25QXX_SPI_DMA_STREAM->CR = W25QXX_SPI_DMA_CHANNEL << 25;
  W25QXX_SPI_DMA_STREAM->CR |= 1<<16;  // Priority level: Medium
  W25QXX_SPI_DMA_STREAM->CR |= LCD_DATA_16BIT<<13;  //存储器数据宽度16位
  W25QXX_SPI_DMA_STREAM->CR |= LCD_DATA_16BIT<<11;  //外设数据宽度为16位
  W25QXX_SPI_DMA_STREAM->CR |= 0<<10;  //存储器非增量模式
  W25QXX_SPI_DMA_STREAM->CR |= 0<<9;   //外设地址非增量模式
  W25QXX_SPI_DMA_STREAM->CR |= 0<<6;   //非存储器到存储器模式
}

#define LCD_DMA_MAX_TRANS	65535		//DMA 65535 bytes one frame
// start DMA transfer from SPI->DR to FSMC
// the max bytes of one frame is LCD_DMA_MAX_TRANS 65535
void lcd_frame_segment_display(u16 size, u32 addr)
{
  W25QXX_SPI_DMA_STREAM->NDTR = size;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_FAST_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>16));
  W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>8));
  W25Qxx_SPI_Read_Write_Byte((u8)addr);
  W25Qxx_SPI_Read_Write_Byte(0XFF);  //8 dummy clock

  //set SPI to 16bit DMA rx only mode
  W25QXX_SPI_NUM->CR1 &= ~(1<<6);  //disable SPI
  W25QXX_SPI_NUM->CR2 |= 1<<0;  //enable SPI rx DMA
  W25QXX_SPI_NUM->CR1 |= LCD_DATA_16BIT<<11;  //16bit data frame
  W25QXX_SPI_NUM->CR1 |= 1<<10;  //rx only

  W25QXX_SPI_DMA_STREAM->CR |= 1<<0;  //enable dma channel
  W25QXX_SPI_NUM->CR1 |= 1<<6;  //enable SPI

  while(W25QXX_SPI_DMA_READING());  //wait for rx complete
  W25QXX_SPI_DMA_STREAM->CR &= (u32)(~(1<<0));
  W25QXX_SPI_DMA_CLEAR_FLAG();  //clear ISR for rx complete
  W25Qxx_SPI_CS_Set(1);

  SPI_Protocol_Init(W25Qxx_SPI, W25Qxx_SPEED);  //Reset SPI clock and config again
}

void lcd_frame_display(u16 sx,u16 sy,u16 w,u16 h, u32 addr)
{
  u32 cur=0;
  u32 segmentSize;
  u32 totalSize = w*h*(2-LCD_DATA_16BIT);

  LCD_SetWindow(sx,sy,sx+w-1,sy+h-1);
  LCD_WR_REG(0x2C);

  for(cur = 0; cur < totalSize; cur += LCD_DMA_MAX_TRANS)
  {
    segmentSize = (cur+LCD_DMA_MAX_TRANS)<=totalSize ? LCD_DMA_MAX_TRANS : totalSize-cur;
    lcd_frame_segment_display(segmentSize, addr+cur*(LCD_DATA_16BIT + 1));
  }
}
#endif
