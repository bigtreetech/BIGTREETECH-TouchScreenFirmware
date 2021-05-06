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

extern char*   fanID[MAX_FAN_COUNT];
extern char*   fanCmd[MAX_FAN_COUNT];
extern uint8_t fanType[MAX_FAN_COUNT];

bool fanIsType(uint8_t i, uint8_t type);
uint8_t fanGetTypID(uint8_t startIndex, uint8_t type);
void fanSetSpeed(uint8_t i, uint8_t speed);
uint8_t fanGetSetSpeed(uint8_t i);
void fanSetPercent(uint8_t i, uint8_t percent);
uint8_t fanGetSetPercent(uint8_t i);
void fanSetCurSpeed(uint8_t i, uint8_t speed);
uint8_t fanGetCurSpeed(uint8_t i);
void fanSetCurPercent(uint8_t i, uint8_t percent);
uint8_t fanGetCurPercent(uint8_t i);
void fanBuildList(void);
void fanControlInit(void);
void loopFan(void);
bool FanChanged(uint8_t i);
void fanSpeedQuery(void);
void fanQuerySetWait(bool wait);

#ifdef __cplusplus
}
#endif

#endif
