#include "os_timer.h"
#include "includes.h"

OS_COUNTER os_counter = {0, 1000};

void OS_InitTimerMs(void)
{
#if defined(GD32F2XX) || defined(GD32F3XX)
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

#if defined(GD32F2XX) || defined(GD32F3XX)
void TIMER6_IRQHandler(void)
{
  if ((TIMER_INTF(TIMER6) & TIMER_INTF_UPIF) != 0)
  {
    TIMER_INTF(TIMER6) &= ~TIMER_INTF_UPIF;  // clear interrupt flag

    os_counter.ms++;
    os_counter.sec--;

    if (os_counter.sec == 0)  // if one second has been elapsed
    {
      os_counter.sec = 1000;  // reset one second counter

      AVG_KPIS();             // collect debug monitoring KPI

      updatePrintTime();      // if printing, update printing info
    }

    TS_CheckPress();  // check touch screen once a millisecond
  }
}
#else
void TIM7_IRQHandler(void)
{
  if ((TIM7->SR & TIM_SR_UIF) != 0)
  {
    TIM7->SR &= ~TIM_SR_UIF;  // clear interrupt flag

    os_counter.ms++;
    os_counter.sec--;

    if (os_counter.sec == 0)  // if one second has been elapsed
    {
      os_counter.sec = 1000;  // reset one second counter

      AVG_KPIS();             // collect debug monitoring KPI

      updatePrintTime();      // if printing, update printing info
    }

    TS_CheckPress();  // check touch screen once a millisecond
  }
}
#endif

// task: task structure to be filled
// time_ms:
//
void OS_TaskInit(OS_TASK *task_t, uint32_t time_ms, FP_TASK function, void *para)
{
  task_t->time_ms = time_ms;
  task_t->task = function;
  task_t->para = para;
}

void OS_TaskLoop(OS_TASK *task_t)
{
  if (task_t->is_exist == 0)
    return;

  if (OS_GetTimeMs() < task_t->next_time)
    return;

  if (task_t->is_repeat == 0)
  {
    task_t->is_exist = 0;
  }
  else
  {
    task_t->next_time = OS_GetTimeMs() + task_t->time_ms;
  }

  (*task_t->task)(task_t->para);
}

void OS_TaskEnable(OS_TASK *task_t, uint8_t is_exec, uint8_t is_repeat)
{
  task_t->is_exist =1;
  task_t->is_repeat = is_repeat;
  task_t->next_time = OS_GetTimeMs() + task_t->time_ms;

  if (is_exec)
    (*task_t->task)(task_t->para);
}

void OS_TaskDisable(OS_TASK *task_t)
{
  task_t->is_exist = 0;
}
