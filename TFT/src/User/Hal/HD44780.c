#include "GPIO_Init.h"
#include "HD44780.h"
#include "stdlib.h"
#include "Settings.h"

#ifdef LCD2004_simulator

HD44780_QUEUE HD44780_queue;

void HD44780_DeConfig(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  free(HD44780_queue.data);
  HD44780_queue.data = 0;
  GPIO_InitSet(LCD_D7, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_D6, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_D5, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_D4, MGPIO_MODE_IPN, 0);
  GPIO_InitSet(LCD_RS, MGPIO_MODE_IPN, 0);
}

/**
* LCD_EN  PB15  4 line mode
* LCD_RS  PB12 
* LCD_D4  PB13 
* LCD_D5  PB14 
* LCD_D6  PC7 
* LCD_D7  PC6 
*/
void HD44780_Config(void)
{
  HD44780_queue.data = malloc(HD44780_data_MAX);
  while(!HD44780_queue.data); // malloc failed

  GPIO_InitSet(LCD_D7, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_D6, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_D5, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_D4, MGPIO_MODE_IPD, 0);
  GPIO_InitSet(LCD_RS, MGPIO_MODE_IPD, 0);

  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Connect GPIOB12 to the interrupt line */
  #if !defined(TFT24_V1_1) 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//Enable SYSCFG clock
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);//PB15 is connected to interrupt line 15
  #else
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12); 
  #endif

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
#endif