#include "os_timer.h"
#include "includes.h"

volatile struct
{ uint32_t milliseconds;
  uint16_t sec_countdown;
} os_counter = {0, 1000};

void OS_TimerInitMs(void)
{
#ifdef GD32F2XX
  nvic_irq_enable(TIMER6_IRQn, 2U, 0U);

  rcu_periph_clock_enable(RCU_TIMER6);
  TIMER_CAR(TIMER6) = 1000 - 1;
  TIMER_PSC(TIMER6) = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;
  TIMER_INTF(TIMER6) = (uint16_t)~(1<<0);
  TIMER_DMAINTEN(TIMER6) |= 1<<0;
  TIMER_CTL0(TIMER6) |= 0x01;
#else
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->APB1ENR |= 1<<5;
  TIM7->ARR = 1000 - 1;
  TIM7->PSC = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;
  TIM7->SR = (uint16_t)~(1<<0);
  TIM7->DIER |= 1<<0;
  TIM7->CR1 |= 0x01;
#endif
}

#ifdef GD32F2XX
void TIMER6_IRQHandler(void)
{
  if ((TIMER_INTF(TIMER6) & TIMER_INTF_UPIF) != 0)
  {
    os_counter.milliseconds++;
    os_counter.sec_countdown--;

    if (os_counter.sec_countdown == 0)
    {
      os_counter.sec_countdown = 1000;  // 1000 ms is 1 second
      updatePrintTime();  // leave it here, do not move it out

      AVG_KPIS();  // debug monitoring KPI
    }

    checkTouchScreen();  // leave it here, do not move it out

    TIMER_INTF(TIMER6) &= TIMER_INTF_UPIF;  // clear interrupt flag
  }
}
#else
void TIM7_IRQHandler(void)
{
  if ((TIM7->SR & TIM_SR_UIF) != 0)
  {
    os_counter.milliseconds++;
    os_counter.sec_countdown--;

    if (os_counter.sec_countdown == 0)
    {
      os_counter.sec_countdown = 1000;  // 1000 ms is 1 second
      updatePrintTime();  // leave it here, do not move it out

      AVG_KPIS();  // debug monitoring KPI
    }

    checkTouchScreen();  // leave it here, do not move it out

    TIM7->SR &= ~TIM_SR_UIF;  // clear interrupt flag
  }
}
#endif

uint32_t OS_GetTimeMs(void)
{
  return os_counter.milliseconds;
}
