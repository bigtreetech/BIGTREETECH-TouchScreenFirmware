#include "spi_slave.h"
#include "spi.h"
#include "GPIO_Init.h"
#include "stdlib.h"

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


SPI_QUEUE SPISlave;

void SPI_ReEnable(u8 mode)
{
  ST7920_SPI_NUM->CR1 = (0<<15)  // 0:2-line 1: 1-line
                      | (0<<14)  // in bidirectional mode 0:read only 1: read/write
                      | (0<<13)  // 0:disable CRC 1:enable CRC
                      | (0<<12)  // 0:Data phase (no CRC phase) 1:Next transfer is CRC (CRC phase)
                      | (0<<11)  // 0:8-bit data frame 1:16-bit data frame
                      | (1<<10)  // 0:Full duplex 1:Receive-only
                      | (1<<9)   // 0:enable NSS 1:disable NSS (Software slave management)
                      | (0<<8)   // This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored
                      | (0<<7)   // 0:MSB 1:LSB
                      | (7<<3)   // bit3-5   000:fPCLK/2    001:fPCLK/4    010:fPCLK/8     011:fPCLK/16
                                 //          100:fPCLK/32   101:fPCLK/64   110:fPCLK/128   111:fPCLK/256
                      | (0<<2)   // 0:Slave 1:Master
                      | (mode<<1)   // CPOL
                      | (mode<<0);  // CPHA

  ST7920_SPI_NUM->CR2 |= 1<<6; // RX buffer not empty interrupt enable SPI_I2S_IT_RXNE
}

void SPI_Slave(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  SPISlave.data = malloc(SPI_SLAVE_MAX);
  while(!SPISlave.data); // malloc failed
  SPI_GPIO_Init(ST7920_SPI);
  GPIO_InitSet(PB12, MGPIO_MODE_IPU, 0);  // CS

  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  SPI_Slave_CS_Config();
  SPI_ReEnable(0); // spi mode0
  
  if((GPIOB->IDR & (1<<12)) != 0)
  {
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
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
  free(SPISlave.data);
  SPISlave.data = NULL;
}

void SPI2_IRQHandler(void)
{
  SPISlave.data[SPISlave.wIndex] =  ST7920_SPI_NUM->DR;
  SPISlave.wIndex = (SPISlave.wIndex + 1) % SPI_SLAVE_MAX;
}

/*External interruption arrangement*/
void SPI_Slave_CS_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Connect GPIOB12 to the interrupt line */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//Enable SYSCFG clock
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);//PB12 is connected to interrupt line 12

  /*Set interrupt line 12 bit external falling edge interrupt */
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//External interrupt channel where the key is enabled
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//Preemption priority 2?
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//Sub-priority 1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Enable external interrupt channel
  NVIC_Init(&NVIC_InitStructure);
}


/*External interruption*/
void EXTI15_10_IRQHandler(void)
{
  if((GPIOB->IDR & (1<<12)) != 0)
  {
    SPI_ReEnable(!!(GPIOB->IDR & (1<<13))); // Adaptive spi mode0 / mode3
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
  else
  {
    RCC->APB1RSTR |= 1<<14;	// Reset SPI2
    RCC->APB1RSTR &= ~(1<<14);
  }
/* Clear interrupt status register */
  EXTI->PR = 1<<12;
}
