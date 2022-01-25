#ifndef _LCD_DMA_H_
#define _LCD_DMA_H_

#include "variants.h"  // for uint16_t etc...

void LCD_DMA_Config(void);
void lcd_frame_display(uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint32_t addr);

#endif
