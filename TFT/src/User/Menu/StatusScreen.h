#ifndef _STATUS_SCREEN_H_
#define _STATUS_SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "GUI.h"

extern const GUI_RECT msgRect;

void menuStatus(void);
void statusScreenSetMsg(const uint8_t *title,const uint8_t *msg);
void statusScreenSetReady(void);

#ifdef __cplusplus
}
#endif

#endif
