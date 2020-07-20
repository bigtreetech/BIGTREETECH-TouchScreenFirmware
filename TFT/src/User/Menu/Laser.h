#ifndef _FAN_H_
#define _FAN_H_
#include "stdint.h"
#include "stdbool.h"

extern const char* fanID[];
extern const char* fanCmd[];


void menuFan(void);

void    fanSetSpeed(uint8_t i, uint8_t speed);
uint8_t fanGetSpeed(uint8_t i);
void    fanSetSpeedPercent(uint8_t i, int16_t percent);
uint8_t fanGetSpeedPercent(uint8_t i);
void    fanSetSendWaiting(uint8_t i, bool isWaiting);
uint8_t fanGetCurIndex(uint8_t i);
void    fanSetCurIndex(uint8_t i);

#endif
