#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void caseLightSetPercent(uint8_t brightness);
void caseLightSetState(bool state);
void menuCaseLight(void);

#ifdef __cplusplus
}
#endif
