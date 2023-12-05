#ifndef _FAN_CONTROL_H_
#define _FAN_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "Settings.h"

#define FAN_TYPE_F       0  // Default cooling fan
#define FAN_TYPE_CTRL_S  1  // Controller FAN on Stepper/Bed On  (Check - Marlin M710)
#define FAN_TYPE_CTRL_I  2  // Controller FAN on Idle            (Check - Marlin M710)
#define FAN_TYPE_UNKNOWN 8  // Unknown / Not defined!

extern const char* fanID[MAX_FAN_COUNT];
extern const char* fanCmd[MAX_FAN_COUNT];

void fanResetSpeed(void);
bool fanIsValid(const uint8_t index);
void fanSetSpeed(const uint8_t i, const uint8_t speed);
uint8_t fanGetSetSpeed(const uint8_t i);
void fanSetPercent(const uint8_t i, const uint8_t percent);
uint8_t fanGetSetPercent(const uint8_t i);
void fanSetCurSpeed(const uint8_t i, const uint8_t speed);
uint8_t fanGetCurSpeed(const uint8_t i);
void fanSetCurPercent(const uint8_t i, const uint8_t percent);
uint8_t fanGetCurPercent(const uint8_t i);
void loopFan(void);
void ctrlFanQuerySetWait(const bool wait);
void ctrlFanQuery(void);

#ifdef __cplusplus
}
#endif

#endif
