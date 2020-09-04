#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"

#define TOAST_X_PAD           START_X
#define TOAST_Y_PAD           3

#define TOAST_MSG_COUNT       3
#define TOAST_MSG_LENGTH      35
#define TOAST_DISPLAY_LENGTH  TOAST_MSG_LENGTH

typedef struct
{
  DIALOG_TYPE style;
  uint8_t isNew;
  char text[TOAST_MSG_LENGTH];
}TOAST;

bool toastRunning(void);
void addToast(DIALOG_TYPE style, char * text);
void drawToast(bool redraw);
void loopToast(void);

#ifdef __cplusplus
}
#endif

#endif
