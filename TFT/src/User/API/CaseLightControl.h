#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

bool caseLightGetState(void);
void caseLightSetState(bool state);

uint8_t caseLightGetBrightness(void);
void caseLightSetBrightness(uint8_t brightness);

uint8_t caseLightGetBrightnessPercent(void);
void caseLightSetBrightnessPercent(int8_t unit);

#ifdef __cplusplus
}
#endif
