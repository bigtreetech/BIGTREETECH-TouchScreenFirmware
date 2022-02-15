#ifndef _SPEED_CONTROL_H_
#define _SPEED_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define SPEED_NUM 2
#define SPEED_MIN 10
#define SPEED_MAX 999

void speedSetPercent(uint8_t tool, uint16_t per);
uint16_t speedGetSetPercent(uint8_t tool);
void speedSetCurPercent(uint8_t tool, uint16_t per);
uint16_t speedGetCurPercent(uint8_t tool);
void loopSpeed(void);
void speedQuerySetWait(bool wait);
void speedQuery(void);

#ifdef __cplusplus
}
#endif

#endif
