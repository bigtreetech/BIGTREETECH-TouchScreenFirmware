#ifndef _POPUP_H_
#define _POPUP_H_

#include "GUI.h"
#include "variants.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

extern BUTTON bottomSingleBtn;
extern BUTTON bottomDoubleBtn[];
extern const GUI_RECT doubleBtnRect[];
extern WINDOW window;

void windowSetButton(const BUTTON *btn);
void windowReDrawButton(uint8_t positon, uint8_t is_press);
void popupDrawPage(DIALOG_TYPE type, BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
void popupReminder(DIALOG_TYPE type, u8* info, u8* context);
void popupPauseForUser(void);
void showDialog(DIALOG_TYPE type, u8 * title, u8 * msg, u8 *ok_txt, u8* cancel_txt, void (*ok_action)(), void (*cancel_action)(), void (*loop_action)());

#endif
