#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "menu.h"

#define TOAST_X_PAD          START_X
#define TOAST_Y_PAD          3

#define TOAST_MSG_COUNT       5
#define TOAST_MSG_LENGTH     35
#define TOAST_DISPLAY_LENGTH TOAST_MSG_LENGTH

#define MAX_MSG_COUNT         5
#define MAX_MSG_TITLE_LENGTH 15
#define MAX_MSG_LENGTH       70

typedef struct
{
  DIALOG_TYPE style;
  char title[MAX_MSG_TITLE_LENGTH];
  char text[MAX_MSG_LENGTH];
} NOTIFICATION;

void addToast(DIALOG_TYPE style, char * text);
void drawToast(bool redraw);
bool toastRunning(void);
void loopToast(void);

void addNotification(DIALOG_TYPE style, char * title, char * text, bool drawDialog);
void replayNotification(uint8_t index);
NOTIFICATION * getNotification(uint8_t index);
bool hasNotification(void);
void clearNotification(void);
void setNotificationHandler(void (* handler)());
void titleBarPress(void);

#ifdef __cplusplus
}
#endif

#endif
