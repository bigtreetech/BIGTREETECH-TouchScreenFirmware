#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"

#define MAX_LANG_LABEL_LENGTH W25QXX_SPI_PAGESIZE


enum
{
  LANG_DEFAULT = 0,
  LANG_FLASH,
  LANGUAGE_NUM,
};

enum {
#define X_WORD(NAME) LABEL_##NAME ,
#include "Language.inc"
#undef  X_WORD

  //add new keywords in 'Language.inc' file only
  //keep the following always at the end of this list
  LABEL_NUM,
  LABEL_BACKGROUND,
  LABEL_DYNAMIC,
  LABEL_CUSTOM_VALUE,
};

extern const char *const lang_key_list[LABEL_NUM];

//use only once in any function call. Calling multiple times will overwrite previous text.
uint8_t * textSelect(uint16_t sel);

//get the address of the label in SPI flash
uint32_t getLabelFlashAddr(uint16_t index);

//load selected label text into buffer form spi flash
bool loadLabelText(uint8_t * buf, uint16_t index);

//initialize and preload label text
#define labelChar(x, i)  char x[MAX_LANG_LABEL_LENGTH]; loadLabelText((u8*)&x, i);

#ifdef __cplusplus
}
#endif

#endif
