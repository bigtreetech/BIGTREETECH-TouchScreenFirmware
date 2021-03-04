#include "HD44780.h"
#include "GPIO_Init.h"
#include "stdlib.h"
#include "variants.h"
#include "Settings.h"

#ifdef LCD2004_EMULATOR

CIRCULAR_QUEUE *HD44780_queue = NULL;

void HD44780_DeConfig(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  GPIO_InitSet(LCD_D7, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_D6, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_D5, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_D4, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_RS, MGPIO_MODE_IPN, 0);

  HD44780_queue = NULL;
}

/**
* LCD_EN  PB15  4 line mode
* LCD_RS  PB12
* LCD_D4  PB13
* LCD_D5  PB14
* LCD_D6  PC7
* LCD_D7  PC6
*/
void HD44780_Config(CIRCULAR_QUEUE *queue)
{
  HD44780_queue = queue;

  // initializes the initial queue indexes before the queue is used.
  // Otherwise, dirty values will let the system probably freeze when the queue is used
  HD44780_queue->index_r = HD44780_queue->index_w = HD44780_queue->count = 0;

  GPIO_InitSet(LCD_D7, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_D6, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_D5, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_D4, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_RS, MGPIO_MODE_IPD, 0);

  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // Connect GPIOB15 to the interrupt line
  #if !defined(TFT24_V1_1)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);             // Enable SYSCFG clock
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);     // PB15 is connected to interrupt line 15
  #else
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
  #endif

  // Set interrupt line 12 bit external falling edge interrupt
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;               // External interrupt channel where the key is enabled
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;       // Preemption priority 2?
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;              // Sub-priority 1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    // Enable external interrupt channel
  NVIC_Init(&NVIC_InitStructure);
}

bool HD44780_writeData(void)
{
  bool dataWritten = false;

  if ((GPIOB->IDR & (1<<15)) != 0)
  {
    uint8_t temp = ((LCD_D7_PORT->IDR & LCD_D7_PIN) >> 3 ) +         // D7
                   ((LCD_D6_PORT->IDR & LCD_D6_PIN) >> 5 ) +         // D6
                   ((LCD_D5_PORT->IDR & LCD_D5_PIN) >> 13) +         // D5
                   ((LCD_D4_PORT->IDR & LCD_D4_PIN) >> 13) ;         // D4

    if ((GPIOB->IDR & (1<<12)) == 0)
    { //Command received
      temp |= 0x80;
    }

    HD44780_queue->data[HD44780_queue->index_w] = temp;              // Receive HD44780 data
    HD44780_queue->index_w = (HD44780_queue->index_w + 1) % CIRCULAR_QUEUE_SIZE;

    dataWritten = true;
  }

  EXTI->PR = 1<<15;

  return dataWritten;
}

bool HD44780_getData(uint8_t *data)
{
  bool dataRead = false;

  if (HD44780_queue->index_r != HD44780_queue->index_w)
  {
    *data = HD44780_queue->data[HD44780_queue->index_r];
    HD44780_queue->index_r = (HD44780_queue->index_r + 1) % CIRCULAR_QUEUE_SIZE;

    dataRead = true;
  }

  return dataRead;
}

#endif
