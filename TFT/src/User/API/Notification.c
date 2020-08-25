#include "Notification.h"
#include "includes.h"

//area for toast notification
const GUI_RECT toastRect = {START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TOAST_Y_PAD, LCD_WIDTH - START_X, TITLE_END_Y - TOAST_Y_PAD};
const GUI_RECT toastIconRect = {START_X, TOAST_Y_PAD, START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TITLE_END_Y - TOAST_Y_PAD};

//toast notification variables
static TOAST toastlist[TOAST_MSG_COUNT];

static uint8_t nextToastIndex = 0;   //next index to store new toast
static uint8_t curToastDisplay = 0;  // current toast notification being displayed
static uint32_t nextToastTime = 0;   //time to change to next notification

bool _toastRunning = false;

//Add new message to toast notification queue
void addToast(DIALOG_TYPE style, char * text)
{
  wakeLCD();
  TOAST t;
  strncpy(t.text, text, TOAST_MSG_LENGTH);
  t.text[TOAST_MSG_LENGTH - 1] = 0; //ensure string ends with null terminator
  t.isNew = true;
  toastlist[nextToastIndex] = t;
  nextToastIndex = (nextToastIndex + 1) % TOAST_MSG_COUNT;
}

//check if notification is currently displayed
bool toastRunning(void)
{
  return _toastRunning;
}

//check if any new notification is available
bool toastAvailable(void)
{
  for (int i = 0; i < TOAST_MSG_COUNT; i++)
  {
      if(toastlist[i].isNew == true)
       return true;
  }
  return false;
}

//show next notification
void drawToast(bool redraw)
{
  if(!redraw)
    curToastDisplay = (curToastDisplay + 1) % TOAST_MSG_COUNT;

  if(toastlist[curToastDisplay].isNew == true || redraw)
  {
    //Set toast notification running status
    _toastRunning = true;

    // Draw icon
    uint8_t *icon;
    uint8_t cursound;
    if (toastlist[curToastDisplay].style == DIALOG_TYPE_ERROR)
    {
      GUI_SetColor(MAT_RED);
      icon = IconCharSelect(ICONCHAR_ERROR);
      cursound = sound_error;
    }
    else if (toastlist[curToastDisplay].style == DIALOG_TYPE_SUCCESS)
    {
      GUI_SetColor(MAT_GREEN);
      icon = IconCharSelect(ICONCHAR_OK_ROUND);
      cursound = sound_success;
    }
    else
    {
      GUI_SetColor(MAT_BLUE);
      icon = IconCharSelect(ICONCHAR_INFO);
      cursound = sound_toast;
    }

    if (!redraw)
      BUZZER_PLAY(cursound);

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_FillPrect(&toastIconRect);
    GUI_SetColor(WHITE);
    GUI_DispStringInPrect(&toastIconRect, icon);

    //draw text
    GUI_SetColor(MAT_LOWWHITE);
    GUI_FillPrect(&toastRect);
    GUI_SetColor(DARKGRAY);
    GUI_DispStringInPrect(&toastRect, (u8 *)toastlist[curToastDisplay].text);

    //set current toast notification as old/completed
    toastlist[curToastDisplay].isNew = false;

    //set new timer if notification is new
    if(!redraw)
      nextToastTime = OS_GetTimeMs() + TOAST_DURATION;

    GUI_RestoreColorDefault();
  }
}

//check and control toast notification display
void loopToast(void)
{
  if (OS_GetTimeMs() > nextToastTime)
  {
    if (toastAvailable())
    {
      drawToast(false);
    }
    else if(_toastRunning == true)
    {
    _toastRunning = false;
    GUI_ClearPrect(&toastIconRect);
    GUI_ClearPrect(&toastRect);
      menuReDrawCurTitle();
    }
  }
}
