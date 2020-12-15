#ifndef _SELECTMODE_H_
#define _SELECTMODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "GUI.h"

typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
}MKEY_VALUES;

extern bool skipMode;

void menuMode(void);
void loopCheckMode(void);

#ifdef __cplusplus
}
#endif

#endif
