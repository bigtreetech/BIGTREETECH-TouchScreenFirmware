#ifndef _XPT2046_H_
#define _XPT2046_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "variants.h"  // for uint8_t etc...

void XPT2046_Init(void);
uint8_t XPT2046_Read_Pen(void);
uint16_t XPT2046_Repeated_Compare_AD(uint8_t CMD);

#ifdef __cplusplus
}
#endif

#endif
