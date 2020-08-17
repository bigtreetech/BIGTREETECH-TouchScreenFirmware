#include "Popup.h"
#include "includes.h"

BUTTON bottomSingleBtn = {
  //button location                       color before pressed   color after pressed
  POPUP_RECT_SINGLE_CONFIRM, NULL, 5, 1,  DARKGREEN, DARKGREEN,  MAT_LOWWHITE, DARKGREEN, WHITE, DARKGREEN
};

BUTTON bottomDoubleBtn[] = {
  {POPUP_RECT_DOUBLE_CONFIRM, NULL, 5, 1, DARKGREEN, DARKGREEN,  MAT_LOWWHITE, DARKGREEN, WHITE, DARKGREEN},
  {POPUP_RECT_DOUBLE_CANCEL,  NULL, 5, 1, MAT_RED,     MAT_RED,  MAT_LOWWHITE,   MAT_RED, WHITE,   MAT_RED},
};

const GUI_RECT doubleBtnRect[] = {POPUP_RECT_DOUBLE_CONFIRM, POPUP_RECT_DOUBLE_CANCEL};

static const GUI_RECT popupMenuRect = POPUP_RECT_SINGLE_CONFIRM;

WINDOW window = {
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

void windowReDrawButton(u8 positon, u8 pressed)
{
  if (positon >= buttonNum)
    return;
  if (pressed >= 2)
    return;
  if (windowButton == NULL)
    return;
  if (windowButton->context == NULL)
    return;

  GUI_DrawButton(windowButton + positon, pressed);
}

void popupDrawPage(DIALOG_TYPE type, BUTTON *btn, const u8 *title, const u8 *context, const u8 *yes, const u8 *no)
{
  #ifdef LCD_LED_PWM_CHANNEL
    if (infoSettings.lcd_idle_timer != LCD_DIM_OFF)
    {
      //The LCD dim function is activated. First check if it's dimmed
      if (lcd_dim.dimmed)
      {
        lcd_dim.dimmed = false;
        Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
      }
      //Set a new idle_ms time
      lcd_dim.idle_ms = OS_GetTimeMs();
    }
  #endif

  setMenuType(MENU_TYPE_DIALOG);

  if (btn != NULL)                     // set the following global variables only if buttons must be provided.
  {                                    // Otherwise, leave these variables unchanged so current values are maintained
    buttonNum = 0;
    windowButton = btn;

    if (yes)
    {
      windowButton[buttonNum++].context = yes;
    }
    if (no)
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

void menuDialog(void)
{
  u16 key_num = IDLE_TOUCH;
  while (infoMenu.menu[infoMenu.cur] == menuDialog)
  {
    key_num = KEY_GetValue(buttonNum, cur_btn_rect);
    switch (key_num)
    {
      case KEY_POPUP_CONFIRM:
        infoMenu.cur--;
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    if (action_loop != NULL)
      action_loop();

    loopProcess();
  }

  if (action_ok != NULL && key_num == KEY_POPUP_CONFIRM)
    action_ok();
  else if (action_cancel != NULL && key_num == KEY_POPUP_CANCEL)
    action_cancel();
}

void popupReminder(DIALOG_TYPE type, u8* info, u8* context)
{
  if (infoSettings.mode == MARLIN) return;

  //Display the last received popup message, overriding previous popup messages, if any
  popupDrawPage(type, &bottomSingleBtn , info, context, textSelect(LABEL_CONFIRM), NULL);

  //Set (or update on the fly, if menuDialog is already running to handle a popup message) the handlers used by menuDialog.
  action_ok = NULL;
  action_cancel = NULL;
  action_loop = NULL;
  cur_btn_rect = &popupMenuRect;

  //Avoid to nest menuDialog popup type (while a menuNotification popup type can be overridden)
  if (infoMenu.menu[infoMenu.cur] != menuDialog)
  { //Handle the user interaction, then reload the previous menu
    infoMenu.menu[++infoMenu.cur] = menuDialog;
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

  // first, display the last received popup message, overriding previous popup messages, if any
  if (cancel_txt)
  {
    popupDrawPage(type, bottomDoubleBtn, title, msg, ok_txt, cancel_txt);

    cur_btn_rect = doubleBtnRect;
  }
  else
  {
    popupDrawPage(type, &bottomSingleBtn, title, msg, ok_txt, cancel_txt);

    cur_btn_rect = &popupMenuRect;
  }

  // second, set (or update on the fly, if menuDialog is already running to handle a popup message) the handlers used by menuDialog.
  action_ok = ok_action;
  action_cancel = cancel_action;
  action_loop = loop_action;

  // third, avoid to nest menuDialog popup type (while a menuNotification popup type can be overridden)
  if (infoMenu.menu[infoMenu.cur] != menuDialog)
  { // forth, handle the user interaction, then reload the previous menu
    infoMenu.menu[++infoMenu.cur] = menuDialog;
  }
}
