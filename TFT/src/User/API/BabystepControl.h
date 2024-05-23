#ifndef _BABYSTEP_CONTROL_H_
#define _BABYSTEP_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void babystepSetValue(float value);     // set current babystep value
float babystepGetValue(void);           // get current babystep value
float babystepResetValue(void);         // reset babystep value to default value
float babystepUpdateValue(float unit);  // update babystep value

#ifdef __cplusplus
}
#endif

#endif
