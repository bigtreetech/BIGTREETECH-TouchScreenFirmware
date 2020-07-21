#include "GPIO_Init.h"
#include "../HD44780.h"
#include "stdlib.h"
#include "Settings.h"

HD44780_QUEUE HD44780_queue;


void HD44780_DeConfig(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  free(HD44780_queue.data);
  HD44780_queue.data = 0;
  GPIO_InitSet(PC6,  MGPIO_MODE_IPN, 0);
  GPIO_InitSet(PC7,  MGPIO_MODE_IPN, 0);
  GPIO_InitSet(PB14, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(PB13, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(PB12, MGPIO_MODE_IPN, 0);
}

/**
*PC6 -- D7  4 line mode
*PC7 -- D6
*PB14-- D5
*PB13-- D4 
*PB15-- EN
*PB12-- RS
*/
void HD44780_Config(void)
{
  HD44780_queue.data = malloc(HD44780_data_MAX);
  while(!HD44780_queue.data); // malloc failed

  GPIO_InitSet(PC6,  MGPIO_MODE_IPD, 0);
  GPIO_InitSet(PC7,  MGPIO_MODE_IPD, 0);
  GPIO_InitSet(PB14, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(PB13, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(PB12, MGPIO_MODE_IPD, 0);

  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Connect GPIOB12 to the interrupt line */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//Enable SYSCFG clock
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);//PB15 is connected to interrupt line 15

  /*Set interrupt line 12 bit external falling edge interrupt */
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//External interrupt channel where the key is enabled
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//Preemption priority 2?
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//Sub-priority 1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Enable external interrupt channel
  NVIC_Init(&NVIC_InitStructure);
}
