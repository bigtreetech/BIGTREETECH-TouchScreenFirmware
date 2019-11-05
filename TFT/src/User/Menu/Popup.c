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
  POPUP_RECT_WINDOW,       //弹窗的区域
  10,                      //四角圆弧的半径
  3,                       //外边的线宽
  0x5D7B,                  //外边和标题栏的背景色
  {MAGENTA, 0x5D7B, POPUP_TITLE_HEIGHT},   //标题栏 字体色/背景色/高度
  {WHITE, BLACK, POPUP_TEXT_HEIGHT},    //文本栏 字体色/背景色/高度
  {WHITE, GRAY,  POPUP_BOTTOM_HEIGHT},  //底部 (字体色)/背景色/(高度)
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

void popupReminder(u8* info, u8* context)
{
  popupDrawPage(&bottomSingleBtn , info, context, textSelect(LABEL_CONFIRM), NULL);    
  if(infoMenu.menu[infoMenu.cur] != menuPopup)
  {
    infoMenu.menu[++infoMenu.cur] = menuPopup;
  }
}
