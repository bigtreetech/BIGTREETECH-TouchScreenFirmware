#ifndef _SELECTMODE_H_
#define _SELECTMODE_H_

#include "GUI.h"
#include "stdint.h"
#include "stdbool.h"

#define IDLE_TOUCH	0xFFFF
typedef enum
{
  MKEY_0 = 0,
  MKEY_1,
  MKEY_IDLE = IDLE_TOUCH,
}MKEY_VALUES;

extern bool skipMode;

MKEY_VALUES MKeyGetValue(void);
void menuMode(void);
void loopCheckMode(void);

#endif
