#ifndef _FAN_CONTROL_H_
#define _FAN_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "Settings.h"

#define FAN_TYPE_F       0  // default cooling fan
#define FAN_TYPE_CTRL_S  1  // controller FAN on stepper/bed on  (check - Marlin M710)
#define FAN_TYPE_CTRL_I  2  // controller FAN on idle            (check - Marlin M710)
#define FAN_TYPE_UNKNOWN 8  // unknown / not defined!

extern const char * fanID[MAX_FAN_COUNT];
extern const char * fanCmd[MAX_FAN_COUNT];

void fanResetSpeed(void);
bool fanIsValid(const uint8_t index);                               // check whether the index is a valid fan index
void fanSetTargetSpeed(const uint8_t i, const uint8_t speed);       // set target fan speed
uint8_t fanGetTargetSpeed(const uint8_t i);                         // get target fan speed
void fanSetTargetPercent(const uint8_t i, const uint8_t percent);   // set target fan percentage
uint8_t fanGetTargetPercent(const uint8_t i);                       // get target fan percentage
void fanSetCurrentSpeed(const uint8_t i, const uint8_t speed);      // set current fan speed
uint8_t fanGetCurrentSpeed(const uint8_t i);                        // get current fan speed
void fanSetCurrentPercent(const uint8_t i, const uint8_t percent);  // set current fan percentage
uint8_t fanGetCurrentPercent(const uint8_t i);                      // get current fan percentage

void loopCheckFan(void);                     // called in loopBackEnd(). Loop for check on fan
void ctrlFanQueryClearSendingWaiting(void);  // called in sendQueueCmd(). Clear sending waiting for controller fan query
void ctrlFanQuery(void);                     // query for controller fan only

#ifdef __cplusplus
}
#endif

#endif
