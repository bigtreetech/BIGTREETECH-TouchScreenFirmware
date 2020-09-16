#include "Popup.h"
#include "includes.h"

static BUTTON bottomSingleBtn = {
  //button location                       color before pressed   color after pressed
  POPUP_RECT_SINGLE_CONFIRM, NULL, 5, 1,  DARKGREEN, DARKGREEN,  MAT_LOWWHITE, DARKGREEN, WHITE, DARKGREEN
};

BUTTON bottomDoubleBtn[] = {
  {POPUP_RECT_DOUBLE_CONFIRM, NULL, 5, 1, DARKGREEN, DARKGREEN,  MAT_LOWWHITE, DARKGREEN, WHITE, DARKGREEN},
  {POPUP_RECT_DOUBLE_CANCEL,  NULL, 5, 1, MAT_RED,     MAT_RED,  MAT_LOWWHITE,   MAT_RED, WHITE,   MAT_RED},
};

const GUI_RECT doubleBtnRect[] = {POPUP_RECT_DOUBLE_CONFIRM, POPUP_RECT_DOUBLE_CANCEL};
static const GUI_RECT singleBtnRect = POPUP_RECT_SINGLE_CONFIRM;

static WINDOW window = {
  DIALOG_TYPE_INFO,             //default window type
  POPUP_RECT_WINDOW,            //rectangle position and size of popup window
  POPUP_TITLE_HEIGHT,           //height of title bar
  POPUP_BOTTOM_HEIGHT,          //height of action bar
  2,                            //window border width
  GRAY,                         //window border color
  {DARKGRAY, MAT_LOWWHITE},     //Title bar font color / background color
  {DARKGRAY, MAT_LOWWHITE},     //Message area font color / background color
  {DARKGRAY, MAT_LOWWHITE},     //actionbar font color / background color
};

static BUTTON *windowButton =  NULL;
static u16 buttonNum = 0;

static const GUI_RECT * cur_btn_rect = NULL;
static void (*action_ok)() = NULL;
static void (*action_cancel)() = NULL;
static void (*action_loop)() = NULL;

static bool popup_redraw = false;
#define X_MAX_CHAR (LCD_WIDTH / BYTE_WIDTH)
#define FULL_SCREEN_MAX_CHAR (LCD_WIDTH / BYTE_WIDTH * LCD_HEIGHT /BYTE_HEIGHT)
static uint8_t popup_title[X_MAX_CHAR];
static uint8_t popup_msg[FULL_SCREEN_MAX_CHAR];
static uint8_t popup_ok[24];
static uint8_t popup_cancel[24];
static DIALOG_TYPE popup_type;

void windowReDrawButton(u8 position, u8 pressed)
{
  if (position >= buttonNum)
    return;
  if (pressed >= 2)
    return;
  if (windowButton == NULL)
    return;
  if (windowButton->context == NULL)
    return;

  GUI_DrawButton(windowButton + position, pressed);
}

void popupDrawPage(DIALOG_TYPE type, BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no)
{
  setMenuType(MENU_TYPE_DIALOG);

  if (btn != NULL)                     // set the following global variables only if buttons must be provided.
  {                                    // Otherwise, leave these variables unchanged so current values are maintained
    buttonNum = 0;
    windowButton = btn;

    if (yes && yes[0])
    {
      windowButton[buttonNum++].context = yes;
    }
    if (no && no[0])
    {
      windowButton[buttonNum++].context = no;
    }
  }

  TSC_ReDrawIcon = windowReDrawButton;
  window.type = type;

  if (btn != NULL)                     // draw a window with buttons bar
  {
    GUI_DrawWindow(&window, title, context, true);

    for(u8 i = 0; i < buttonNum; i++)
      GUI_DrawButton(&windowButton[i], 0);
  }
  else                                 // draw a window with no buttons bar
  {
    GUI_DrawWindow(&window, title, context, false);
  }
}

static void menuDialog(void)
{
  while (infoMenu.menu[infoMenu.cur] == menuDialog)
  {
    uint16_t key_num = KEY_GetValue(buttonNum, cur_btn_rect);
    switch (key_num)
    {
      case KEY_POPUP_CONFIRM:
        infoMenu.cur--;
        if (action_ok != NULL)
          action_ok();
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        if (action_cancel != NULL)
          action_cancel();
        break;

      default:
        break;
    }

    if (action_loop != NULL)
      action_loop();

    loopProcess();
  }
}

void popup_strcpy(uint8_t *dst, uint8_t *src, uint16_t size)
{
  if (src)
  {
    strncpy((char *)dst, (char *)src, size);
    dst[size - 1] = 0;
  }
  else
  {
    dst[0] = 0;
  }
}
/** Show save setting dialog
 * @param title - the title to show on window (title of which menu asked fot it?)
 * @param msg - the msg to to show in msg box
 * @param title - the title to show on window (title of which menu asked fot it?)
 * @param msg - the msg to to show in msg box
 * @param ok_action - pointer to a function to perform if ok is pressed. (pass NULL if no action need to be performed)
 * @param cancel_action - pointer to a function to perform if Cancel is pressed.(pass NULL if no action need to be performed)
*/
void showDialog(DIALOG_TYPE type, u8 * title, u8 * msg, u8 *ok_txt, u8* cancel_txt, void (*ok_action)(), void (*cancel_action)(), void (*loop_action)())
{
  if (infoSettings.mode == MARLIN)
    return;

  popup_redraw = true;
  popup_type = type;

  popup_strcpy(popup_title, title, sizeof(popup_title));
  popup_strcpy(popup_msg, msg, sizeof(popup_msg));
  popup_strcpy(popup_ok, ok_txt, sizeof(popup_ok));
  popup_strcpy(popup_cancel, cancel_txt, sizeof(popup_cancel));

  action_ok = ok_action;
  action_cancel = cancel_action;
  action_loop = loop_action;
}

void popupReminder(DIALOG_TYPE type, u8* title, u8* msg)
{
  showDialog(type, title, msg, textSelect(LABEL_CONFIRM), NULL, NULL, NULL, NULL);
}

void loopPopup(void)
{
  if (popup_redraw == false)
    return;

  popup_redraw = false;

  wakeLCD();

  //display the last received popup message, overriding previous popup messages, if any
  if (popup_cancel[0])
  {
    popupDrawPage(popup_type, bottomDoubleBtn, popup_title, popup_msg, popup_ok, popup_cancel);
    cur_btn_rect = doubleBtnRect;
  }
  else
  {
    popupDrawPage(popup_type, &bottomSingleBtn, popup_title, popup_msg, popup_ok, NULL);
    cur_btn_rect = &singleBtnRect;
  }
  //avoid to nest menuDialog popup type (while a menuNotification popup type can be overridden)
  if (infoMenu.menu[infoMenu.cur] != menuDialog)
  { //handle the user interaction, then reload the previous menu
    infoMenu.menu[++infoMenu.cur] = menuDialog;
  }
}
