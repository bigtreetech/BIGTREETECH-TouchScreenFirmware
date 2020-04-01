#ifndef _COLORS_H_
#define _COLORS_H_

#include "stdint.h"

// Color Definition

#define WHITE                 0xFFFF
#define BLACK                 0x0000
#define RED                   0xF800
#define GREEN                 0x07E0
#define BLUE                  0x001F
#define CYAN                  0x7FFF
#define MAGENTA               0xF81F
#define YELLOW                0xFFE0
#define ORANGE                0xFC00
#define PURPLE                0x6012
#define LIME                  0xBFE0
#define BROWN                 0X9240
#define DARKBLUE              0X000F
#define DARKGREEN             0x2261
#define GRAY                  0X8430
#define DARKGRAY              0x39C7


typedef enum
{
LCD_WHITE = 0,
LCD_BLACK,
LCD_RED,
LCD_GREEN,
LCD_BLUE,
LCD_CYAN,
LCD_MAGENTA,
LCD_YELLOW,
LCD_ORANGE,
LCD_PURPLE,
LCD_LIME,
LCD_BROWN,
LCD_DARKBROWN,
LCD_GRAY,
LCD_DARKGRAY,
LCD_COLOR_COUNT
}LCD_COLOR;

extern uint16_t lcd_colors[LCD_COLOR_COUNT];

#endif
