#ifndef _FANCONTROL_H_
#define _FANCONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "includes.h"

#define FAN_TYPE_F        0  // Default cooling fan
#define FAN_TYPE_CTRL_S   1  // Controller FAN on Stepper/Bed On  (Check - Marlin M710)
#define FAN_TYPE_CTRL_I   2  // Controller FAN on Idle            (Check - Marlin M710)
#define FAN_TYPE_UNKNOWN  8  // Unknown / Not defined!

extern char*   fanID[MAX_FAN_COUNT];
extern char*   fanCmd[MAX_FAN_COUNT];
extern uint8_t fanType[MAX_FAN_COUNT];

bool    fanIsType(uint8_t i, uint8_t type);
uint8_t fanGetTypID(uint8_t startIndex, uint8_t type);
void    fanSetSpeed(uint8_t i, uint8_t speed);
uint8_t fanGetSpeed(uint8_t i);
void    fanSetSpeedPercent(uint8_t i, uint8_t percent);
uint8_t fanGetSpeedPercent(uint8_t i);
void    fanSetSendWaiting(uint8_t i, bool isWaiting);
void    fanBuildList(void);
void    fanControlInit(void);
void    loopFan(void);

void    fanSpeedQuery(void);
void    fanSpeedQuerySetWait(bool wait);

#ifdef __cplusplus
}
#endif

#endif
