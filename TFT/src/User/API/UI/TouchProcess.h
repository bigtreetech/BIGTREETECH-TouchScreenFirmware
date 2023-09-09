#ifndef _TOUCH_PROCESS_H_
#define _TOUCH_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "GUI.h"

#ifdef MKS_TFT35_V1_0
  // touch screen X, Y channels are swapped for MKS_TFT35_V1_0
  // Defined in MKS TFT35 Touch Screen source code: user\others\mks_touch_screen.c CHX=0x90, CHY=0xD0
  #define CMD_RDX 0x90
  #define CMD_RDY 0xD0
#else
  #define CMD_RDX 0xD0
  #define CMD_RDY 0x90
#endif

enum
{
  KNOB_INC = 1,
  KNOB_DEC,
};

extern bool touchSound;

void TSC_Calibration(void);
bool isPress(void);
uint16_t KEY_GetValue(uint8_t total_rect, const GUI_RECT *menuRect);
uint16_t Key_value(uint8_t total_rect, const GUI_RECT *menuRect);

void checkTouchScreen(void);  // WARNING, TIMER INTERRUPT ROUTINE CALLED ONCE A MILLISECOND

extern void (*TSC_ReDrawIcon)(uint8_t position, uint8_t is_press);
extern void TS_Get_Coordinates(uint16_t *x, uint16_t *y);

#ifdef __cplusplus
}
#endif

#endif
