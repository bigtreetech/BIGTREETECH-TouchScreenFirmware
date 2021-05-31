#ifndef _CONDITION_CALLBACKS_H_
#define _CONDITION_CALLBACKS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool fullQueueConditionCallback(void);
bool usedQueueConditionCallback(void);
bool noResponseConditionCallback(void);
bool hostPrintingConditionCallback(void);

#ifdef __cplusplus
}
#endif

#endif
