#ifndef _SCREEN_SHOT_H_
#define _SCREEN_SHOT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"  // for SCREEN_SHOT_TO_SD etc...

#ifdef SCREEN_SHOT_TO_SD
  bool screenShotBMP(char *bmp);
  void loopScreenShot(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
