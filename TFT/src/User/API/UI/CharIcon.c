#include "CharIcon.h"
#include "LCD_Colors.h"
#include <stdint.h>

const uint16_t charIconColor[CHARICON_NUM] = {
  BLACK,         // CHARICON_BLANK = 0
  WHITE,         // CHARICON_NOZZLE
  WHITE,         // CHARICON_BED
  WHITE,         // CHARICON_FAN
  MAT_YELLOW,    // CHARICON_FOLDER
  MAT_BLUE,      // CHARICON_FILE
  WHITE,         // CHARICON_PAGEUP
  WHITE,         // CHARICON_PAGEDOWN
  WHITE,         // CHARICON_BACK
  WHITE,         // CHARICON_LEFT_TOP
  WHITE,         // CHARICON_LEFT_BOTTOM
  WHITE,         // CHARICON_RIGHT_TOP
  WHITE,         // CHARICON_RIGHT_BOTTOM
  MAT_YELLOW,    // CHARICON_ALERT
  MAT_RED,       // CHARICON_WARNING
  MAT_RED,       // CHARICON_ERROR
  MAT_YELLOW,    // CHARICON_CAUTION
  MAT_BLUE,      // CHARICON_INFO
  MAT_YELLOW,    // CHARICON_HAND
  MAT_YELLOW,    // CHARICON_WAIT
  MAT_BLUE,      // CHARICON_QUESTION
  WHITE,         // CHARICON_PLAY
  WHITE,         // CHARICON_PAUSE
  WHITE,         // CHARICON_STOP
  WHITE,         // CHARICON_EJECT
  WHITE,         // CHARICON_PLAY_ROUND
  WHITE,         // CHARICON_PAUSE_ROUND
  WHITE,         // CHARICON_OK_ROUND
  WHITE,         // CHARICON_CANCEL_ROUND
  WHITE,         // CHARICON_MINUS_ROUND
  WHITE,         // CHARICON_PLUS_ROUND
  WHITE,         // CHARICON_MINUS
  WHITE,         // CHARICON_PLUS
  MAT_GREEN,     // CHARICON_OK
  MAT_RED,       // CHARICON_CANCEL
  MAT_RED,       // CHARICON_HALT
  WHITE,         // CHARICON_UP_DOWN
  WHITE,         // CHARICON_LEFT_RIGHT
  WHITE,         // CHARICON_POINT_LEFT
  WHITE,         // CHARICON_POINT_RIGHT
  WHITE,         // CHARICON_RETURN
  WHITE,         // CHARICON_REDO
  WHITE,         // CHARICON_UNDO
  WHITE,         // CHARICON_DOWNLOAD
  WHITE,         // CHARICON_UPLOAD
  WHITE,         // CHARICON_BULLET
  WHITE,         // CHARICON_BACK_SMALL
  WHITE,         // CHARICON_EXPAND
  WHITE,         // CHARICON_MOVE
  WHITE,         // CHARICON_ROTATE
  MAT_BLUE,      // CHARICON_RESET
  WHITE,         // CHARICON_EDIT
  WHITE,         // CHARICON_SAVE
  MAT_ORANGE,    // CHARICON_DELETE
  MAT_LOWWHITE,  // CHARICON_RADIO_CHECKED
  MAT_LOWWHITE,  // CHARICON_RADIO_UNCHECKED
  MAT_GREEN,     // CHARICON_CHECKED
  MAT_LOWWHITE,  // CHARICON_UNCHECKED
  MAT_RED,       // CHARICON_SOUND_OFF
  MAT_GREEN,     // CHARICON_SOUND_ON
  MAT_RED,       // CHARICON_ALERT_OFF
  MAT_GREEN,     // CHARICON_ALERT_ON
  MAT_RED,       // CHARICON_POWER_OFF
  MAT_GREEN,     // CHARICON_POWER_ON
  MAT_RED,       // CHARICON_WIFI_OFF
  MAT_GREEN,     // CHARICON_WIFI_ON
  WHITE,         // CHARICON_KEYBOARD
  MAT_RED,       // CHARICON_SD_ERROR
  MAT_GREEN,     // CHARICON_SD_OK
  MAT_BLUE,      // CHARICON_SD_REFRESH
  MAT_RED,       // CHARICON_USB_ERROR
  MAT_GREEN,     // CHARICON_USB_OK
  MAT_RED,       // CHARICON_PRINTER_ERROR
  MAT_GREEN,     // CHARICON_PRINTER_OK
  WHITE,         // CHARICON_BOARD
  WHITE,         // CHARICON_EEPROM
  WHITE,         // CHARICON_PRINT
  WHITE,         // CHARICON_SETTING1
  WHITE,         // CHARICON_PLUGIN
  WHITE,         // CHARICON_FEATURE
  WHITE,         // CHARICON_SETTING2
  WHITE,         // CHARICON_DETAIL
  WHITE,         // CHARICON_DETAIL2
  WHITE,         // CHARICON_ADJUST
  WHITE,         // CHARICON_MENU
  WHITE,         // CHARICON_POWER
  WHITE,         // CHARICON_TOUCH
  WHITE,         // CHARICON_LANGUAGE
  WHITE,         // CHARICON_CODE
  WHITE,         // CHARICON_POWER_PLUG
  WHITE,         // CHARICON_ROTATE_DEVICE
  WHITE,         // CHARICON_WINDOW
  WHITE,         // CHARICON_BACKGROUND_COLOR
  WHITE,         // CHARICON_FONT_COLOR
  WHITE,         // CHARICON_PAINT
  MAT_GREEN,     // CHARICON_TOGGLE_SMALL_ON
  MAT_RED,       // CHARICON_TOGGLE_SMALL_OFF
  MAT_DARKGRAY,  // CHARICON_TOGGLE_BODY
  WHITE,         // CHARICON_TOGGLE_SWITCH
  MAT_GREEN,     // CHARICON_TOGGLE_ON
  MAT_RED,       // CHARICON_TOGGLE_OFF
};

