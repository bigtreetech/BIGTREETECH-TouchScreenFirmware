#ifndef _LCD_DMA_H_
#define _LCD_DMA_H_

#include "variants.h"  // for u16 etc...

void LCD_DMA_Config(void);
void lcd_frame_display(u16 sx, u16 sy, u16 w, u16 h, u32 addr);

#endif
