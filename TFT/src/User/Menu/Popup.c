#include "Popup.h"
#include "includes.h"


#define BUTTON_NUM 1

BUTTON bottomSingleBtn = {
  //button location                       color before pressed   color after pressed
  POPUP_RECT_SINGLE_CONFIRM, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK
};

BUTTON bottomDoubleBtn[] = {
  {POPUP_RECT_DOUBLE_CONFIRM, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
  {POPUP_RECT_DOUBLE_CANCEL,  NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
};

GUI_RECT doubleBtnRect[] ={POPUP_RECT_DOUBLE_CONFIRM, POPUP_RECT_DOUBLE_CANCEL};


WINDOW window = {
  POPUP_RECT_WINDOW,                      //rectangle position and size of popup window
  10,                                     //Four-corner arc radius
  3,                                      //Outer line width
  0x5D7B,                                 //Outer and title bar background color
  {MAGENTA, 0x5D7B, POPUP_TITLE_HEIGHT},  //Title bar font color / background color / height
  {WHITE, BLACK, POPUP_TEXT_HEIGHT},      //Message area font color / background color / height
  {WHITE, GRAY,  POPUP_BOTTOM_HEIGHT},    //Bottom (font color) / background color / (height)
};

static BUTTON *windowButton =  NULL;
static u16 buttonNum = 0;

void windowReDrawButton(u8 positon, u8 pressed)
{
  if(positon >= buttonNum)            return;
  if(pressed >= 2)                    return;
  if(windowButton == NULL)            return;
  if(windowButton->context == NULL)   return;

  GUI_DrawButton(windowButton + positon, pressed);
}


void popupDrawPage(BUTTON *btn, const u8 *title, const u8 *context, const u8 *yes, const u8 *no)
{
  buttonNum = 0;
  windowButton = btn;
  if(yes)
  {
    windowButton[buttonNum++].context = yes;
  }
  if(no)
  {
    windowButton[buttonNum++].context = no;
  }

  TSC_ReDrawIcon = windowReDrawButton;
  GUI_DrawWindow(&window, title, context);

  for(u8 i = 0; i < buttonNum; i++)
    GUI_DrawButton(&windowButton[i], 0);
}

static const GUI_RECT popupMenuRect = POPUP_RECT_SINGLE_CONFIRM;

void menuPopup(void)
{
  u16 key_num = IDLE_TOUCH;

  while(infoMenu.menu[infoMenu.cur] == menuPopup)
  {
    key_num = KEY_GetValue(BUTTON_NUM, &popupMenuRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }
}

void popupReminder(u8* info, u8* context) {
  #ifdef CLEAN_MODE_SWITCHING_SUPPORT
    if (infoSettings.mode == LCD12864) return;
  #endif
  popupDrawPage(&bottomSingleBtn , info, context, textSelect(LABEL_CONFIRM), NULL);
  if(infoMenu.menu[infoMenu.cur] != menuPopup)
  {
    infoMenu.menu[++infoMenu.cur] = menuPopup;
  }
}

