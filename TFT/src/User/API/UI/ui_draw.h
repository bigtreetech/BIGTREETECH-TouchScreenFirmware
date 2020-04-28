#ifndef _UI_DRAW_H_
#define _UI_DRAW_H_

#include "stdint.h"
#include "variants.h"
#include "stdbool.h"
#include "GUI.h"

#define SPACE_X ((LCD_WIDTH - ICON_WIDTH*4)/4)
#define START_X (SPACE_X/2)
#define SPACE_X_PER_ICON (LCD_WIDTH/4)

#define SPACE_Y ((LCD_HEIGHT - ICON_START_Y - ICON_HEIGHT*2)/2)

/*about GCODE KEY*/
#define GKEY_WIDTH  (LCD_WIDTH/6)
#define GKEY_HEIGHT ((LCD_HEIGHT - ICON_START_Y)/4)

//select marlin or bigtree
#define SPACE_SELEX   ((LCD_WIDTH-ICON_WIDTH*2)/4)
#define SPACE_SELEY   ((LCD_HEIGHT-ICON_HEIGHT)/2)
#define text_startx    (LCD_WIDTH/2)

void LCD_DMA_Config(void);

void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
bool bmp_DirectDisplay(GUI_POINT pos, char *bmp) ;
void ICON_CustomReadDisplay(u16 sx,u16 sy,u16 w, u16 h, u32 addr);
void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon);

#endif
