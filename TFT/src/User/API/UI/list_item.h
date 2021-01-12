// *List View Mode ** Author: Gurmeet Athwal https://github.com/guruathwal **

#ifndef _LIST_ITEM_H_
#define _LIST_ITEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "GUI.h"
#include "menu.h"

typedef enum
{
  #define X_CHAR(NAME) ICONCHAR_##NAME ,
  #include "Char_Icon.inc"
  #undef  X_CHAR
  //keep below items always at the end
  ICONCHAR_NUM,
  ICONCHAR_BACKGROUND
} ICON_CHAR;

typedef enum
{
  TOP_LEFT = 0,
  TOP_CENTER,
  TOP_RIGHT,
  LEFT_CENTER,
  MIDDLE,
  RIGHT_CENTER,
  BOTTOM_LEFT,
  BOTTOM_CENTER,
  BOTTOM_RIGHT
} ICON_POS;

#define LISTBTN_BKCOLOR 0x2187
#define MAT_RED         0xE124
#define MAT_YELLOW      0xED80
#define MAT_GREEN       0x1DC4
#define MAT_BLUE        0x24BD
#define MAT_ORANGE      0xF3A0
#define MATT_DARKGRAY   0x52AA
#define MAT_LOWWHITE    0xCE79
#define MATT_PURPLE     0x9135

uint8_t * IconCharSelect(uint8_t sel);

void setDynamicLabel(uint8_t i, char * label);  //set list item title to any text
char * getDynamicLabel(uint8_t i);              //get the custom text of the list item title

void setDynamicTextValue(uint8_t i, char * txt);  //set list item value to any text (upto 7 characters)
char * getDynamicTextValue(uint8_t i);            //get the custom text of the list item value
void setDynamicValue(uint8_t i, float value);     //set list item value to any numeric value (upto 7 digits)

void DrawCharIcon(const GUI_RECT * rect, ICON_POS iconalign, uint16_t iconindex, bool drawBgColor, uint16_t btn_color);
void ListItem_Display(const GUI_RECT * rect, uint8_t position, const LISTITEM * curitem, bool pressed);
void ListMenuSetItem(const LISTITEM * menuItem, uint8_t position);
void draw_itemtitle(GUI_POINT pos, LABEL label, uint8_t position, int textarea_width);
void ListItem_DisplayToggle(uint16_t sx, uint16_t sy, uint8_t iconchar_state);
void ListItem_DisplayCustomValue(const GUI_RECT * rect, LABEL value, int i);
GUI_POINT getTextStartPoint(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, ICON_POS pos, const char * textchar);

#ifdef __cplusplus
}
#endif

#endif
