
#ifndef _LCD_COLORS_H_
#define _LCD_COLORS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Configuration.h"  // for KEYBOARD_MATERIAL_THEME, LIVE_TEXT_COMMON_COLOR etc.
#include "menu.h"

// color definition
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
#define MAT_GREEN     0x254B
#define MAT_BLUE      0x24BD
#define MAT_CYAN      0x157C
#define MAT_MAGENTA   0x9135
#define MAT_YELLOW    0xED80
#define MAT_ORANGE    0xF3A0
#define MAT_PURPLE    0x61D6
#define MAT_LIME      0xC6C6
#define MAT_DARKGRAY  0x52AA
#define MAT_LOWWHITE  0xCE79
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

// colors scheme in menu / Popup / Notification menu
#define MENU_BUSY_DOT_COLOR         YELLOW

#define POPUP_BORDER_COLOR          GRAY
#define POPUP_TITLE_FONT_COLOR      DARKGRAY
#define POPUP_TITLE_BG_COLOR        MAT_LOWWHITE
#define POPUP_MSG_FONT_COLOR        DARKGRAY
#define POPUP_MSG_BG_COLOR          MAT_LOWWHITE
#define POPUP_ACTION_FONT_COLOR     DARKGRAY
#define POPUP_ACTION_BG_COLOR       MAT_LOWWHITE

#define NOTIF_ICON_FG_COLOR         WHITE
#define NOTIF_ICON_ERROR_BG_COLOR   MAT_RED
#define NOTIF_ICON_SUCCESS_BG_COLOR MAT_GREEN
#define NOTIF_ICON_INFO_BG_COLOR    MAT_BLUE
#define NOTIF_TEXT_FONT_COLOR       DARKGRAY
#define NOTIF_TEXT_BG_COLOR         MAT_LOWWHITE

// colors scheme for gantry and infomsg in Status Screen / Printing menu
#define GANTRY_XYZ_FONT_COLOR WHITE
#define GANTRY_XYZ_BG_COLOR   infoSettings.status_xyz_bg_color
#define INFOMSG_FONT_COLOR    BLACK
#define INFOMSG_BG_COLOR      WHITE

// colors scheme for live text (name and value) in Status Screen menu
#define SS_NAME_COLOR    WHITE

#if !defined(TFT70_V3_0) && !defined(LIVE_TEXT_COMMON_COLOR)
  #define SS_VAL_COLOR   BLACK
#else
  #define SS_VAL_COLOR   SS_NAME_COLOR
#endif

#ifdef TFT70_V3_0
  #define SS_VAL_COLOR_2 ORANGE
#endif

// colors scheme for live text in List Item menu
#define LI_KEY_PRESSED_COLOR WHITE
#define LI_VAL_COLOR         MAT_LOWWHITE

// colors scheme for live text in Preheat menu
#define PH_VAL_COLOR WHITE

// colors scheme for live text in Level Corner menu
#define LC_VAL_COLOR   ORANGE
#define LC_VAL_COLOR_2 WHITE

// colors scheme for keyboard UI in Mesh Editor menu
#define MESH_FONT_COLOR     infoSettings.font_color
#define MESH_BG_COLOR       infoSettings.bg_color
#define MESH_BORDER_COLOR   infoSettings.list_border_color
#define MESH_BORDER_COLOR_2 0x4b0d

// colors scheme for keyboard UI in LED Color menu
#define LC_KB_FONT_COLOR     infoSettings.font_color
#define LC_KB_BG_COLOR       infoSettings.bg_color
#define LC_KB_BORDER_COLOR   infoSettings.list_border_color
#define LC_KB_BORDER_COLOR_2 0x4b0d

#define LC_CTRL_FONT_COLOR   WHITE
#define LC_CTRL_BG_COLOR     0x2174
#define LC_CTRL_BORDER_COLOR 0x4b0d

// colors scheme for keyboard UI in Terminal / Numpad menu
#ifndef KEYBOARD_MATERIAL_THEME
  #if KEYBOARD_COLOR_LAYOUT == 0
    #define KEY_FONT_COLOR     infoSettings.font_color
    #define KEY_BG_COLOR       infoSettings.bg_color
    #define KEY_BORDER_COLOR   infoSettings.list_border_color
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #elif KEYBOARD_COLOR_LAYOUT == 1
    #define KEY_FONT_COLOR     infoSettings.font_color
    #define KEY_BG_COLOR       infoSettings.list_border_color
    #define KEY_BORDER_COLOR   infoSettings.bg_color
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #else
    #define KEY_FONT_COLOR     BLACK
    #define KEY_BG_COLOR       WHITE
    #define KEY_BORDER_COLOR   0x2174
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #endif

  #define KB_BG_COLOR          BLACK
  #define BAR_FONT_COLOR       WHITE
  #define BAR_BG_COLOR         0x2174
  #define BAR_BORDER_COLOR     0x4b0d
  #define TEXTBOX_FONT_COLOR   BLACK
  #define TEXTBOX_BG_COLOR     0xFFF2
#else
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

  #define CTRL_SEND_BG_COLOR   MAT_GREEN
  #define CTRL_BACK_BG_COLOR   MAT_RED
  #define CTRL_ABC_BG_COLOR    MAT_SLATE
  #define CTRL_DEL_BG_COLOR    MAT_ORANGE

  #define BTN_ROUND_CORNER     ((LCD_WIDTH / 100) + 0.5)
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

// colors scheme for progress bar in Printing menu
#if PROGRESS_BAR_COLOR == 0  // ORANGE
  #define PB_BORDER           ORANGE
  #define PB_FILL             MAT_ORANGE
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 1  // YELLOW
  #define PB_BORDER           YELLOW
  #define PB_FILL             MAT_YELLOW
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 2  // RED
  #define PB_BORDER           RED
  #define PB_FILL             MAT_RED
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 3  // GREEN
  #define PB_BORDER           GREEN
  #define PB_FILL             MAT_GREEN
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 4  // BLUE
  #define PB_BORDER           BLUE
  #define PB_FILL             MAT_BLUE
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 5  // CYAN
  #define PB_BORDER           CYAN
  #define PB_FILL             MAT_CYAN
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 6  // MAGENTA
  #define PB_BORDER           MAGENTA
  #define PB_FILL             MAT_MAGENTA
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 7  // PURPLE
  #define PB_BORDER           PURPLE
  #define PB_FILL             MAT_PURPLE
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 8  // LIME
  #define PB_BORDER           LIME
  #define PB_FILL             MAT_LIME
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#elif PROGRESS_BAR_COLOR == 9  // GRAY
  #define PB_BORDER           MAT_LOWWHITE
  #define PB_FILL             GRAY
  #define PB_BCKG             DARKGRAY
  #define PB_STRIPE_ELAPSED   BLACK
  #define PB_STRIPE_REMAINING PB_FILL
#endif

#ifdef __cplusplus
}
#endif

#endif
