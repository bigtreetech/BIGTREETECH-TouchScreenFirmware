#include "lcd_dma.h"
#include "variants.h"
#include "lcd.h"
#include "GUI.h"
#include "delay.h"
#include "w25qxx.h"

#ifdef STM32_HAS_FSMC
//Config for SPI Channel
#if W25Qxx_SPI == _SPI1
  #define W25QXX_SPI_NUM          SPI1
  #define W25QXX_SPI_DMA_RCC_AHB  RCC_AHBPeriph_DMA1
  #define W25QXX_SPI_DMA          DMA1
  #define W25QXX_SPI_DMA_CHANNEL  DMA1_Channel2
  #define W25QXX_SPI_DMA_IFCR_BIT 5
#elif W25Qxx_SPI == _SPI2
  #define W25QXX_SPI_NUM          SPI2
  #define W25QXX_SPI_DMA          DMA1
  #define W25QXX_SPI_DMA_RCC_AHB  RCC_AHBPeriph_DMA1
  #define W25QXX_SPI_DMA_CHANNEL  DMA1_Channel4
  #define W25QXX_SPI_DMA_IFCR_BIT 13
#elif W25Qxx_SPI == _SPI3
  #define W25QXX_SPI_NUM          SPI3
  #define W25QXX_SPI_DMA          DMA2
  #define W25QXX_SPI_DMA_RCC_AHB  RCC_AHBPeriph_DMA2
  #define W25QXX_SPI_DMA_CHANNEL  DMA2_Channel1
  #define W25QXX_SPI_DMA_IFCR_BIT 1
#endif

//SPI --> FSMC DMA (LCD_RAM)
//16bits, SPI_RX to LCD_RAM.
void LCD_DMA_Config(void)
{
  RCC->AHBENR |= W25QXX_SPI_DMA_RCC_AHB;				//开启DMA时钟
  Delay_ms(5);					//等待DMA时钟稳定
  W25QXX_SPI_DMA_CHANNEL->CPAR = (u32)&W25QXX_SPI_NUM->DR;			//外设地址为:SPI->DR
  W25QXX_SPI_DMA_CHANNEL->CMAR = (u32)&LCD->LCD_RAM;//目标地址为LCD_RAM
  W25QXX_SPI_DMA_CHANNEL->CNDTR = 0;			//DMA1,传输数据量,暂时设置为0
  W25QXX_SPI_DMA_CHANNEL->CCR = 0X00000000;	//复位
  W25QXX_SPI_DMA_CHANNEL->CCR |= 0<<4;		//从外设读
  W25QXX_SPI_DMA_CHANNEL->CCR |= 0<<5;		//普通模式
  W25QXX_SPI_DMA_CHANNEL->CCR |= 0<<6;		//外设地址非增量模式
  W25QXX_SPI_DMA_CHANNEL->CCR |= 0<<7;		//存储器非增量模式
  W25QXX_SPI_DMA_CHANNEL->CCR |= LCD_DATA_16BIT<<8;		//外设数据宽度为16位
  W25QXX_SPI_DMA_CHANNEL->CCR |= LCD_DATA_16BIT<<10;		//存储器数据宽度16位
  W25QXX_SPI_DMA_CHANNEL->CCR |= 1<<12;		//中等优先级
  W25QXX_SPI_DMA_CHANNEL->CCR |= 0<<14;		//非存储器到存储器模式
}

#define LCD_DMA_MAX_TRANS	65535		//DMA 65535 bytes one frame
// start DMA transfer from SPI->DR to FSMC
// the max bytes of one frame is LCD_DMA_MAX_TRANS 65535
void lcd_frame_segment_display(u16 size, u32 addr)
{
  W25QXX_SPI_DMA_CHANNEL->CNDTR = size;

  W25Qxx_SPI_CS_Set(0);
  W25Qxx_SPI_Read_Write_Byte(CMD_FAST_READ_DATA);
  W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>16));
  W25Qxx_SPI_Read_Write_Byte((u8)((addr)>>8));
  W25Qxx_SPI_Read_Write_Byte((u8)addr);
  W25Qxx_SPI_Read_Write_Byte(0XFF);  //8 dummy clock

  //set SPI to 16bit DMA rx only mode
  W25QXX_SPI_NUM->CR1 &= ~(1<<6);
  W25QXX_SPI_NUM->CR2 |= 1<<0;  //enable SPI rx DMA
  W25QXX_SPI_NUM->CR1 |= LCD_DATA_16BIT<<11;  //16bit data frame
  W25QXX_SPI_NUM->CR1 |= 1<<10;  //rx only

  W25QXX_SPI_DMA_CHANNEL->CCR |= 1<<0;  //enable dma channel
  W25QXX_SPI_NUM->CR1 |= 1<<6;  //enable SPI

  while((W25QXX_SPI_DMA->ISR&(1<<W25QXX_SPI_DMA_IFCR_BIT)) == 0);  //wait for rx complete
  W25QXX_SPI_DMA_CHANNEL->CCR &= (u32)(~(1<<0));
  W25QXX_SPI_DMA->IFCR |= (u32)(1<<W25QXX_SPI_DMA_IFCR_BIT);  //clear ISR for rx complete
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
