#ifndef _UI_DRAW_H_
#define _UI_DRAW_H_

#include "stdint.h"
#include "variants.h"

#define SPACE_X ((LCD_WIDTH - ICON_WIDTH*4)/4)
#define START_X (SPACE_X/2)
#define SPACE_X_PER_ICON (LCD_WIDTH/4)

#define SPACE_Y ((LCD_HEIGHT - TITLE_END_Y - ICON_HEIGHT*2)/2)

void LCD_DMA_Config(void);

void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon);

#endif

