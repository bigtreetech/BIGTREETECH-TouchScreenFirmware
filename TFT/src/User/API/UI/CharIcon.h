#ifndef _CHAR_ICON_H_
#define _CHAR_ICON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
  #define X_CHAR(NAME) CHARICON_##NAME ,
  #include "CharIcon.inc"
  #undef  X_CHAR
  //keep below items always at the end
  CHARICON_NUM,
  CHARICON_BACKGROUND
} CHAR_ICON;

extern const uint16_t charIconColor[CHARICON_NUM];

uint8_t * IconCharSelect(uint8_t sel);
char * IconChar(uint8_t sel);

#ifdef __cplusplus
}
#endif

#endif
