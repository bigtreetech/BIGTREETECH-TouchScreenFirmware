#ifndef _TOUCH_PROCESS_H_
#define _TOUCH_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define CMD_RDX 0xD0
#define CMD_RDY 0x90

enum
{
  KNOB_INC=1,
  KNOB_DEC,
};

#define KEY_CLICK          0x0000  // The key is defined as uint16_t , 16 (uint16_t 16bit)-3 (3 bits flag) = 13 bit, so uint16_t supports a maximum of 2 ^ 13 = 8192 key values
#define KEY_DOUBLE_CLICK   0x2000  //The third bit is used to identify the double-click action
#define KEY_LONG_RELEASE   0x4000  // The second bit is used to identify the release action after a long press
#define KEY_LONG_CLICK     0x8000  // The first bit is used to identify the long press action


extern bool touchSound;
void TSC_Calibration(void);
uint8_t   isPress(void);
uint16_t  KEY_GetValue(uint8_t total_rect,const GUI_RECT* menuRect);
uint16_t  Key_value(uint8_t total_rect,const GUI_RECT* menuRect);
uint16_t  KNOB_GetRV(GUI_RECT *knob);

void loopTouchScreen(void);

extern void (*TSC_ReDrawIcon)(uint8_t position, uint8_t is_press);
extern void TS_Get_Coordinates(uint16_t *x, uint16_t *y);

#ifdef __cplusplus
}
#endif

#endif
