#ifndef _FANCONTROL_H_
#define _FANCONTROL_H_

#include "stdint.h"
#include "stdbool.h"

extern const char* fanID[];
extern const char* fanCmd[];


bool    fanIsType(uint8_t i, uint8_t type);
uint8_t fanGetTypID(uint8_t startIndex, uint8_t type);
void    fanSetSpeed(uint8_t i, uint8_t speed);
uint8_t fanGetSpeed(uint8_t i);
void    fanSetSpeedPercent(uint8_t i, uint8_t percent);
uint8_t fanGetSpeedPercent(uint8_t i);
void    fanSetSendWaiting(uint8_t i, bool isWaiting);

void    loopFan(void);

void    fanSpeedQuery(void);
void    fanSpeedQuerySetWait(bool wait);

#endif
