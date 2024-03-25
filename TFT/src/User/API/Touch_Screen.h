#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "GUI.h"

extern int32_t TS_CalPara[7];  // touch screen calibration parameters
extern bool TS_Sound;
extern void (* TS_ReDrawIcon)(uint8_t position, uint8_t isPressed);

void TS_GetCoordinates(uint16_t * x, uint16_t * y);
void TS_CheckPress(void);  // WARNING, TIMER INTERRUPT ROUTINE CALLED ONCE A MILLISECOND
bool TS_IsPressed(void);
uint16_t TS_KeyValue(uint8_t totalRect, const GUI_RECT * menuRect);
uint16_t KEY_GetValue(uint8_t totalRect, const GUI_RECT * menuRect);

void TS_Calibrate(void);

#ifdef __cplusplus
}
#endif

#endif
