#include "timer_pwm.h"
#include "includes.h"  // for mcuClocks

typedef struct {
  TIM_TypeDef * tim;
  volatile uint32_t * rcc_src;
  uint8_t rcc_bit;
} TIMER;

static const TIMER pwmTimer[_TIM_CNT] = {
  {TIM1,  &RCC->APB2ENR, 11},  // Timer1  APB2 bit11
  {TIM2,  &RCC->APB1ENR, 0},   // Timer2  APB1 bit0
  {TIM3,  &RCC->APB1ENR, 1},   // Timer3  APB1 bit1
  {TIM4,  &RCC->APB1ENR, 2},   // Timer4  APB1 bit2
  {TIM5,  &RCC->APB1ENR, 3},   // Timer5  APB1 bit3
  {TIM6,  &RCC->APB1ENR, 4},   // Timer6  APB1 bit4
  {TIM7,  &RCC->APB1ENR, 5},   // Timer7  APB1 bit5
  {TIM8,  &RCC->APB2ENR, 13},  // Timer8  APB2 bit13
};

void TIM_PWM_SetDutyCycle(uint16_t tim_ch, uint8_t duty)
{
  uint16_t timerIndex = TIMER_GET_TIM(tim_ch);
  uint16_t channel = TIMER_GET_CH(tim_ch);
  const TIMER * timer = &pwmTimer[timerIndex];

  switch (channel)
  {
    case 0: timer->tim->CCR1 = duty; break;
    case 1: timer->tim->CCR2 = duty; break;
    case 2: timer->tim->CCR3 = duty; break;
    case 3: timer->tim->CCR4 = duty; break;
  }
}

void TIM_PWM_Init(uint16_t tim_ch)
{
  uint16_t timerIndex = TIMER_GET_TIM(tim_ch);
  uint16_t channel = TIMER_GET_CH(tim_ch);
  const TIMER * timer = &pwmTimer[timerIndex];
  uint32_t timerTmpClk = (timer->rcc_src == &RCC->APB1ENR) ? mcuClocks.PCLK1_Timer_Frequency : mcuClocks.PCLK2_Timer_Frequency;

  *timer->rcc_src |= (1 << timer->rcc_bit);  // enable timer clock

  // set PWM frequency to 500Hz
  timer->tim->ARR = 100 - 1;
  timer->tim->PSC = timerTmpClk / (500 * 100) - 1;

  switch (channel)
  {
    case 0: timer->tim->CCMR1 |= (6<<4)  | (1<<3);  break;  // CH1 PWM1 mode
    case 1: timer->tim->CCMR1 |= (6<<12) | (1<<11); break;  // CH2 PWM1 mode
    case 2: timer->tim->CCMR2 |= (6<<4)  | (1<<3);  break;  // CH3 PWM1 mode
    case 3: timer->tim->CCMR2 |= (6<<12) | (1<<11); break;  // CH4 PWM1 mode
  }

  timer->tim->CCER |= 1 << (4 * channel);  // enable channle
  timer->tim->CR1 = (1 << 7)               // auto-reload preload enable
                  | (1 << 0);              // enbale timer

  if (timer->tim == TIM1 || timer->tim == TIM8)
  { // TIM1 & TIM8 advanced timer need config BDTR register for PWM
    timer->tim->BDTR |= 1 << 15;  // main output enable
  }
}
