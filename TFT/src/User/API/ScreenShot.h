#ifndef _SCREEN_SHOT_H_
#define _SCREEN_SHOT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool screenShotBMP(char *bmp);
void loopScreenShot(void);

#ifdef __cplusplus
}
#endif

#endif
