#ifndef _LASER_H_
#define _LASER_H_
#include "stdint.h"
#include "stdbool.h"

extern const char* laserID[];
extern const char* laserCmd[];


void menuLaser(void);

void    laserSetSpeed(uint8_t i, uint8_t speed);
void    laserSetSendWaiting(uint8_t i, bool isWaiting);
uint8_t laserGetCurIndex(uint8_t i);
void    laserSetCurIndex(uint8_t i);
uint8_t laserGetSpeed(uint8_t i);

#endif
