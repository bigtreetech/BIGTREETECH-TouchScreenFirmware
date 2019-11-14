#ifndef _STATUSSCREEN_H_
#define _STATUSSCREEN_H_

#include "GUI.h"
#include "variants.h"

void menuStatus(void);
void set_status_icon(void);
void drawTemperature(void);
void toggleTool(void);
void statusScreen_setMsg(uint8_t *title,uint8_t *msg);
void drawStatusScreenMsg(void);
#endif

//LCD resolution, font and icon size
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)

#define statusicon_val_x0   5
#define statusicon_val_y0  63
//#define  statusicon_val_charcount  7
#define statusicon_name_x0 59
#define statusicon_name_y0  7
//#define  statusicon_name_charcount  3
#define STATUS_MSGBOX_ICON_XCENTER 16
#define STATUS_MSGBOX_ICON_YCENTER 16
#define STATUS_MSGBOX_TITLE_XOFFSET 30
#define STATUS_MSGBOX_BODY_XOFFSET  0
#define STATUS_MSGBOX_BODY_YOFFSET 59
#define STATUS_MSGBOX_BODY_BOTTOM 6

#elif defined(TFT28_V1_0) || defined(TFT24_V1_1)
//for status screen
#define statusicon_val_x0   2
#define statusicon_val_y0  47
//#define  statusicon_val_charcount  7
#define statusicon_name_x0 44
#define statusicon_name_y0  5
//#define  statusicon_name_charcount  3

#define STATUS_MSGBOX_ICON_XCENTER  11
#define STATUS_MSGBOX_ICON_YCENTER  11
#define STATUS_MSGBOX_TITLE_XOFFSET 24
#define STATUS_MSGBOX_BODY_XOFFSET  0
#define STATUS_MSGBOX_BODY_BOTTOM 45

#endif