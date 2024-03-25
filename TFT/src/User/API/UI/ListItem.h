// *List View Mode ** Author: Gurmeet Athwal https://github.com/guruathwal **

#ifndef _LIST_ITEM_H_
#define _LIST_ITEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "GUI.h"   // for GUI_RECT etc.
#include "menu.h"  // for LISTITEM etc.

void setDynamicLabel(uint8_t i, char * label);  // set list item title to any text
char * getDynamicLabel(uint8_t i);              // get the custom text of the list item title

void setDynamicTextValue(uint8_t i, char * txt);  // set list item value to any text (upto 7 characters)
char * getDynamicTextValue(uint8_t i);            // get the custom text of the list item value
void setDynamicValue(uint8_t i, float value);     // set list item value to any numeric value (upto 7 digits)

void ListItem_Display(const GUI_RECT * rect, uint8_t position, const LISTITEM * curitem, bool pressed);

#ifdef __cplusplus
}
#endif

#endif
