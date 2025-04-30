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

void speedSetTargetPercent(const uint8_t tool, const uint16_t per);   // set target speed percentage
uint16_t speedGetTargetPercent(const uint8_t tool);                   // get target speed percentage
void speedSetCurrentPercent(const uint8_t tool, const uint16_t per);  // set current speed percentage
uint16_t speedGetCurrentPercent(const uint8_t tool);                  // get current speed percentage

void loopCheckSpeed(void);                 // called in loopBackEnd(). Loop for check on speed
void speedQueryClearSendingWaiting(void);  // called in sendQueueCmd(). Clear sending waiting for speed query
void speedQuery(void);                     // query for speed

#ifdef __cplusplus
}
#endif

#endif
