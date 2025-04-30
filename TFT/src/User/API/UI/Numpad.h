#ifndef _NUMPAD_H_
#define _NUMPAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

double numPadFloat(const char * title, double param_val, double reset_val, bool negative);   // numpad for decimal numbers
int32_t numPadInt(const char * title, int32_t param_val, int32_t reset_val, bool negative);  // numpad for integer numbers

#ifdef __cplusplus
}
#endif

#endif
