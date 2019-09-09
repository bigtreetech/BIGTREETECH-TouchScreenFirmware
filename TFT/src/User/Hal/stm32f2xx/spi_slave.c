#include "spi_slave.h"
#include "spi.h"
#include "GPIO_Init.h"

//TODO:
//now support SPI2 and PB12 CS only
//more compatibility changes are needed
//Config for SPI Channel
#if ST7920_SPI == _SPI1
  #define ST7920_SPI_NUM          SPI1
#elif ST7920_SPI == _SPI2
  #define ST7920_SPI_NUM          SPI2
#elif ST7920_SPI == _SPI3
  #define ST7920_SPI_NUM          SPI3
#endif

//#define _SPI_SLAVE_IRQ(n)  n##_IRQHandler
//#define SPI_SLAVE_IRQ  _SPI_SLAVE_IRQ(W25QXX_SPI_NUM)

SPI_QUEUE SPISlave;

void SPI_ReEnable(u8 mode)
{
  ST7920_SPI_NUM->CR1 = (0<<15)  //0:双线双向 1:单线双向
                      | (0<<14)  //单线双向时使用（0:只收 1:只发）
                      | (0<<13)  //1:启用硬件CRC
                      | (0<<12)  //0:下一个发送的值来自发送缓冲区 1:下一个发送的值来自发送CRC寄存器。
                      | (0<<11)  //0:8位数据帧 1:16位数据帧
                      | (1<<10)  //0:全双工 1:只接收
                      | (1<<9)   //0:硬件NSS 1:软件NSS
                      | (0<<8)   //SSM位为’1’时有意义。它决定了NSS上的电平
                      | (0<<7)   //0:MSB 1:LSB
                      | (7<<3)   //bit3-5   000： fPCLK/2 001： fPCLK/4 010： fPCLK/8 011： fPCLK/16
                                 //         100： fPCLK/32 101： fPCLK/64 110： fPCLK/128 111： fPCLK/256
                      | (0<<2)   //0:从设备 1:主设备
                      | (mode<<1)   //CPOL
                      | (mode<<0);  //CPHA
            
  ST7920_SPI_NUM->CR2 |= 1<<6;  //接收中断使能 SPI_I2S_IT_RXNE
}

void SPI_Slave(void) 
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  SPI_GPIO_Init(ST7920_SPI);  
  GPIO_InitSet(PB12, MGPIO_MODE_IPU, 0);  //CS
  
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  SPI_ReEnable(1);
}

void SPI_SlaveDeInit(void)
{  
  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  SPI_I2S_DeInit(ST7920_SPI_NUM);
}

void SPI2_IRQHandler(void) 
{ 
  SPISlave.data[SPISlave.wIndex] =  ST7920_SPI_NUM->DR;
  SPISlave.wIndex = (SPISlave.wIndex + 1) % SPI_SLAVE_MAX;
} 

/* 外部中断配置 */
void SPI_Slave_CS_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 

  /* 将GPIOA_0和中断线连接 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);//PB12 连接到中断线12

  /* 设置中断线0位外部下降沿中断 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级2， 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); 
}


/* 外部中断 */
void EXTI15_10_IRQHandler(void)
{
  if((GPIOB->IDR & (1<<12)) != 0)
  {
    SPI_ReEnable(!!(GPIOB->IDR & (1<<13))); //自适应 spi mode0/mode3
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
  else
  {
    RCC->APB1RSTR |= 1<<14;	//复位SPI1
    RCC->APB1RSTR &= ~(1<<14);
  }
/* 清除中断状态寄存器 */
  EXTI->PR = 1<<12;
}
