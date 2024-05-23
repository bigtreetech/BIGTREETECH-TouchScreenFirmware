#include "spi_slave.h"
#include "includes.h"  // for infoSettings, ST7920_EMULATOR etc.
#include "spi.h"
#include "GPIO_Init.h"
#include "HD44780.h"

#ifdef ST7920_EMULATOR

// TODO:
// now support SPI2 and PB12 CS only
// more compatibility changes are needed
// config for SPI Channel
#if ST7920_SPI == _SPI1
  #define ST7920_SPI_NUM  SPI0
#elif ST7920_SPI == _SPI2
  #define ST7920_SPI_NUM  SPI1
#elif ST7920_SPI == _SPI3
  #define ST7920_SPI_NUM  SPI2
#endif

static CIRCULAR_QUEUE * spi_queue = NULL;

// external interruption arrangement
static inline void SPI_Slave_CS_Config(void)
{
  gpio_exti_source_select(GPIO_EVENT_PORT_GPIOB, GPIO_EVENT_PIN_12);

  exti_init(EXTI_12, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
  exti_interrupt_flag_clear(EXTI_12);
  nvic_irq_enable(EXTI10_15_IRQn, 0U, 1U);
}

static void SPI_ReEnable(uint8_t mode)
{
  SPI_CTL0(ST7920_SPI_NUM) = (0<<15)     // 0:2-line 1: 1-line
                           | (0<<14)     // in bidirectional mode 0:read only 1: read/write
                           | (0<<13)     // 0:disable CRC 1:enable CRC
                           | (0<<12)     // 0:Data phase (no CRC phase) 1:Next transfer is CRC (CRC phase)
                           | (0<<11)     // 0:8-bit data frame 1:16-bit data frame
                           | (1<<10)     // 0:Full duplex 1:Receive-only
                           | (1<<9)      // 0:enable NSS 1:disable NSS (Software slave management)
                           | (0<<8)      // this bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored
                           | (0<<7)      // 0:MSB 1:LSB
                           | (7<<3)      // bit3-5   000:fPCLK/2    001:fPCLK/4    010:fPCLK/8     011:fPCLK/16
                                         //          100:fPCLK/32   101:fPCLK/64   110:fPCLK/128   111:fPCLK/256
                           | (0<<2)      // 0:Slave 1:Master
                           | (mode<<1)   // CPOL
                           | (mode<<0);  // CPHA

  SPI_CTL1(ST7920_SPI_NUM) |= 1<<6;      // RX buffer not empty interrupt enable SPI_I2S_IT_RXNE
}

void SPI_Slave(CIRCULAR_QUEUE * queue)
{
  spi_queue = queue;

  // initializes the initial queue indexes before the queue is used.
  // Otherwise, dirty values will let the system probably freeze when the queue is used
  spi_queue->index_r = spi_queue->index_w = spi_queue->count = 0;

  SPI_Slave_GPIO_Init(ST7920_SPI);
  GPIO_InitSet(PB12, MGPIO_MODE_IPU, 0);   // CS

  nvic_irq_enable(SPI1_IRQn, 1U, 0U);

  rcu_periph_clock_enable(RCU_SPI1);

  SPI_Slave_CS_Config();
  SPI_ReEnable(0);                         // spi mode0

  if ((GPIO_ISTAT(GPIOB) & (1<<12)) != 0)
    SPI_CTL0(ST7920_SPI_NUM) |= (1<<6);
}

void SPI_SlaveDeInit(void)
{
  nvic_irq_disable(SPI1_IRQn);
  nvic_irq_disable(EXTI10_15_IRQn);

  spi_disable(ST7920_SPI_NUM);

  spi_queue = NULL;
}

bool SPI_SlaveGetData(uint8_t * data)
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

void SPI1_IRQHandler(void)
{
  spi_queue->data[spi_queue->index_w] = SPI_DATA(ST7920_SPI_NUM);
  spi_queue->index_w = (spi_queue->index_w + 1) % CIRCULAR_QUEUE_SIZE;
}

#endif  // ST7920_EMULATOR

#ifdef HAS_EMULATOR

// external interruption
void EXTI10_15_IRQHandler(void)
{
  switch (infoSettings.marlin_type)
  {
    #ifdef LCD2004_EMULATOR
      case LCD2004:
        HD44780_writeData();
        break;
    #endif

    #ifdef ST7920_EMULATOR
      case LCD12864:
        if ((GPIO_ISTAT(GPIOB) & (1<<12)) != 0)
        {
          SPI_ReEnable(!!(GPIO_ISTAT(GPIOB) & (1<<13)));  // adaptive spi mode0 / mode3
          SPI_CTL0(ST7920_SPI_NUM) |= (1<<6);
        }
        else
        {
          rcu_periph_reset_enable(RCU_SPI1RST);
          rcu_periph_reset_disable(RCU_SPI1RST);          // reset SPI1
        }

        EXTI_PD = 1<<12;                                  // clear interrupt status register
        break;
    #endif
  }
}

#endif  // HAS_EMULATOR
