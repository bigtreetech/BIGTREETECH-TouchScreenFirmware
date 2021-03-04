#ifndef _UI_DRAW_H_
#define _UI_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"
#include "GUI.h"

#define SPACE_X           ((LCD_WIDTH - ICON_WIDTH*4)/4)
#define START_X           (SPACE_X/2)
#define SPACE_X_PER_ICON  (LCD_WIDTH/4)

#define SPACE_Y           ((LCD_HEIGHT - ICON_START_Y - ICON_HEIGHT*2)/2)

#ifdef TFT70_V3_0
#define SSICON_FIRST_ROW 5
#define SSICON_SECOND ROW
#endif

//select marlin or bigtree
#define SPACE_SELEX       ((LCD_WIDTH-ICON_WIDTH*2)/4)
#define SPACE_SELEY       ((LCD_HEIGHT-ICON_HEIGHT)/2)
#define text_startx       (LCD_WIDTH/2)

void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
bool model_DirectDisplay(GUI_POINT pos, char *gcode);
bool model_DecodeToFlash(char *gcode);
void ICON_CustomReadDisplay(uint16_t sx,uint16_t sy, u32 address);
void SMALLICON_ReadDisplay(uint16_t sx,uint16_t sy, uint8_t icon);
void ICON_PressedDisplay(uint16_t sx,uint16_t sy, uint8_t icon);


#ifdef __cplusplus
}
#endif

#endif
