#include "os_timer.h"
#include "includes.h"

volatile uint32_t os_counter=0;

//
void OS_TimerInit(uint16_t psc, uint16_t arr)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->APB1ENR |= 1<<5;
  TIM7->ARR = arr;
  TIM7->PSC = psc;
  TIM7->SR = (uint16_t)~(1<<0);
  TIM7->DIER |= 1<<0;
  TIM7->CR1 |= 0x01;
}

void TIM7_IRQHandler(void)
{
  if ((TIM7->SR & 0x01) != 0) {   // update interrupt flag
    TIM7->SR = (uint16_t)~(1<<0); // clear interrupt flag

    os_counter++;

    setPrintingTime(os_counter);

    loopTouchScreen();

    if(os_counter == (uint32_t)(~0)) {
      os_counter = 0;
    }
  }
}

/* 1ms */
uint32_t OS_GetTimeMs(void)
{
  return os_counter;
}

/*
 * task: task structure to be filled
 * time_ms:
 */
void OS_TaskInit(OS_TASK *task, uint32_t time_ms, FP_TASK function, void *para)
{
  task->time_ms = time_ms;
  task->task = function;
  task->para = para;
}
/*
*/
void OS_TaskLoop(OS_TASK *task_t)
{
  if(task_t->is_exist == 0)   return;
  if(OS_GetTimeMs() < task_t->next_time)  return;
  if(task_t->is_repeat == 0)
  {
    task_t->is_exist = 0;
  }
  else
  {
    task_t->next_time = OS_GetTimeMs() + task_t->time_ms;
  }
  (*task_t->task)(task_t->para);
}

/*
*/
void OS_TaskEnable(OS_TASK *task_t, uint8_t is_exec,uint8_t is_repeat)
{
  task_t->is_exist =1;
  task_t->is_repeat = is_repeat;
  task_t->next_time = OS_GetTimeMs() + task_t->time_ms;
  if(is_exec)
    (*task_t->task)(task_t->para);
}

/*
*/
void OS_TaskDisable(OS_TASK *task_t)
{
  task_t->is_exist = 0;
}
