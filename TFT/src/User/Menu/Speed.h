#ifndef _SPEED_H_
#define _SPEED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
  uint8_t cur;
  uint8_t set;
} LASTSPEED;

void setSpeedItemIndex(uint8_t index);
void menuSpeed(void);

#ifdef __cplusplus
}
#endif

#endif