#define CHAR_BLANK            '\0'
#define CHAR_NOZZLE           "\u085F"
#define CHAR_BED              "\u0860"
#define CHAR_FAN              "\u0861"
#define CHAR_FOLDER           "\u0862"
#define CHAR_FILE             "\u0863"

#define CHAR_PAGEUP           "\u0867\u0868"
#define CHAR_PAGEDOWN         "\u0869\u086A"
#define CHAR_BACK             "\u086B\u086C"
#define CHAR_LEFT_PART1       "\u086D"
#define CHAR_LEFT_PART2       "\u086E"
#define CHAR_RIGHT_PART1      "\u086F"
#define CHAR_RIGHT_PART2      "\u0870"

#define CHAR_ALERT            "\u0880"
#define CHAR_WARNING          "\u0881"
#define CHAR_ERROR            "\u0882"
#define CHAR_CAUTION          "\u0883"
#define CHAR_INFO             "\u0884"
#define CHAR_HAND             "\u0885"
#define CHAR_WAIT             "\u0886"
#define CHAR_QUESTION         "\u0887"
#define CHAR_PLAY             "\u0888"
#define CHAR_PAUSE            "\u0889"
#define CHAR_STOP             "\u088A"
#define CHAR_EJECT            "\u088B"
#define CHAR_PLAY_ROUND       "\u088C"
#define CHAR_PAUSE_ROUND      "\u088D"
#define CHAR_OK_ROUND         "\u088E"
#define CHAR_CANCEL_ROUND     "\u088F"
#define CHAR_MINUS_ROUND      "\u0890"
#define CHAR_PLUS_ROUND       "\u0891"
#define CHAR_MINUS            "\u0892"
#define CHAR_PLUS             "\u0893"
#define CHAR_OK               "\u0894"
#define CHAR_CANCEL           "\u0895"
#define CHAR_HALT             "\u0896"
#define CHAR_UP_DOWN          "\u0897"
#define CHAR_LEFT_RIGHT       "\u0898"
#define CHAR_POINT_LEFT       "\u0899"
#define CHAR_POINT_RIGHT      "\u089A"
#define CHAR_RETURN           "\u089B"
#define CHAR_REDO             "\u089C"
#define CHAR_UNDO             "\u089D"
#define CHAR_DOWNLOAD         "\u089E"
#define CHAR_UPLOAD           "\u089F"
#define CHAR_BULLET           "\u08A0"
#define CHAR_BACK_SMALL       "\u08A1"
#define CHAR_EXPAND           "\u08A2"
#define CHAR_MOVE             "\u08A3"
#define CHAR_ROTATE           "\u08A4"
#define CHAR_RESET            "\u08A5"
#define CHAR_EDIT             "\u08A6"
#define CHAR_SAVE             "\u08A7"
#define CHAR_DELETE           "\u08A8"
#define CHAR_RADIO_CHECKED    "\u08A9"
#define CHAR_RADIO_UNCHECKED  "\u08AA"
#define CHAR_CHECKED          "\u08AB"
#define CHAR_UNCHECKED        "\u08AC"
#define CHAR_SOUND_OFF        "\u08AD"
#define CHAR_SOUND_ON         "\u08AE"
#define CHAR_ALERT_OFF        "\u08AF"
#define CHAR_ALERT_ON         "\u08B0"
#define CHAR_POWER_OFF        "\u08B1"
#define CHAR_POWER_ON         "\u08B2"
#define CHAR_WIFI_OFF         "\u08B3"
#define CHAR_WIFI_ON          "\u08B4"
#define CHAR_KEYBOARD         "\u08B5"
#define CHAR_SD_ERROR         "\u08B6"
#define CHAR_SD_OK            "\u08B7"
#define CHAR_SD_REFRESH       "\u08B8"
#define CHAR_USB_ERROR        "\u08B9"
#define CHAR_USB_OK           "\u08BA"
#define CHAR_PRINTER_ERROR    "\u08BB"
#define CHAR_PRINTER_OK       "\u08BC"
#define CHAR_BOARD            "\u08BD"
#define CHAR_EEPROM           "\u08BE"
#define CHAR_PRINT            "\u08BF"
#define CHAR_SETTING1         "\u08C0"
#define CHAR_PLUGIN           "\u08C1"
#define CHAR_FEATURE          "\u08C2"
#define CHAR_SETTING2         "\u08C3"
#define CHAR_DETAIL           "\u08C4"
#define CHAR_DETAIL2          "\u08C5"
#define CHAR_ADJUST           "\u08C6"
#define CHAR_MENU             "\u08C7"
#define CHAR_POWER            "\u08C8"
#define CHAR_TOUCH            "\u08C9"
#define CHAR_LANGUAGE         "\u08CA"
#define CHAR_CODE             "\u08CB"
#define CHAR_POWER_PLUG       "\u08CC"
#define CHAR_ROTATE_DEVICE    "\u08CD"
#define CHAR_WINDOW           "\u08CE"
#define CHAR_BACKGROUND_COLOR "\u08CF"
#define CHAR_FONT_COLOR       "\u08D0"
#define CHAR_PAINT            "\u08D1"
#define CHAR_TOGGLE_SMALL_ON  "\u08D2"
#define CHAR_TOGGLE_SMALL_OFF "\u08D3"
#define CHAR_TOGGLE_BODY      "\u08D4\u08D5"
#define CHAR_TOGGLE_SWITCH    "\u08D6"
#define CHAR_TOGGLE_ON        ""              // only for toggle state detection in list menu
#define CHAR_TOGGLE_OFF       ""              // only for toggle state detection in list menu

// this list is Auto-Generated. Please add new icons in CharIcon.inc only
static const char * const charIcon[CHARICON_NUM] = {
  #define X_CHAR(NAME) CHAR_##NAME ,
    #include "CharIcon.inc"
  #undef X_CHAR
};

uint8_t * IconCharSelect(uint8_t sel)
{
  return (uint8_t *)charIcon[sel];
}

// draw icons in item
void drawCharIcon(const GUI_RECT * rect, ALIGN_POSITION iconalign, uint16_t iconindex, bool drawBgColor, uint16_t btn_color)
{
  GUI_POINT icon_p = getTextStartPoint(rect->x0, rect->y0, rect->x1, rect->y1, iconalign, (char *)IconCharSelect(iconindex));

  GUI_SetColor(charIconColor[iconindex]);

  if (drawBgColor)
  {
    GUI_SetBkColor(btn_color);
    GUI_ClearPrect(rect);
  }

  GUI_DispString(icon_p.x, icon_p.y, IconCharSelect(iconindex));
  GUI_RestoreColorDefault();
}
