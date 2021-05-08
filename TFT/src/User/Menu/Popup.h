#ifndef _POPUP_H_
#define _POPUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"
#include "GUI.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
};

extern const GUI_RECT doubleBtnRect[];
extern BUTTON bottomDoubleBtn[];

void _setDialogTitleStr(uint8_t * str);
void _setDialogMsgStr(uint8_t * str);
uint8_t *getDialogMsgStr(void);
void _setDialogOkTextStr(uint8_t * str);
void _setDialogCancelTextStr(uint8_t * str);

void _setDialogTitleLabel(int16_t index);
void _setDialogMsgLabel(int16_t index);
void _setDialogOkTextLabel(int16_t index);
void _setDialogCancelTextLabel(int16_t index);

#define setDialogTitle(x) _Generic(((x+0)), const uint8_t*: _setDialogTitleStr, \
                                                  uint8_t*: _setDialogTitleStr, \
                                                  default: _setDialogTitleLabel)(x)
#define setDialogMsg(x) _Generic(((x+0)), const uint8_t*: _setDialogMsgStr, \
                                                uint8_t*: _setDialogMsgStr, \
                                                default: _setDialogMsgLabel)(x)
#define setDialogOkText(x) _Generic(((x+0)), const uint8_t*: _setDialogOkTextStr, \
                                                   uint8_t*: _setDialogOkTextStr, \
                                                   default: _setDialogOkTextLabel)(x)
#define setDialogCancelText(x)  _Generic(((x+0)), const uint8_t*: _setDialogCancelTextStr, \
                                                        uint8_t*: _setDialogCancelTextStr, \
                                                        default: _setDialogCancelTextLabel)(x)

//set text from LABEL index or pointer (uint8_t*)
#define setDialogText(title, msg, oktext, canceltext) \
  {                                                   \
    setDialogTitle(title);                            \
    setDialogMsg(msg);                                \
    setDialogOkText(oktext);                          \
    setDialogCancelText(canceltext);                  \
  }

void popupDrawPage(DIALOG_TYPE type, BUTTON * btn, const uint8_t * title, const uint8_t * context, const uint8_t * yes,
                    const uint8_t * no);
//void popupReminder(DIALOG_TYPE type, uint8_t* title, uint8_t* msg);
void menuDialog(void);
void showDialog(DIALOG_TYPE type, void (*ok_action)(), void (*cancel_action)(), void (*loop_action)());
void loopPopup(void);

#define popupReminder(_type, _title, _msg)                        \
  {                                                               \
    setDialogText(_title, _msg, LABEL_CONFIRM, LABEL_BACKGROUND); \
    showDialog(_type, NULL, NULL, NULL);                          \
  }

#ifdef __cplusplus
}
#endif

#endif
