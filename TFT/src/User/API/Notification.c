#include "Notification.h"
#include "includes.h"
#include "my_misc.h"

// area for toast notification
const GUI_RECT toastRect = {START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TOAST_Y_PAD, LCD_WIDTH - START_X, TITLE_END_Y - TOAST_Y_PAD};
const GUI_RECT toastIconRect = {START_X, TOAST_Y_PAD, START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TITLE_END_Y - TOAST_Y_PAD};

typedef struct
{
  DIALOG_TYPE style;
  uint8_t isNew;
  char text[TOAST_MSG_LENGTH];
} TOAST;

// toast notification variables
static TOAST toastlist[TOAST_MSG_COUNT];  // toast notification array
static bool _toastRunning = false;        // "true" when a toast notification is currently displayed
static bool _toastAvailable = false;      // "true" when a new toast is added. "false" when no more toasts to display are available
static uint8_t nextToastIndex = 0;        // next index to store new toast
static uint8_t curToastDisplay = 0;       // current toast notification being displayed
static uint32_t nextToastTime = 0;        // time to change to next toast notification

// message notification variables
static NOTIFICATION msglist[MAX_MSG_COUNT];    // message notification array
static uint8_t nextMsgIndex = 0;               // next index to store new message
static void (* notificationHandler)() = NULL;  // message notification handler

// add new message to toast notification queue
void addToast(DIALOG_TYPE style, char * text)
{
  LCD_WAKE();

  strncpy_no_pad(toastlist[nextToastIndex].text, text, TOAST_MSG_LENGTH);
  toastlist[nextToastIndex].style = style;
  toastlist[nextToastIndex].isNew = true;

  _toastAvailable = true;
  nextToastIndex = (nextToastIndex + 1) % TOAST_MSG_COUNT;
}

// show next notification
void drawToast(bool redraw)
{
  if (!redraw)
    curToastDisplay = (curToastDisplay + 1) % TOAST_MSG_COUNT;

  if (toastlist[curToastDisplay].isNew == true || redraw)
  {
    // set toast notification running status
    _toastRunning = true;

    // draw icon
    uint8_t * icon;
    SOUND curSound;

    switch (toastlist[curToastDisplay].style)
    {
      case DIALOG_TYPE_ERROR:
        GUI_SetColor(NOTIF_ICON_ERROR_BG_COLOR);
        icon = IconCharSelect(CHARICON_ERROR);
        curSound = SOUND_ERROR;
        break;

      case DIALOG_TYPE_SUCCESS:
        GUI_SetColor(NOTIF_ICON_SUCCESS_BG_COLOR);
        icon = IconCharSelect(CHARICON_OK_ROUND);
        curSound = SOUND_SUCCESS;
        break;

      default:
        GUI_SetColor(NOTIF_ICON_INFO_BG_COLOR);
        icon = IconCharSelect(CHARICON_INFO);
        curSound = SOUND_TOAST;
        break;
    }

    if (!redraw)  // if notification is new
    {
      BUZZER_PLAY(curSound);  // play sound
      nextToastTime = OS_GetTimeMs() + SEC_TO_MS(TOAST_DURATION);  // set new timer
    }

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_FillPrect(&toastIconRect);
    GUI_SetColor(NOTIF_ICON_FG_COLOR);
    GUI_DispStringInPrect(&toastIconRect, icon);

    // draw text
    GUI_SetColor(NOTIF_TEXT_BG_COLOR);
    GUI_FillPrect(&toastRect);
    GUI_SetColor(NOTIF_TEXT_FONT_COLOR);
    GUI_DispStringInPrect(&toastRect, (uint8_t *)toastlist[curToastDisplay].text);

    // set current toast notification as old/completed
    toastlist[curToastDisplay].isNew = false;

    GUI_RestoreColorDefault();
  }
}

// check if notification is currently displayed
bool toastRunning(void)
{
  return _toastRunning;
}

// check if any new notification is available
static inline bool toastAvailable(void)
{
  for (int i = 0; i < TOAST_MSG_COUNT; i++)
  {
    if (toastlist[i].isNew == true)
      return true;
  }

  return false;
}

// check and control toast notification display
void loopToast(void)
{
  // if no new toast is available or it is not yet expired on screen or in case a full screen menu is displayed, do nothing
  if (_toastAvailable == false || OS_GetTimeMs() < nextToastTime || getMenuType() == MENU_TYPE_FULLSCREEN)
    return;

  if (toastAvailable())
  {
    drawToast(false);
  }
  else if (_toastRunning == true)
  {
    _toastRunning = false;
    _toastAvailable = false;
    GUI_ClearPrect(&toastIconRect);
    GUI_ClearPrect(&toastRect);
    menuDrawTitle();
  }
}

// add new message to notification queue
void addNotification(DIALOG_TYPE style, char * title, char * text, bool drawDialog)
{
  LCD_WAKE();

  if (nextMsgIndex >= MAX_MSG_COUNT)  // if no more available slots, skip the oldest notification
  {
    // remove oldest message and move all messages up one step
    for (int i = 0; i < MAX_MSG_COUNT - 1; i++)
    {
      memcpy(&msglist[i], &msglist[i + 1], sizeof(NOTIFICATION));
    }

    nextMsgIndex = MAX_MSG_COUNT - 1;
  }

  // store message
  msglist[nextMsgIndex].style = style;
  strncpy_no_pad(msglist[nextMsgIndex].text, text, MAX_MSG_LENGTH);
  strncpy_no_pad(msglist[nextMsgIndex].title, title, MAX_MSG_TITLE_LENGTH);
  nextMsgIndex++;

  if (drawDialog && MENU_IS_NOT(menuNotification))
    popupReminder(style, (uint8_t *)title, (uint8_t *)text);

  if (notificationHandler != NULL)
    notificationHandler();

  notificationDot();

  statusSetMsg((uint8_t *)title, (uint8_t *)text);
}

// replay a notification
void replayNotification(uint8_t index)
{
  if (index < nextMsgIndex)
    popupReminder(msglist[index].style, (uint8_t *)msglist[index].title, (uint8_t *)msglist[index].text);
}

// retrieve a stored notification
NOTIFICATION * getNotification(uint8_t index)
{
  if (msglist[index].title[0] != '\0' && msglist[index].text[0] != '\0')
    return &msglist[index];
  else
    return NULL;
}

bool hasNotification(void)
{
  return (nextMsgIndex != 0);
}

void clearNotification(void)
{
  nextMsgIndex = 0;

  for (int i = 0; i < MAX_MSG_COUNT; i++)
  {
    msglist[i].text[0] = '\0';
    msglist[i].title[0] = '\0';
  }

  notificationDot();
  statusSetReady();
}

void setNotificationHandler(void (* handler)())
{
  notificationHandler = handler;
}

// check if pressed on titlebar area
void titleBarPress(void)
{
  if (getMenuType() == MENU_TYPE_ICON || getMenuType() == MENU_TYPE_LISTVIEW)
  {
    if (MENU_IS_NOT(menuNotification))
      OPEN_MENU(menuNotification);
  }
}
