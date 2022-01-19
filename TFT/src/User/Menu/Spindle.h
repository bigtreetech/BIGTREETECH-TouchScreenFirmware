//TG MODIFIED BY T.GIOIOSA
#ifndef _TOOL_H_
#define _TOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "Configuration.h"
#include "Settings.h"
#include "menu.h"
#include "SpindleControl.h"
MENUITEMS heatItems;
       
uint8_t spindleState;
int16_t actTarget;
int16_t actCurrent;

char* spindleID[MAX_SPINDLE_COUNT];      
char* spindleDisplayID[MAX_SPINDLE_COUNT]; 
char* spindleCmd[MAX_SPINDLE_COUNT]; 
MENUITEMS spindleItems; 

//Icons list for on/off change  //TG 1/16/20 new for spindle  //TODO CHANGE ICONS
const ITEM itemSpindleONOFF[2];

void showSpeed(u8 index);
void menuSpindle(void);        //TG 1/16/20 new replaces menuSpindle
void toolSetCurrentIndex(uint8_t index);
void drawSpindleStatusInIcon(void);
void updateSpeedStatusDisplay(u8 index, bool speed_only);
#ifdef __cplusplus
}
#endif

#endif
