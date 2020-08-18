#ifndef _FANCONTROL_H_
#define _FANCONTROL_H_

#include "stdint.h"
#include "stdbool.h"

extern const char* fanID[];
extern const char* fanCmd[];

void    fanSetSpeed(uint8_t i, uint8_t speed);
uint8_t fanGetSpeed(uint8_t i);
void    fanSetSpeedPercent(uint8_t i, uint8_t percent);
uint8_t fanGetSpeedPercent(uint8_t i);
void    fanSetSendWaiting(uint8_t i, bool isWaiting);

void    loopFan(void);



#endif
