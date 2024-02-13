#ifndef _OS_TIMER_H_
#define _OS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// NOTE: not needed to define "ms" attribute as "volatile"
typedef struct
{
  uint32_t ms;   // milliseconds
  uint16_t sec;  // seconds
} OS_COUNTER;

typedef void (*FP_TASK)(void *);

typedef struct
{
  uint32_t time_ms;
  uint32_t next_time;
  FP_TASK  task;
  void     *para;
  uint8_t  is_exist;
  uint8_t  is_repeat;
} OS_TASK;

extern OS_COUNTER os_counter;

void OS_InitTimerMs(void);

static inline uint32_t OS_GetTimeMs(void)
{
  return os_counter.ms;
}

void OS_TaskInit(OS_TASK *task, uint32_t time_ms, FP_TASK function, void *para);
void OS_TaskLoop(OS_TASK *task);
void OS_TaskEnable(OS_TASK *task, uint8_t is_exec, uint8_t is_repeat);
void OS_TaskDisable(OS_TASK *task);

#ifdef __cplusplus
}
#endif

#endif
