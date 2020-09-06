
#ifndef _COLORS_H_
#define _COLORS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "menu.h"

// Color Definition

#define WHITE                 0xFFFF
#define BLACK                 0x0000
#define RED                   0xF800
#define GREEN                 0x0760
#define BLUE                  0x00DF
#define CYAN                  0x07FF
#define MAGENTA               0xF81F
#define YELLOW                0xFFE0
#define ORANGE                0xFB23
#define PURPLE                0x7817
#define LIME                  0xBFE0
#define BROWN                 0X9240
#define DARKBLUE              0X0030
#define DARKGREEN             0x0340
#define GRAY                  0X8430
#define DARKGRAY              0x2124


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
LCD_DARKBLUE,
LCD_DARKGREEN,
LCD_GRAY,
LCD_DARKGRAY,
LCD_COLOR_COUNT
}LCD_COLOR;

extern const u16 lcd_colors[LCD_COLOR_COUNT];
extern const LABEL lcd_color_names[LCD_COLOR_COUNT];

#ifdef __cplusplus
}
#endif

#endif
