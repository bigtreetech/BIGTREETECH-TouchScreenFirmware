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
bool fanIsValid(const uint8_t index);                           // check whether the index is a valid fan index
void fanSetSpeed(const uint8_t i, const uint8_t speed);
uint8_t fanGetSetSpeed(const uint8_t i);
void fanSetPercent(const uint8_t i, const uint8_t percent);
uint8_t fanGetSetPercent(const uint8_t i);
void fanSetCurSpeed(const uint8_t i, const uint8_t speed);
uint8_t fanGetCurSpeed(const uint8_t i);
void fanSetCurPercent(const uint8_t i, const uint8_t percent);  // set current fan percent
uint8_t fanGetCurPercent(const uint8_t i);                      // get current fan percent

void loopCheckFan(void);                     // called in loopBackEnd(). Loop for check on fan
void ctrlFanQueryClearSendingWaiting(void);  // called in sendQueueCmd(). Clear sending waiting for controller fan query
void ctrlFanQuery(void);                     // query for controller fan only

#ifdef __cplusplus
}
#endif

#endif
