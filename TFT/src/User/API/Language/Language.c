#include "Language.h"
#include "includes.h"
#include "language_keywords.h"

#if DEFAULT_LANGUAGE == ENGLISH
#include "language_en.h"
#elif DEFAULT_LANGUAGE == CHINESE
#include "language_cn.h"
#elif DEFAULT_LANGUAGE == RUSSIAN
#include "language_ru.h"
#elif DEFAULT_LANGUAGE == JAPANESE
#include "language_jp.h"
#elif DEFAULT_LANGUAGE == GERMAN
#include "language_de.h"
#elif DEFAULT_LANGUAGE == ARMENIAN
#include "language_am.h"
#elif DEFAULT_LANGUAGE == CZECH
#include "language_cz.h"
#elif DEFAULT_LANGUAGE == SPANISH
#include "language_es.h"
#elif DEFAULT_LANGUAGE == FRENCH
#include "language_fr.h"
#elif DEFAULT_LANGUAGE == PORTUGUESE
#include "language_pt.h"
#elif DEFAULT_LANGUAGE == ITALIAN
#include "language_it.h"
#elif DEFAULT_LANGUAGE == POLISH
#include "language_pl.h"
#elif DEFAULT_LANGUAGE == SLOVAK
#include "language_sk.h"
#elif DEFAULT_LANGUAGE == DUTCH
#include "language_du.h"
#elif DEFAULT_LANGUAGE == HUNGARIAN
#include "language_hu.h"
#elif DEFAULT_LANGUAGE == TURKISH
#include "language_tr.h"
#elif DEFAULT_LANGUAGE == GREEK
#include "language_gr.h"
#elif DEFAULT_LANGUAGE == SLOVENIAN
#include "language_sl.h"
#elif DEFAULT_LANGUAGE == CATALAN
#include "language_ca.h"
#elif DEFAULT_LANGUAGE == TRAD_CHINESE
#include "language_tc.h"
#elif DEFAULT_LANGUAGE == UKRAINIAN
#include "language_uk.h"
#else
  #error "Error: invalid language defined"
#endif
//
// Add new Keywords in Language.inc file Only
//
uint8_t tempLabelString[MAX_LANG_LABEL_LENGTH];

const char *const default_pack[LABEL_NUM] = {
  #define X_WORD(NAME) STRING_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

//keyword list for language.ini
const char *const lang_key_list[LABEL_NUM] =
{
  #define X_WORD(NAME) LANG_KEY_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

uint8_t *textSelect(uint16_t sel)
{
  switch(infoSettings.language)
  {
    case LANG_DEFAULT:
      return (uint8_t *)default_pack[sel];
    case LANG_FLASH:
      loadLabelText(tempLabelString, sel);
      return tempLabelString;
    default:
      return NULL;
  }
}

uint32_t getLabelFlashAddr(uint16_t index)
{
  if (index > LABEL_BACKGROUND) return LANGUAGE_ADDR;
  return (LANGUAGE_ADDR + (MAX_LANG_LABEL_LENGTH * index));
}

bool loadLabelText(uint8_t* buf, uint16_t index)
{
  if(index >= LABEL_NUM) return false;
  if(infoSettings.language == LANG_FLASH)
    W25Qxx_ReadBuffer(buf, getLabelFlashAddr(index), MAX_LANG_LABEL_LENGTH);
  else
    memcpy(buf, textSelect(index), sizeof(tempLabelString));
  return true;
}
