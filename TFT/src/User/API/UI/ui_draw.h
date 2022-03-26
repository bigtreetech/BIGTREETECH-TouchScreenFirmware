#ifndef _UI_DRAW_H_
#define _UI_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"
#include "GUI.h"

#ifdef PORTRAIT_MODE
  #define SPACE_X          ((LCD_WIDTH - ICON_WIDTH * 3) / 3)
  #define SPACE_X_PER_ICON (LCD_WIDTH / 3)
#else
  #define SPACE_X          ((LCD_WIDTH - ICON_WIDTH * 4) / 4)
  #define SPACE_X_PER_ICON (LCD_WIDTH / 4)
#endif

#define START_X            (SPACE_X / 2)

#ifdef PORTRAIT_MODE
  #define SPACE_Y          ((LCD_HEIGHT - ICON_START_Y - ICON_HEIGHT * 3) / 3)
#else
  #define SPACE_Y          ((LCD_HEIGHT - ICON_START_Y - ICON_HEIGHT * 2) / 2)
#endif

// select marlin or bigtree
#define SPACE_SELEX      ((LCD_WIDTH - ICON_WIDTH * 2) / 4)
#define SPACE_SELEY      ((LCD_HEIGHT - ICON_HEIGHT) / 2)
#define text_startx      (LCD_WIDTH / 2)

// thumbnail parser options
#define PARSER_CLASSIC   0
#define PARSER_RGB565    1
#define PARSER_BASE64PNG 2

#define COLOR_BYTE_SIZE sizeof(uint16_t)  // RGB565 color byte is equal to uint16_t

typedef struct
{
  uint16_t index;
  uint32_t address;
  uint16_t width;
  uint16_t height;
} BMP_INFO;

// uint32_t _getBMPsizeAddr(uint8_t *w, uint8_t *h, uint32_t address);
// uint32_t _getBMPsizeIndex(uint8_t *w, uint8_t *h, uint16_t index);

//#define getBMPsize(x, y, c) _Generic(((c+0)), uint32_t: _getBMPsizeAddr, uint32_t*: _getBMPsizeAddr, default: _getBMPsizeIndex)(x, y, c)

void LOGO_ReadDisplay(void);
void ICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
void ICON_ReadBuffer(uint16_t *buf, uint16_t x, uint16_t y, int16_t w, int16_t h, uint16_t icon);
uint16_t ICON_ReadPixel(uint32_t address, uint16_t w, uint16_t h, int16_t x, int16_t y);
bool model_DirectDisplay(GUI_POINT pos, char *gcode);
bool model_DecodeToFlash(char *gcode);
void IMAGE_ReadDisplay(uint16_t sx, uint16_t sy, uint32_t address);
void SMALLICON_ReadDisplay(uint16_t sx, uint16_t sy, uint8_t icon);
void ICON_PressedDisplay(uint16_t sx, uint16_t sy, uint8_t icon);

#ifdef __cplusplus
}
#endif

#endif
