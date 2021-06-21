#ifndef _PIN_MKS_TFT28_NEW_GENIUS_H_ // modify to actual filename !!!
#define _PIN_MKS_TFT28_NEW_GENIUS_H_ // modify to actual filename !!!

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT28_NEW_GENIUS"
#endif

#if (!defined(ST7920_EMULATOR) && defined(FIL_RUNOUT_PIN))
  #define FIL_RUNOUT_PIN PB0
#endif
  
#include "pin_MKS_TFT32_V1_4.h"

#endif
