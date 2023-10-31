#ifndef _STATUS_SCREEN_H_
#define _STATUS_SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "GUI.h"

extern const GUI_RECT msgRect;

void statusScreen_setMsg(const uint8_t *title,const uint8_t *msg);
void statusScreen_setReady(void);
void menuStatus(void);

#ifdef __cplusplus
}
#endif

#endif
