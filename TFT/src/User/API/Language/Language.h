#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"

#define MAX_LANG_LABEL_LENGTH W25QXX_SPI_PAGESIZE

#define ENGLISH      0
#define CHINESE      1
#define RUSSIAN      2
#define JAPANESE     3
#define ARMENIAN     4
#define GERMAN       5
#define CZECH        6
#define SPANISH      7
#define FRENCH       8
#define PORTUGUESE   9
#define ITALIAN      10
#define POLISH       11
#define SLOVAK       12
#define DUTCH        13
#define HUNGARIAN    14
#define TURKISH      15
#define GREEK        16
#define SLOVENIAN    17
#define CATALAN      18
#define TRAD_CHINESE 19
#define UKRAINIAN    20
#define BRAZIL       21
#define CROATIAN     22

enum
{
  LANG_DEFAULT = 0,
  LANG_FLASH,
  LANGUAGE_NUM,
};

// this list is Auto-Generated. Please add new keywords in Language.inc file only
enum
{
  #define X_WORD(NAME) LABEL_##NAME ,
    #include "Language.inc"
  #undef X_WORD

  // keep the following always at the end of this list
  LABEL_NUM,
  LABEL_NULL,
  LABEL_DYNAMIC,
  LABEL_CUSTOM_VALUE,
};

extern const char * const default_pack[LABEL_NUM];
extern const char * const lang_key_list[LABEL_NUM];

// use only once in any function call. Calling multiple times will overwrite previous text
uint8_t * textSelect(uint16_t sel);

// get the address of the label in SPI flash
uint32_t getLabelFlashAddr(uint16_t index);

// load selected label text into buffer form spi flash
bool loadLabelText(uint8_t * buf, uint16_t index);

// initialize and preload label text
#define LABELCHAR(x, i) char x[MAX_LANG_LABEL_LENGTH]; loadLabelText((uint8_t *)&x, i);

#ifdef __cplusplus
}
#endif

#endif
