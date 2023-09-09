#ifndef _OS_TIMER_H_
#define _OS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*FP_TASK)(void *);

void OS_TimerInitMs(void);
uint32_t OS_GetTimeMs(void);

#ifdef __cplusplus
}
#endif

#endif
