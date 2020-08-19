#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "stdbool.h"
#include "variants.h"


enum
{
  ENGLISH = 0,
  CHINESE,
  RUSSIAN,
  JAPANESE,
  ARMENIAN,
  GERMAN,
  CZECH,
  SPAIN,
  FRENCH,
  PORTUGUESE,
  ITALIAN,
  POLISH,
  SLOVAK,
  DUTCH,
  HUNGARIAN,
  TURKISH,
  GREEK,
  SLOVENIAN,
  CATALAN,
  TRAD_CHINESE,
  
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

uint8_t * textSelect(uint16_t sel);

#endif
