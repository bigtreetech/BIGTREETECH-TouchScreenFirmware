#include "spi_slave.h"
#include "spi.h"
#include "GPIO_Init.h"
#include "stdlib.h"

#if defined(ST7920_SPI) && defined(MKS_32_V1_4)
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
                      | (mode<<1)      // CPOL  (mode<<1)
//                      | (mode<<0);     // CPHA  (mode<<0)
                      | (1<<0);        // always use 1 instead of "mode"!

  ST7920_SPI_NUM->CR2 |= 1<<6; // RX buffer not empty interrupt enable SPI_I2S_IT_RXNE
}

void SPI_Slave(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  SPISlave.data = malloc(SPI_SLAVE_MAX);
  while(!SPISlave.data);  // malloc failed

#ifndef SPI3_PIN_SMART_USAGE                     // if enabled, it avoids any SPI3 CS pin usage and free the MISO (PB4 pin) for encoder pins
  SPI_GPIO_Init(ST7920_SPI);
  GPIO_InitSet(SPI3_CS_PIN, MGPIO_MODE_IPU, 0);  // CS
#endif

  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

#ifndef SPI3_PIN_SMART_USAGE                     // if enabled, it avoids any SPI3 CS pin usage and free the MISO (PB4 pin) for encoder pins
  SPI_Slave_CS_Config();                         // not needed, you can also comment out this line!
#endif

  SPI_ReEnable(0);  // spi mode0

//  if((GPIOB->IDR & (1<<12)) != 0)              // always leave this line commented out!
  {
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
}

void SPI_SlaveDeInit(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);
  free(SPISlave.data);
  SPISlave.data = NULL;
}

void SPI3_IRQHandler(void)
{
  SPISlave.data[SPISlave.wIndex] = ST7920_SPI_NUM->DR;
  SPISlave.wIndex = (SPISlave.wIndex + 1) % SPI_SLAVE_MAX;
}

void SPI_Slave_CS_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_IRQHandler(void)
{
  if((GPIOB->IDR & (1<<1)) != 0)
  {
    SPI_ReEnable(!!(GPIOB->IDR & (1<<13)));  // Adaptive spi mode0 / mode3
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
  else
  {
    RCC->APB1RSTR |= 1<<14;  // Reset SPI3
    RCC->APB1RSTR &= ~(1<<14);
  }

  EXTI->PR = 1<<1;
}

#endif
