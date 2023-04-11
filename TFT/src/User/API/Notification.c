#include "Notification.h"
#include "includes.h"
#include "my_misc.h"

// area for toast notification
const GUI_RECT toastRect = {START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TOAST_Y_PAD, LCD_WIDTH - START_X, TITLE_END_Y - TOAST_Y_PAD};
const GUI_RECT toastIconRect = {START_X, TOAST_Y_PAD, START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TITLE_END_Y - TOAST_Y_PAD};

// toast notification variables
static TOAST toastlist[TOAST_MSG_COUNT];

static uint8_t nextToastIndex = 0;   // next index to store new toast
static uint8_t curToastDisplay = 0;  // current toast notification being displayed
static uint32_t nextToastTime = 0;   // time to change to next notification

static NOTIFICATION msglist[MAX_MSG_COUNT];
static uint8_t nextMsgIndex = 0;  // next index to store new message
static void (*notificationHandler)() = NULL;

bool _toastRunning = false;

// add new message to toast notification queue
void addToast(DIALOG_TYPE style, char * text)
{
  LCD_WAKE();

  TOAST t;
  strncpy_no_pad(t.text, text, TOAST_MSG_LENGTH);
  t.style = style;
  t.isNew = true;
  toastlist[nextToastIndex] = t;
  nextToastIndex = (nextToastIndex + 1) % TOAST_MSG_COUNT;
}

// check if notification is currently displayed
bool toastRunning(void)
{
  return _toastRunning;
}

// check if any new notification is available
bool toastAvailable(void)
{
  for (int i = 0; i < TOAST_MSG_COUNT; i++)
  {
    if (toastlist[i].isNew == true)
      return true;
  }
  return false;
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
    uint8_t *icon;
    uint8_t cursound;
    if (toastlist[curToastDisplay].style == DIALOG_TYPE_ERROR)
    {
      GUI_SetColor(NOTIF_ICON_ERROR_BG_COLOR);
      icon = IconCharSelect(CHARICON_ERROR);
      cursound = SOUND_ERROR;
    }
    else if (toastlist[curToastDisplay].style == DIALOG_TYPE_SUCCESS)
    {
      GUI_SetColor(NOTIF_ICON_SUCCESS_BG_COLOR);
      icon = IconCharSelect(CHARICON_OK_ROUND);
      cursound = SOUND_SUCCESS;
    }
    else
    {
      GUI_SetColor(NOTIF_ICON_INFO_BG_COLOR);
      icon = IconCharSelect(CHARICON_INFO);
      cursound = SOUND_TOAST;
    }

    if (cursound >= 0 && !redraw)
      BUZZER_PLAY(cursound);

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

    // set new timer if notification is new
    if (!redraw)
      nextToastTime = OS_GetTimeMs() + SEC_TO_MS(TOAST_DURATION);

    GUI_RestoreColorDefault();
  }
}

// check and control toast notification display
void loopToast(void)
{
  if (getMenuType() == MENU_TYPE_FULLSCREEN)
    return;

  if (OS_GetTimeMs() > nextToastTime)
  {
    if (toastAvailable())
    {
      drawToast(false);
    }
    else if (_toastRunning == true)
    {
      _toastRunning = false;
      GUI_ClearPrect(&toastIconRect);
      GUI_ClearPrect(&toastRect);
      menuDrawTitle();
    }
  }
}

// add new message to notification queue
void addNotification(DIALOG_TYPE style, char *title, char *text, bool ShowDialog)
{
  LCD_WAKE();

  if (nextMsgIndex > MAX_MSG_COUNT - 1)
  {
    // remove oldest message and move all messages up one step
    for (int i = 0; i < MAX_MSG_COUNT - 1; i++)
    {
      memcpy(&msglist[i], &msglist[i + 1], sizeof(NOTIFICATION));
    }
    nextMsgIndex = MAX_MSG_COUNT - 1;
  }

  // store message
  msglist[nextMsgIndex].style  = style;
  strncpy_no_pad(msglist[nextMsgIndex].text, text, MAX_MSG_LENGTH);
  strncpy_no_pad(msglist[nextMsgIndex].title, title, MAX_MSG_TITLE_LENGTH);

  if (ShowDialog && MENU_IS_NOT(menuNotification))
    popupReminder(style, (uint8_t *)title, (uint8_t *)msglist[nextMsgIndex].text);

  if (nextMsgIndex < MAX_MSG_COUNT) nextMsgIndex += 1;  //(nextMsgIndex + 1) % MAX_MSG_COUNT;

  if (notificationHandler != NULL)
    notificationHandler();

  notificationDot();

  statusScreen_setMsg((uint8_t *)title, (uint8_t *)text);
}

// replay a notification
void replayNotification(uint8_t index)
{
  if (index < nextMsgIndex)
    popupReminder(msglist[index].style, (uint8_t *)msglist[index].title, (uint8_t *)msglist[index].text);
}

// retrieve a stored notification
NOTIFICATION *getNotification(uint8_t index)
{
  if (strlen(msglist[index].title) > 0 && strlen(msglist[index].text) > 0)
    return &msglist[index];
  else
    return NULL;
}

bool hasNotification(void)
{
  if (nextMsgIndex == 0)
   return false;
  else
    return true;
}

void clearNotification(void)
{
  nextMsgIndex = 0;
  for (int i = 0; i < MAX_MSG_COUNT; i++)
  {
    msglist[i].text[0] = 0;
    msglist[i].title[0] = 0;
  }
  notificationDot();
  statusScreen_setReady();
}

// check if pressed on titlebar area
void titleBarPress(void)
{
  if (getMenuType() == MENU_TYPE_ICON || getMenuType() == MENU_TYPE_LISTVIEW)
  {
    if (MENU_IS_NOT(menuNotification))
    {
      OPEN_MENU(menuNotification);
    }
  }
}

void setNotificationHandler(void (*handler)())
{
  notificationHandler = handler;
}
