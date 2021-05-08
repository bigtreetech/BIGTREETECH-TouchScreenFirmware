#ifndef _NUMPAD_H_
#define _NUMPAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

double numPadFloat(uint8_t* title, double old_val, double reset_val, bool negative);

int32_t numPadInt(uint8_t* title, int32_t old_val, int32_t reset_val, bool negative);

#ifdef __cplusplus
}
#endif

#endif
