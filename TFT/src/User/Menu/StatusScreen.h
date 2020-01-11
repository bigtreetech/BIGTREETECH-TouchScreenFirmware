#ifndef _STATUSSCREEN_H_
#define _STATUSSCREEN_H_
#include "GUI.h"


//Colors for drawing the icons
#define GANTRYLBL_BKCOLOR   0x2187
#define GANTRYLBL_COLOR     WHITE
#define HEADING_COLOR       WHITE
#define VAL_COLOR           BLACK
#define INFOBOX_BKCOLOR     0x4b0d
#define INFOBOX_BORDER      0x4b0d
#define INFOBOX_ICON_COLOR  0x03BF
#define INFOMSG_BKCOLOR     WHITE
#define INFOMSG_COLOR       BLACK

void menuStatus(void);
//void set_status_icon(void);
void drawTemperature(void);
void storegantry(int n, float val);
void toggleTool(void);
void statusScreen_setMsg(const uint8_t *title,const uint8_t *msg);
void drawStatusScreenMsg(void);
float getAxisLocation(uint8_t n);
void gantry_dec(int n, float val);
void gantry_inc(int n, float val);
//LCD resolution, font and icon size
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)

  #define SSICON_VAL_Y0           63
  //#define  statusicon_val_charcount  7

  #define SSICON_NAME_Y0          7
  //#define  statusicon_name_charcount  3

  #define STATUS_MSG_ICON_XOFFSET     5
  #define STATUS_MSG_ICON_YOFFSET     5
  #define STATUS_MSG_TITLE_XOFFSET    5
  #define STATUS_MSG_BODY_XOFFSET     0
  #define STATUS_MSG_BODY_YOFFSET     59
  #define STATUS_MSG_BODY_BOTTOM      6
  #define STATUS_TITLE_SHORT          20
  #define STATUS_GANTRY_YOFFSET       6


#elif defined(TFT28_V1_0) || defined(TFT24_V1_1) || defined(TFT28_V3_0)

  #define SSICON_VAL_Y0           47
  //#define  statusicon_val_charcount  7

  #define SSICON_NAME_Y0          5
  //#define  statusicon_name_charcount  3

  #define STATUS_MSG_ICON_XOFFSET     3
  #define STATUS_MSG_ICON_YOFFSET     3
  #define STATUS_MSG_TITLE_XOFFSET    3
  #define STATUS_MSG_BODY_XOFFSET     0
  #define STATUS_MSG_BODY_YOFFSET     45
  #define STATUS_MSG_BODY_BOTTOM      4
  #define STATUS_TITLE_SHORT          10
  #define SS_ICON_START_Y              30
  #define STATUS_GANTRY_YOFFSET       4
#endif

#endif
