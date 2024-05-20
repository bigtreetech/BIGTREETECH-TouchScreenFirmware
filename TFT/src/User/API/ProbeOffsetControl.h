#ifndef _PROBE_OFFSET_CONTROL_H_
#define _PROBE_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void probeOffsetEnable(float shim);        // enable probe offset
void probeOffsetDisable(void);             // disable probe offset
bool probeOffsetGetStatus(void);           // get probe offset status
float probeOffsetSetValue(float value);    // set Z offset value
float probeOffsetGetValue(void);           // get current Z offset value
float probeOffsetResetValue(void);         // reset Z offset value to default value
float probeOffsetUpdateValue(float unit);  // update Z offset value

#ifdef __cplusplus
}
#endif

#endif
