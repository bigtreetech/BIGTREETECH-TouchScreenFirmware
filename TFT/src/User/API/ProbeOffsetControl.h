#ifndef _PROBE_OFFSET_CONTROL_H_
#define _PROBE_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Enable probe offset
void probeOffsetEnable(bool skipZOffset, float shim);

// Disable probe offset
void probeOffsetDisable(void);

// Get probe offset status
bool probeOffsetGetStatus(void);

// Set Z offset value
float probeOffsetSetValue(float value);

// Get current Z offset value
float probeOffsetGetValue(void);

// Reset Z offset value to default value
float probeOffsetResetValue(void);

// Update Z offset value
float probeOffsetUpdateValue(float unit, int8_t direction);

#ifdef __cplusplus
}
#endif

#endif
