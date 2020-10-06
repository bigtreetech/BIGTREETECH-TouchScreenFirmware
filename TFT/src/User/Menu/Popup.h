#ifndef _POPUP_H_
#define _POPUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "GUI.h"
#include "variants.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};
enum
{
  KEY_TEXT_OK = 0,
  KEY_TEXT_CANCEL,
  KEY_TEXT_
};

extern const GUI_RECT doubleBtnRect[];
extern BUTTON bottomDoubleBtn[];

void _setDialogTitleStr(uint8_t * str);
void _setDialogMsgStr(uint8_t * str);
void _setDialogOkTextStr(uint8_t * str);
void _setDialogCancelTextStr(uint8_t * str);

void _setDialogTitleLabel(int16_t index);
void _setDialogMsgLabel(int16_t index);
void _setDialogOkTextLabel(int16_t index);
void _setDialogCancelTextLabel(int16_t index);

#define setDialogTitle(x)       _Generic(((x+0)), const u8*: _setDialogTitleStr, u8*: _setDialogTitleStr, default: _setDialogTitleLabel)(x)
#define setDialogMsg(x)         _Generic(((x+0)), const u8*: _setDialogMsgStr, u8*: _setDialogMsgStr, default: _setDialogMsgLabel)(x)
#define setDialogOkText(x)      _Generic(((x+0)), const u8*: _setDialogOkTextStr, u8*: _setDialogOkTextStr, default: _setDialogOkTextLabel)(x)
#define setDialogCancelText(x)  _Generic(((x+0)), const u8*: _setDialogCancelTextStr, u8*: _setDialogCancelTextStr, default: _setDialogCancelTextLabel)(x)

//set text from LABEL index or pointer (uint8_t*)
#define setDialogText(title, msg, oktext, canceltext) {setDialogTitle(title); setDialogMsg(msg); setDialogOkText(oktext); setDialogCancelText(canceltext);}

void popupDrawPage(DIALOG_TYPE type, BUTTON *btn, const uint8_t *title, const uint8_t *context, const uint8_t *yes, const uint8_t *no);
//void popupReminder(DIALOG_TYPE type, u8* title, u8* msg);
void showDialog(DIALOG_TYPE type, void (*ok_action)(), void (*cancel_action)(), void (*loop_action)());
void loopPopup(void);

#define popupReminder(_type, _title, _msg) {setDialogText(_title, _msg, LABEL_CONFIRM, LABEL_BACKGROUND); showDialog(_type, NULL, NULL, NULL);}

#ifdef __cplusplus
}
#endif

#endif
