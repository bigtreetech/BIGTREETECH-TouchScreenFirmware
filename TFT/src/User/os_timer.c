#include "os_timer.h"
#include "includes.h"

u32 os_counter=0;

//用于产生系统时钟，本工程设置10ms中断一次
void OS_TimerInit(u16 psc,u16 arr)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->APB1ENR|=1<<2;	           //TIM4时钟使能
  TIM4->ARR=arr;  	             //设定自动重装值
  TIM4->PSC=psc;  	             //预分频器
  TIM4->SR = (uint16_t)~(1<<0);  //清除更新中断
  TIM4->DIER|=1<<0;              //允许更新中断
  TIM4->CR1|=0x01;               //使能定时器3
}

void TIM4_IRQHandler(void)   //TIM4中断
{
  if ((TIM4->SR&0x01) != 0) //检查指定的TIM中断发生与否:TIM 中断源
  {
    os_counter++;
    //打印过程中统计打印耗时
    setPrintingTime(os_counter);

    loopTouchScreen();

    //计时溢出
    if(os_counter==(1<<30))
    {
      os_counter=0;
    }

    TIM4->SR = (uint16_t)~(1<<0);  //清除TIMx的中断待处理位:TIM 中断源
  }
}

/* 系统时间 单位是 10ms */
u32 OS_GetTime(void)
{
  return os_counter;
}

/*
功能:任务结构体初始化
参数:
task_t:待填充的任务结构体
time:任务多久执行一次
task:待执行的任务
para:待执行任务的参数
*/
void OS_TaskInit(OS_TASK *task, u32 time, FP_TASK function, void *para)
{
  task->time = time;
  task->task = function;
  task->para = para;
}
/*
功能:在主循环中调用,检测是否需要执行任务
参数:待执行任务对应的任务结构体
*/
void OS_TaskCheck(OS_TASK *task_t)
{
  if(task_t->is_exist==0)   return;
  if(OS_GetTime()<task_t->start_time+task_t->time)  return;
  if(task_t->is_repeat==0)
  {
    task_t->is_exist=0;
  }
  else
  {
    task_t->start_time=OS_GetTime();
  }
  (*task_t->task)(task_t->para);
}

/*
功能:使能 task_t 对应的任务
参数:
task_t:待执行任务对应的任务结构体
is_exec:此任务是否立即执行
is_repeat：此任务是否重复执行
*/
void OS_TaskEnable(OS_TASK *task_t,u8 is_exec,u8 is_repeat)
{
  task_t->is_exist=1;
  task_t->is_repeat=is_repeat;
  task_t->start_time=OS_GetTime();
  if(is_exec)
    (*task_t->task)(task_t->para);
}

/*
功能:失能 task_t 对应的任务
参数:待执行任务对应的任务结构体
*/
void OS_TaskDisable(OS_TASK *task_t)
{
  task_t->is_exist=0;
}
