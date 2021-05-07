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

#define TOAST_MSG_COUNT      3
#define TOAST_MSG_LENGTH     35
#define TOAST_DISPLAY_LENGTH TOAST_MSG_LENGTH

#define MAX_MSG_COUNT        3
#define MAX_MSG_TITLE_LENGTH 15
#define MAX_MSG_LENGTH       70

typedef struct
{
  DIALOG_TYPE style;
  uint8_t isNew;
  char text[TOAST_MSG_LENGTH];
} TOAST;

typedef struct
{
  DIALOG_TYPE style;
  char title[MAX_MSG_TITLE_LENGTH];
  char text[MAX_MSG_LENGTH];
} NOTIFICATION;

bool toastRunning(void);
void addToast(DIALOG_TYPE style, char * text);
void drawToast(bool redraw);
void loopToast(void);
void addNotification(DIALOG_TYPE style, char * title, char * text, bool ShowDialog);
void replayNotification(uint8_t index);
NOTIFICATION * getNotification(uint8_t index);
bool hasNotification(void);
void titleBarPress(void);
void clearNotification(void);
void setNotificationHandler(void (*handler)());

#ifdef __cplusplus
}
#endif

#endif
