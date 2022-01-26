#include "timer_pwm.h"
#include "includes.h"  // for mcuClocks

typedef struct {
  uint32_t tim;
  uint32_t rcc_src;
  uint8_t apb;
} TIMER;

static TIMER pwmTimer[_TIM_CNT] = {
  {TIMER0,  RCU_TIMER0,  2},   // Timer1  APB2 bit0
  {TIMER1,  RCU_TIMER1,  1},   // Timer2  APB1 bit0
  {TIMER2,  RCU_TIMER2,  1},   // Timer3  APB1 bit1
  {TIMER3,  RCU_TIMER3,  1},   // Timer4  APB1 bit2
  {TIMER4,  RCU_TIMER4,  1},   // Timer5  APB1 bit3
  {TIMER5,  RCU_TIMER5,  1},   // Timer6  APB1 bit4
  {TIMER6,  RCU_TIMER6,  1},   // Timer7  APB1 bit5
  {TIMER7,  RCU_TIMER7,  2},   // Timer8  APB2 bit1
  {TIMER8,  RCU_TIMER8,  2},  // Timer9  APB2 bit16
  {TIMER9,  RCU_TIMER9,  2},  // Timer10 APB2 bit17
  {TIMER10, RCU_TIMER10, 2},  // Timer11 APB2 bit18
  {TIMER11, RCU_TIMER11, 1},   // Timer12 APB1 bit6
  {TIMER12, RCU_TIMER12, 1},   // Timer13 APB1 bit7
  {TIMER13, RCU_TIMER13, 1},   // Timer14 APB1 bit8
};

void TIM_PWM_SetDutyCycle(uint16_t tim_ch, uint8_t duty)
{
  uint16_t timerIndex = TIMER_GET_TIM(tim_ch);
  uint16_t channel = TIMER_GET_CH(tim_ch);
  const TIMER* timer = &pwmTimer[timerIndex];
  switch (channel)
  {
    case 0: TIMER_CH0CV(timer->tim) = duty; break;
    case 1: TIMER_CH1CV(timer->tim) = duty; break;
    case 2: TIMER_CH2CV(timer->tim) = duty; break;
    case 3: TIMER_CH3CV(timer->tim) = duty; break;
  }
}

void TIM_PWM_Init(uint16_t tim_ch)
{
  uint16_t timerIndex = TIMER_GET_TIM(tim_ch);
  uint16_t channel = TIMER_GET_CH(tim_ch);
  const TIMER* timer = &pwmTimer[timerIndex];
  uint32_t timerTmpClk = (timer->apb == 1) ? mcuClocks.PCLK1_Timer_Frequency : mcuClocks.PCLK2_Timer_Frequency;

  rcu_periph_clock_enable(timer->rcc_src);

  // Set PWM frequency to 500Hz
  TIMER_CAR(timer->tim) = 100 - 1;
  TIMER_PSC(timer->tim) = timerTmpClk / (500 * 100) - 1;

  switch (channel)
  {
    case 0: TIMER_CHCTL0(timer->tim) |= (6<<4)  | (1<<3);   break;  // CH1 PWM0 mode
    case 1: TIMER_CHCTL0(timer->tim) |= (6<<12) | (1<<11);  break;  // CH2 PWM0 mode
    case 2: TIMER_CHCTL1(timer->tim) |= (6<<4)  | (1<<3);   break;  // CH3 PWM0 mode
    case 3: TIMER_CHCTL1(timer->tim) |= (6<<12) | (1<<11);  break;  // CH4 PWM0 mode
  }

  TIMER_CHCTL2(timer->tim) |= 1 << (2 * channel);  //enable channle
  TIMER_CTL0(timer->tim) = (1 << 7)   // Auto-reload preload enable
                        | (1 << 0);  // Enbale timer

  if (timer->tim == TIMER0 || timer->tim == TIMER7)
  { // TIM1 & TIM8 advanced timer need config BDTR register for PWM
    TIMER_CCHP(timer->tim) |= 1 << 15;  // Main output enable
  }
}
