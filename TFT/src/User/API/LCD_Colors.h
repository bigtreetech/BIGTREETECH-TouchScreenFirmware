
#ifndef _LCD_COLORS_H_
#define _LCD_COLORS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Configuration.h"  // for KEYBOARD_MATERIAL_THEME etc...
#include "menu.h"

// Color Definition
#define WHITE         0xFFFF
#define BLACK         0x0000
#define RED           0xF800
#define GREEN         0x0760
#define BLUE          0x00DF
#define CYAN          0x07FF
#define MAGENTA       0xF81F
#define YELLOW        0xFFE0
#define ORANGE        0xFB23
#define PURPLE        0x7817
#define LIME          0xBFE0
#define BROWN         0X9240
#define DARKBLUE      0X0030
#define DARKGREEN     0x0340
#define GRAY          0X8430
#define DARKGRAY      0x2124

#define MAT_RED       0xE124
#define MAT_YELLOW    0xED80
#define MAT_GREEN     0x254B
#define MAT_BLUE      0x24BD
#define MAT_ORANGE    0xF3A0
#define MAT_DARKGRAY  0x52AA
#define MAT_LOWWHITE  0xCE79
#define MAT_PURPLE    0x9135
#define MAT_SLATE     0x4B0D
#define MAT_DARKSLATE 0x2187
#define MAT_ASBESTOS  0x7C51
#define MAT_SILVER    0xB618
#define MAT_CONCRETE  0x9514

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
} LCD_COLOR;

extern const uint16_t lcd_colors[LCD_COLOR_COUNT];
extern const LABEL lcd_color_names[LCD_COLOR_COUNT];

// colors scheme for Keyboards UI
#ifdef KEYBOARD_MATERIAL_THEME

  #if KEYBOARD_COLOR_LAYOUT == 0
    #define KEY_FONT_COLOR     WHITE
    #define KEY_BG_COLOR       MAT_DARKSLATE
    #define KB_BG_COLOR        DARKGRAY

    #define CTRL_FONT_COLOR    WHITE

    #define BAR_FONT_COLOR     KEY_FONT_COLOR
    #define BAR_BG_COLOR       KB_BG_COLOR
    #define BAR_BORDER_COLOR   KEY_BG_COLOR

    #define TEXTBOX_FONT_COLOR BLACK
    #define TEXTBOX_BG_COLOR   MAT_LOWWHITE

  #elif KEYBOARD_COLOR_LAYOUT == 1

    #define KEY_FONT_COLOR     DARKGRAY
    #define KEY_BG_COLOR       MAT_SILVER
    #define KB_BG_COLOR        MAT_ASBESTOS

    #define CTRL_FONT_COLOR    WHITE

    #define BAR_FONT_COLOR     KEY_FONT_COLOR
    #define BAR_BG_COLOR       KB_BG_COLOR
    #define BAR_BORDER_COLOR   KEY_BG_COLOR

    #define TEXTBOX_FONT_COLOR BLACK
    #define TEXTBOX_BG_COLOR   KEY_BG_COLOR

  #else

    #define KEY_BG_COLOR       WHITE
    #define KEY_FONT_COLOR     BLACK
    #define KB_BG_COLOR        BLACK

    #define CTRL_FONT_COLOR    WHITE

    #define BAR_FONT_COLOR     WHITE
    #define BAR_BG_COLOR       KB_BG_COLOR
    #define BAR_BORDER_COLOR   KEY_BG_COLOR

    #define TEXTBOX_FONT_COLOR BLACK
    #define TEXTBOX_BG_COLOR   KEY_BG_COLOR

  #endif

  #define BTN_ROUND_CORNER  ((LCD_WIDTH / 100) + 0.5)

#endif

// Material Dark
#define COLORSCHEME1_TERM_GCODE MAT_ORANGE
#define COLORSCHEME1_TERM_ACK   MAT_LOWWHITE
#define COLORSCHEME1_TERM_BACK  MAT_DARKSLATE

// Material Light
#define COLORSCHEME2_TERM_GCODE MAT_ORANGE
#define COLORSCHEME2_TERM_ACK   MAT_DARKSLATE
#define COLORSCHEME2_TERM_BACK  MAT_LOWWHITE

// High Contrast
#define COLORSCHEME3_TERM_GCODE ORANGE
#define COLORSCHEME3_TERM_ACK   WHITE
#define COLORSCHEME3_TERM_BACK  BLACK

#ifdef __cplusplus
}
#endif

#endif
