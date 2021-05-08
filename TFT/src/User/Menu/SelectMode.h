#ifndef _SELECT_MODE_H_
#define _SELECT_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "menu.h"

typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
} MKEY_VALUES;

extern bool skipMode;
extern bool freshBoot;

void loopCheckMode(void);
void menuMode(void);
void switchMode(void);

#ifdef __cplusplus
}
#endif

#endif
