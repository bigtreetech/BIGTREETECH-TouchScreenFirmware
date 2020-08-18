#include "Language.h"
#include "includes.h"
#include "language_en.h"
#include "language_cn.h"
#include "language_ru.h"
#include "language_jp.h"
#include "language_de.h"
#include "language_am.h"
#include "language_cz.h"
#include "language_es.h"
#include "language_fr.h"
#include "language_pt.h"
#include "language_it.h"
#include "language_pl.h"
#include "language_sk.h"
#include "language_du.h"
#include "language_hu.h"
#include "language_tr.h"
#include "language_gr.h"
#include "language_sl.h"
#include "language_ca.h"
#include "language_tc.h"
//
// Add new Keywords in Language.inc file Only
//
const char *const en_pack[LABEL_NUM] = {
  #define X_WORD(NAME) EN_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const cn_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CN_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const ru_pack[LABEL_NUM] = {
  #define X_WORD(NAME) RU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const jp_pack[LABEL_NUM] = {
  #define X_WORD(NAME) JP_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const am_pack[LABEL_NUM] = {
  #define X_WORD(NAME) AM_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const de_pack[LABEL_NUM] = {
  #define X_WORD(NAME) DE_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const cz_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CZ_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const es_pack[LABEL_NUM] = {
  #define X_WORD(NAME) ES_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const fr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) FR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const pt_pack[LABEL_NUM] = {
  #define X_WORD(NAME) PT_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const it_pack[LABEL_NUM] = {
  #define X_WORD(NAME) IT_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const pl_pack[LABEL_NUM] = {
  #define X_WORD(NAME) PL_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const sk_pack[LABEL_NUM] = {
  #define X_WORD(NAME) SK_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const du_pack[LABEL_NUM] = {
  #define X_WORD(NAME) DU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const hu_pack[LABEL_NUM] = {
  #define X_WORD(NAME) HU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const tr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) TR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const gr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) GR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const sl_pack[LABEL_NUM] = {
  #define X_WORD(NAME) SL_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const ca_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CA_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

const char *const tc_pack[LABEL_NUM] = {
  #define X_WORD(NAME) TC_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

uint8_t *textSelect(uint16_t sel)
{
  switch(infoSettings.language)
  {
    case ENGLISH:       return (uint8_t *)en_pack[sel];
    case CHINESE:       return (uint8_t *)cn_pack[sel];
    case RUSSIAN:       return (uint8_t *)ru_pack[sel];
    case JAPANESE:      return (uint8_t *)jp_pack[sel];
    case ARMENIAN:      return (uint8_t *)am_pack[sel];
    case GERMAN:        return (uint8_t *)de_pack[sel];
    case CZECH:         return (uint8_t *)cz_pack[sel];
    case SPAIN:         return (uint8_t *)es_pack[sel];
    case FRENCH:        return (uint8_t *)fr_pack[sel];
    case PORTUGUESE:    return (uint8_t *)pt_pack[sel];
    case ITALIAN:       return (uint8_t *)it_pack[sel];
    case POLISH:        return (uint8_t *)pl_pack[sel];
    case SLOVAK:        return (uint8_t *)sk_pack[sel];
    case DUTCH:         return (uint8_t *)du_pack[sel];
    case HUNGARIAN:     return (uint8_t *)hu_pack[sel];
    case TURKISH:       return (uint8_t *)tr_pack[sel];
    case GREEK:         return (uint8_t *)gr_pack[sel];
    case SLOVENIAN:     return (uint8_t *)sl_pack[sel];
    case CATALAN:       return (uint8_t *)ca_pack[sel];
    case TRAD_CHINESE:  return (uint8_t *)tc_pack[sel];

    default:            return NULL;
  }
}
