#ifndef _STATUS_SCREEN_H_
#define _STATUS_SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "GUI.h"

const GUI_RECT * statusGetMsgRect(void);
void statusSetMsg(const char * title,const char * msg);
void statusSetReady(void);
void menuStatus(void);

#ifdef __cplusplus
}
#endif

#endif
