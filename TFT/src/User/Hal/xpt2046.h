#ifndef _XPT2046_H_
#define _XPT2046_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "sw_spi.h"

void XPT2046_Init(void);
uint8_t XPT2046_Read_Pen(void);
uint16_t XPT2046_Repeated_Compare_AD(u8 CMD);

#ifdef __cplusplus
}
#endif

#endif
