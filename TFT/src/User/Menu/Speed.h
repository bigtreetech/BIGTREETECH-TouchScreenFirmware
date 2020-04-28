#ifndef _SPEED_H_
#define _SPEED_H_

#include "stdint.h"
#include "stdbool.h"

void speedSetSendWaiting(uint8_t tool, bool isWaiting);
void speedSetPercent(uint8_t tool, uint16_t per);
uint16_t speedGetPercent(uint8_t tool);
void menuSpeed(void);

#endif
