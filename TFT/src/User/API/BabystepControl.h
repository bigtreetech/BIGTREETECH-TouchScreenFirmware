#ifndef _BABYSTEP_CONTROL_H_
#define _BABYSTEP_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Reset only babystep value to default value */
float babystepReset(void);

/* Get current babystep value */
float babystepGetValue(void);

/* Reset babystep value to default value */
float babystepResetValue(void);

// Decrease babystep value
float babystepDecreaseValue(float unit);

// Increase babystep value
float babystepIncreaseValue(float unit);

// Update babystep value by encoder
float babystepUpdateValueByEncoder(float unit, int8_t direction);

#ifdef __cplusplus
}
#endif

#endif
