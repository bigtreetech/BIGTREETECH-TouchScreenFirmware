#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void menuCaseLight(void);
void caseLightSetPercent(uint8_t brightness);
void caseLightSetState(bool state);

#ifdef __cplusplus
}
#endif
