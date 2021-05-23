#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void caseLightValueQuery(void);
void caseLightSetState(bool state);
void caseLightSetBrightness(uint8_t brightness);
void caseLightQuerySetWait(bool wait);
uint8_t caseLightGetBrightness(void);
uint8_t caseLightGetBrightnessPercent(void);
bool caseLightGetState(void);
void loopCaseLight(void);
void caseLightSendWaiting(bool isWaiting);
void caseLightChangeBrightness(int8_t brightness_delta);
void caseLightChangeBrightnessPrecent(int8_t brightness_delta_percent);

#ifdef __cplusplus
}
#endif
