#ifndef _XPT2046_H_
#define _XPT2046_H_

#include "sw_spi.h"
#include "stdbool.h"

void XPT2046_Init(void);
u8   XPT2046_Read_Pen(void);
u16  XPT2046_Repeated_Compare_AD(u8 CMD);

bool LCD_ReadPen(uint8_t intervals);
uint8_t LCD_ReadTouch(void);
void Touch_Sw(uint8_t num);
bool LCD_BtnTouch(uint8_t intervals);

#endif
