#include "menu.h"

typedef struct
{
  DIALOG_TYPE style;
  uint8_t     isNew;
  char        text[TOAST_MSG_LENGTH];
}TOAST;

bool toastRunning(void);
void addToast(DIALOG_TYPE style, char * text);
void drawToast(bool redraw);
void loopToast(void);
