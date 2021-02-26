#include "spi_slave.h"
#include "spi.h"
#include "GPIO_Init.h"
#include "stdlib.h"
#include "stm32f10x_conf.h"

#if defined(MKS_32_V1_4) || defined (MKS_28_V1_0)

#if defined(ST7920_EMULATOR)
//TODO:
//now support SPI3 and PB1 CS only
//more compatibility changes are needed
//Config for SPI Channel
#if ST7920_SPI == _SPI1
  #define ST7920_SPI_NUM          SPI1
#elif ST7920_SPI == _SPI2
  #define ST7920_SPI_NUM          SPI2
#elif ST7920_SPI == _SPI3
  #define ST7920_SPI_NUM          SPI3
#endif

CIRCULAR_QUEUE *spi_queue = NULL;

void SPI_Slave_CS_Config(void);        // forward declaration

void SPI_ReEnable(u8 mode)
{
  ST7920_SPI_NUM->CR1 = (0<<15)        // 0:2-line 1: 1-line
                      | (0<<14)        // in bidirectional mode 0:read only 1: read/write
                      | (0<<13)        // 0:disable CRC 1:enable CRC
                      | (0<<12)        // 0:Data phase (no CRC phase) 1:Next transfer is CRC (CRC phase)
                      | (0<<11)        // 0:8-bit data frame 1:16-bit data frame
                      | (1<<10)        // 0:Full duplex 1:Receive-only
                      | (1<<9)         // 0:enable NSS 1:disable NSS (Software slave management)
                      | (0<<8)         // This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored
                      | (0<<7)         // 0:MSB 1:LSB
                      | (7<<3)         // bit3-5   000:fPCLK/2    001:fPCLK/4    010:fPCLK/8     011:fPCLK/16
                                       //          100:fPCLK/32   101:fPCLK/64   110:fPCLK/128   111:fPCLK/256
                      | (0<<2)         // 0:Slave 1:Master
                      | (mode<<1)      // CPOL
//                      | (mode<<0);   // CPHA
                      | (1<<0);        // always use 1 instead of "mode"!

  ST7920_SPI_NUM->CR2 |= 1<<6;         // RX buffer not empty interrupt enable SPI_I2S_IT_RXNE
}

void SPI_Slave(CIRCULAR_QUEUE *queue)
{
  spi_queue = queue;

  // initializes the initial queue indexes before the queue is used.
  // Otherwise, dirty values will let the system probably freeze when the queue is used
  spi_queue->index_r = spi_queue->index_w = spi_queue->count = 0;

  NVIC_InitTypeDef NVIC_InitStructure;

#ifndef SPI3_PIN_SMART_USAGE                                         // if enabled, it avoids any SPI3 CS pin usage and free the MISO (PB4 pin) for encoder pins
  SPI_GPIO_Init(ST7920_SPI);
  GPIO_InitSet(SPI3_CS_PIN, MGPIO_MODE_IPU, 0);                      // CS
#endif

  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

#ifndef SPI3_PIN_SMART_USAGE                                         // if enabled, it avoids any SPI3 CS pin usage and free the MISO (PB4 pin) for encoder pins
  SPI_Slave_CS_Config();                                             // not needed, you can also comment out this line!
#endif

  SPI_ReEnable(0);  // spi mode0

//  if ((GPIOB->IDR & (1<<12)) != 0)                                 // always leave this line commented out!
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

  spi_queue = NULL;
}

void SPI3_IRQHandler(void)
{
  spi_queue->data[spi_queue->index_w] = ST7920_SPI_NUM->DR;
  spi_queue->index_w = (spi_queue->index_w + 1) % CIRCULAR_QUEUE_SIZE;
}

bool SPI_SlaveGetData(uint8_t *data)
{
  bool dataRead = false;

  if (spi_queue->index_r != spi_queue->index_w)
  {
    *data = spi_queue->data[spi_queue->index_r];
    spi_queue->index_r = (spi_queue->index_r + 1) % CIRCULAR_QUEUE_SIZE;

    dataRead = true;
  }

  return dataRead;
}

// External interruption arrangement
void SPI_Slave_CS_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // Connect GPIOB1 to the interrupt line
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);        // PB1 is connected to interrupt line 1

  // Set interrupt line 1 bit external falling edge interrupt
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;                   // External interrupt channel where the key is enabled
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;       // Preemption priority 2?
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;              // Sub-priority 1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    // Enable external interrupt channel
  NVIC_Init(&NVIC_InitStructure);
}

// External interruption
void EXTI1_IRQHandler(void)
{
  if ((GPIOB->IDR & (1<<1)) != 0)
  {
    SPI_ReEnable(!!(GPIOB->IDR & (1<<13)));                          // Adaptive spi mode0 / mode3
    ST7920_SPI_NUM->CR1 |= (1<<6);
  }
  else
  {
    RCC->APB1RSTR |= 1<<14;                                          // Reset SPI3
    RCC->APB1RSTR &= ~(1<<14);
  }

  EXTI->PR = 1<<1;                                                   // Clear interrupt status register
}
#endif             // endif for #if defined(ST7920_EMULATOR)

#endif             // endif for #if defined(MKS_32_V1_4) || defined (MKS_28_V1_0)
