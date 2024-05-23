#ifndef _CHAR_ICON_H_
#define _CHAR_ICON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "GUI.h"

// this list is Auto-Generated. Please add new icons in CharIcon.inc only
typedef enum
{
  #define X_CHAR(NAME) CHARICON_##NAME ,
    #include "CharIcon.inc"
  #undef X_CHAR

  // keep the following always at the end of this list
  CHARICON_NUM,
  CHARICON_NULL
} CHAR_ICON;

extern const uint16_t charIconColor[CHARICON_NUM];

uint8_t * IconCharSelect(uint8_t sel);
void drawCharIcon(const GUI_RECT * rect, ALIGN_POSITION iconalign, uint16_t iconindex, bool drawBgColor, uint16_t btn_color);

#ifdef __cplusplus
}
#endif

#endif
