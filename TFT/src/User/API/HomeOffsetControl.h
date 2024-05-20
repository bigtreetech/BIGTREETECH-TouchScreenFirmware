#ifndef _HOME_OFFSET_CONTROL_H_
#define _HOME_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void homeOffsetEnable(float shim);        // enable home offset
void homeOffsetDisable(void);             // disable home offset
bool homeOffsetGetStatus(void);           // get home offset status
float homeOffsetSetValue(float value);    // set Z offset value
float homeOffsetGetValue(void);           // get current Z offset value
float homeOffsetResetValue(void);         // reset Z offset value to default value
float homeOffsetUpdateValue(float unit);  // update Z offset value

#ifdef __cplusplus
}
#endif

#endif
