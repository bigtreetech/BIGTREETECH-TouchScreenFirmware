#ifndef _XPT2046_H_
#define _XPT2046_H_

#include "sw_spi.h"
#include "stdbool.h"

void XPT2046_Init(void);
u8   XPT2046_Read_Pen(void);
u16  XPT2046_Repeated_Compare_AD(u8 CMD);

#endif
