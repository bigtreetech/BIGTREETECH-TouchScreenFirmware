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

#define LISTBTN_BKCOLOR MAT_DARKSLATE

void setDynamicLabel(uint8_t i, char * label);  //set list item title to any text
char * getDynamicLabel(uint8_t i);              //get the custom text of the list item title

void setDynamicTextValue(uint8_t i, char * txt);  //set list item value to any text (upto 7 characters)
char * getDynamicTextValue(uint8_t i);            //get the custom text of the list item value
void setDynamicValue(uint8_t i, float value);     //set list item value to any numeric value (upto 7 digits)

void ListItem_Display(const GUI_RECT * rect, uint8_t position, const LISTITEM * curitem, bool pressed);
void ListMenuSetItem(const LISTITEM * menuItem, uint8_t position);
void draw_itemtitle(GUI_POINT pos, LABEL label, uint8_t position, int textarea_width);
void ListItem_DisplayToggle(uint16_t sx, uint16_t sy, uint8_t iconchar_state);
void ListItem_DisplayCustomValue(const GUI_RECT * rect, LABEL value, int i);
GUI_POINT getTextStartPoint(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey,ALIGN_POSITION pos, const char * textchar);

#ifdef __cplusplus
}
#endif

#endif
