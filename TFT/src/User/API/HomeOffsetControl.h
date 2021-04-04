#ifndef _HOME_OFFSET_CONTROL_H_
#define _HOME_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Enable home offset
void homeOffsetEnable(bool skipZOffset, float shim);

// Disable home offset
void homeOffsetDisable(void);

// Get home offset status
bool homeOffsetGetStatus(void);

// Set Z offset value
float homeOffsetSetValue(float value);

// Get current Z offset value
float homeOffsetGetValue(void);

// Reset Z offset value to default value
float homeOffsetResetValue(void);

// Decrease Z offset value
float homeOffsetDecreaseValue(float unit);

// Increase Z offset value
float homeOffsetIncreaseValue(float unit);

// Update Z offset value by encoder
float homeOffsetUpdateValueByEncoder(float unit, int8_t direction);

#ifdef __cplusplus
}
#endif

#endif
