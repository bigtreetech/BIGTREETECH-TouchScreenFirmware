#ifndef _SPEED_H_
#define _SPEED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for TFT70_V3_0

#ifdef TFT70_V3_0
  #include <stdint.h>

  void setSpeedItemIndex(uint8_t index);
#endif

void menuSpeed(void);

#ifdef __cplusplus
}
#endif

#endif
