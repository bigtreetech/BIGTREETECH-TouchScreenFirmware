#ifndef _FAN_H_
#define _FAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
  uint8_t cur;
  uint8_t set;
} LASTFAN;

void menuFan(void);

#ifdef __cplusplus
}
#endif

#endif
