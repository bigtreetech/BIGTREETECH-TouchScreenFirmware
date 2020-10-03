#define LANGUAGE_CHECK
#include "Language.h"
#include "includes.h"

#if LANGUAGE_CHECK==0
#include "language_en.h"
#elif LANGUAGE_CHECK==1
#include "language_cn.h"
#elif LANGUAGE_CHECK==2
#include "language_ru.h"
#elif LANGUAGE_CHECK==3
#include "language_jp.h"
#elif LANGUAGE_CHECK==5
#include "language_de.h"
#elif LANGUAGE_CHECK==4
#include "language_am.h"
#elif LANGUAGE_CHECK==6
#include "language_cz.h"
#elif LANGUAGE_CHECK==7
#include "language_es.h"
#elif LANGUAGE_CHECK==8
#include "language_fr.h"
#elif LANGUAGE_CHECK==9
#include "language_pt.h"
#elif LANGUAGE_CHECK==10
#include "language_it.h"
#elif LANGUAGE_CHECK==11
#include "language_pl.h"
#elif LANGUAGE_CHECK==12
#include "language_sk.h"
#elif LANGUAGE_CHECK==13
#include "language_du.h"
#elif LANGUAGE_CHECK==14
#include "language_hu.h"
#elif LANGUAGE_CHECK==15
#include "language_tr.h"
#elif LANGUAGE_CHECK==16
#include "language_gr.h"
#elif LANGUAGE_CHECK==17
#include "language_sl.h"
#elif LANGUAGE_CHECK==18
#include "language_ca.h"
#elif LANGUAGE_CHECK==19
#include "language_tc.h"
#endif
//
// Add new Keywords in Language.inc file Only
//
#if(LANGUAGE_CHECK==0)
const char *const en_pack[LABEL_NUM] = {
  #define X_WORD(NAME) EN_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};



#elif(LANGUAGE_CHECK==1)
const char *const cn_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CN_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};


#elif(LANGUAGE_CHECK==2)
const char *const ru_pack[LABEL_NUM] = {
  #define X_WORD(NAME) RU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};


#elif(LANGUAGE_CHECK==3)
const char *const jp_pack[LABEL_NUM] = {
  #define X_WORD(NAME) JP_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};


#elif(LANGUAGE_CHECK==4)
const char *const am_pack[LABEL_NUM] = {
  #define X_WORD(NAME) AM_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};


#elif(LANGUAGE_CHECK==5)
const char *const de_pack[LABEL_NUM] = {
  #define X_WORD(NAME) DE_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==6)
const char *const cz_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CZ_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==7)
const char *const es_pack[LABEL_NUM] = {
  #define X_WORD(NAME) ES_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==8)
const char *const fr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) FR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==9)
const char *const pt_pack[LABEL_NUM] = {
  #define X_WORD(NAME) PT_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==10)
const char *const it_pack[LABEL_NUM] = {
  #define X_WORD(NAME) IT_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==11)
const char *const pl_pack[LABEL_NUM] = {
  #define X_WORD(NAME) PL_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==12)
const char *const sk_pack[LABEL_NUM] = {
  #define X_WORD(NAME) SK_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==13)
const char *const du_pack[LABEL_NUM] = {
  #define X_WORD(NAME) DU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==14)
const char *const hu_pack[LABEL_NUM] = {
  #define X_WORD(NAME) HU_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==15)
const char *const tr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) TR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==16)
const char *const gr_pack[LABEL_NUM] = {
  #define X_WORD(NAME) GR_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==17)
const char *const sl_pack[LABEL_NUM] = {
  #define X_WORD(NAME) SL_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==18)
const char *const ca_pack[LABEL_NUM] = {
  #define X_WORD(NAME) CA_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};

#elif(LANGUAGE_CHECK==19)
const char *const tc_pack[LABEL_NUM] = {
  #define X_WORD(NAME) TC_##NAME ,
  #include "Language.inc"
  #undef  X_WORD
};
#endif

#if(LANGUAGE_CHECK==0)
uint8_t *textSelect(uint16_t sel)
{
  return (uint8_t *)en_pack[sel];
}

#elif(LANGUAGE_CHECK==1)
uint8_t *textSelect(uint16_t sel)
{
     return (uint8_t *)cn_pack[sel];
}
   
#elif(LANGUAGE_CHECK==2)
uint8_t *textSelect(uint16_t sel)
{
     return (uint8_t *)ru_pack[sel];
}

#elif(LANGUAGE_CHECK==3)
uint8_t *textSelect(uint16_t sel)
{
    return (uint8_t *)jp_pack[sel];
}

#elif(LANGUAGE_CHECK==4)
 uint8_t *textSelect(uint16_t sel)
{
    return (uint8_t *)am_pack[sel];
}

#elif(LANGUAGE_CHECK==5)
 uint8_t *textSelect(uint16_t sel)
{
      return (uint8_t *)de_pack[sel];
}

#elif(LANGUAGE_CHECK==6)
 uint8_t *textSelect(uint16_t sel)
{
       return (uint8_t *)cz_pack[sel];
}

#elif(LANGUAGE_CHECK==7)
 uint8_t *textSelect(uint16_t sel)
{
       return (uint8_t *)es_pack[sel];
}

#elif(LANGUAGE_CHECK==8)
 uint8_t *textSelect(uint16_t sel)
{
      return (uint8_t *)fr_pack[sel];
}
   
#elif(LANGUAGE_CHECK==9)
 uint8_t *textSelect(uint16_t sel)
{
    case PORTUGUESE:    return (uint8_t *)pt_pack[sel];
}

#elif(LANGUAGE_CHECK==10)
 uint8_t *textSelect(uint16_t sel)
{
     return (uint8_t *)it_pack[sel];
}

#elif(LANGUAGE_CHECK==11)
 uint8_t *textSelect(uint16_t sel)
{
      return (uint8_t *)pl_pack[sel];
}

#elif(LANGUAGE_CHECK==12)
 uint8_t *textSelect(uint16_t sel)
{
      return (uint8_t *)sk_pack[sel];
}

#elif(LANGUAGE_CHECK==13)
 uint8_t *textSelect(uint16_t sel)
{
       return (uint8_t *)du_pack[sel];
}

#elif(LANGUAGE_CHECK==14)
 uint8_t *textSelect(uint16_t sel)
{
     case HUNGARIAN:     return (uint8_t *)hu_pack[sel];
}

#elif(LANGUAGE_CHECK==15)
 uint8_t *textSelect(uint16_t sel)
{
      return (uint8_t *)tr_pack[sel];
}

#elif(LANGUAGE_CHECK==16)
 uint8_t *textSelect(uint16_t sel)
{
       return (uint8_t *)gr_pack[sel];
}

#elif(LANGUAGE_CHECK==17)
 uint8_t *textSelect(uint16_t sel)
{
    case SLOVENIAN:     return (uint8_t *)sl_pack[sel];
}

#elif(LANGUAGE_CHECK==18)
 uint8_t *textSelect(uint16_t sel)
{
     return (uint8_t *)ca_pack[sel];
}

#elif(LANGUAGE_CHECK==19)
 uint8_t *textSelect(uint16_t sel)
{
    return (uint8_t *)tc_pack[sel];
}
#endif
