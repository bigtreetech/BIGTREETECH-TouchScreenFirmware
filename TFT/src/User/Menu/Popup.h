#ifndef _POPUP_H_
#define _POPUP_H_

#include "GUI.h"
#include "variants.h"

//TODO:fix here for more resolution
#if LCD_WIDTH == 320 && LCD_HEIGHT == 240
  #define POPUP_RECT_WINDOW         {30, 25, 290, 215}

  #define POPUP_RECT_SINGLE_CONFIRM {110,  165, 210, 205}
  #define POPUP_RECT_DOUBLE_CONFIRM {40,  165, 140, 205}
  #define POPUP_RECT_DOUBLE_CANCEL  {180, 165, 280, 205}

  #define POPUP_TITLE_HEIGHT  30
  #define POPUP_TEXT_HEIGHT   100
  #define POPUP_BOTTOM_HEIGHT 50
#elif LCD_WIDTH == 480 && LCD_HEIGHT == 320
  #define POPUP_RECT_WINDOW         {80, 50, 400, 270}

  #define POPUP_RECT_SINGLE_CONFIRM {180, 210, 300, 260}
  #define POPUP_RECT_DOUBLE_CONFIRM {90,  210, 210, 260}
  #define POPUP_RECT_DOUBLE_CANCEL  {270, 210, 390, 260}

  #define POPUP_TITLE_HEIGHT  40
  #define POPUP_TEXT_HEIGHT   110
  #define POPUP_BOTTOM_HEIGHT 70
#endif

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

extern BUTTON bottomSingleBtn;
extern BUTTON bottomDoubleBtn[];
extern GUI_RECT doubleBtnRect[];
extern WINDOW window;

void windowSetButton(const BUTTON *btn);
void windowReDrawButton(uint8_t positon, uint8_t is_press);
void popupDrawPage(BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
void popupReminder(u8* info, u8* context);

#endif
