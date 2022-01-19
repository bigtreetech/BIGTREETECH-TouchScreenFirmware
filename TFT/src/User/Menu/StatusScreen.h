//TG MODIFIED BY T.GIOIOSA
#ifndef _STATUSSCREEN_H_
#define _STATUSSCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

//Colors for drawing the icons
#define GANTRYLBL_BKCOLOR   MAT_DARKSLATE
#define GANTRYLBL_COLOR     WHITE
#define HEADING_COLOR       WHITE
#define VAL_COLOR           BLACK
#define INFOBOX_BKCOLOR     MAT_SLATE
#define INFOBOX_BORDER      MAT_SLATE
#define INFOBOX_ICON_COLOR  MAT_BLUE
#define INFOMSG_BKCOLOR     WHITE
#define INFOMSG_COLOR       BLACK

extern const GUI_POINT ss_title_point;
extern const GUI_POINT ss_val_point;
extern const GUI_POINT ss_val2_point;
extern const GUI_RECT msgRect;

void menuStatus(void);
void drawAllLiveIconData(void);
void drawSingleLiveIconLine(uint8_t currentSpeedID);

//void storegantry(int n, float val);       //TG 1/12/20 seems to not be used in this version
void statusScreen_setMsg(const uint8_t *title,const uint8_t *msg);
void statusScreen_setReady(void);
void drawStatusScreenMsg(void);
//float getAxisLocation(uint8_t n);         //TG 1/12/20 seems to not be used in this version
//void gantry_dec(int n, float val);        //TG 1/12/20 seems to not be used in this version
//void gantry_inc(int n, float val);        //TG 1/12/20 seems to not be used in this version

#ifdef __cplusplus
}
#endif

#endif
