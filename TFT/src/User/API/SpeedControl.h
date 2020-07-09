#ifndef _SPEEDCONTROL_H_
#define _SPEEDCONTROL_H_


#define SPEED_NUM 2
#define SPEED_MIN 10
#define SPEED_MAX 999

#include "stdint.h"
#include "stdbool.h"

void      speedSetSendWaiting(uint8_t tool, bool isWaiting);
void      speedSetPercent(uint8_t tool, uint16_t per);
uint16_t  speedGetPercent(uint8_t tool);
bool      SpeedChanged(uint8_t i);
void      loopSpeed(void);


#endif
