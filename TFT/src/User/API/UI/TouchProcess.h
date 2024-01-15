#ifndef _TOUCH_PROCESS_H_
#define _TOUCH_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "GUI.h"

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
