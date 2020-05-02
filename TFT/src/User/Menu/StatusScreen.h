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

#endif
