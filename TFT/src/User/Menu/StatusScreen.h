#ifndef _STATUSSCREEN_H_
#define _STATUSSCREEN_H_
#include "GUI.h"

void menuStatus(void);
//void set_status_icon(void);
void drawTemperature(void);
void storegantry(int n, float val);
void toggleTool(void);
void statusScreen_setMsg(const uint8_t *title,const uint8_t *msg);
void drawStatusScreenMsg(void);
//LCD resolution, font and icon size
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)

  #define SSICON_VAL_X0           5
  #define SSICON_VAL_Y0           63
  //#define  statusicon_val_charcount  7

  #define SSICON_NAME_X0          57
  #define SSICON_NAME_Y0          7
  //#define  statusicon_name_charcount  3

  #define STATUS_MSG_ICON_XCENTER     16
  #define STATUS_MSG_ICON_YCENTER     16
  #define STATUS_MSG_TITLE_XOFFSET    30
  #define STATUS_MSG_BODY_XOFFSET     0
  #define STATUS_MSG_BODY_YOFFSET     59
  #define STATUS_MSG_BODY_BOTTOM      6
  #define STATUS_TITLE_SHORT          20
  #define SS_TITLE_END_Y              50
  #define STATUS_GANTRY_YOFFSET       8

#elif defined(TFT28_V1_0) || defined(TFT24_V1_1)

  #define SSICON_VAL_X0           2
  #define SSICON_VAL_Y0           47
  //#define  statusicon_val_charcount  7

  #define SSICON_NAME_X0          44
  #define SSICON_NAME_Y0          5
  //#define  statusicon_name_charcount  3

  #define STATUS_MSG_ICON_XCENTER     11
  #define STATUS_MSG_ICON_YCENTER     11
  #define STATUS_MSG_TITLE_XOFFSET    24
  #define STATUS_MSG_BODY_XOFFSET     0
  #define STATUS_MSG_BODY_YOFFSET     45
  #define STATUS_MSG_BODY_BOTTOM      4
  #define STATUS_TITLE_SHORT          10
  #define SS_TITLE_END_Y              30
  #define STATUS_GANTRY_YOFFSET       6
  #endif

#endif
