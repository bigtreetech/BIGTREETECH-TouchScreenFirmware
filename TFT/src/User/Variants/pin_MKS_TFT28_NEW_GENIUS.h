#ifndef _PIN_MKS_TFT28_NEW_GENIUS_H_  // modify to actual filename !!!
#define _PIN_MKS_TFT28_NEW_GENIUS_H_  // modify to actual filename !!!

// Update folder for fonts and icons
#ifndef UPDATE_DIR
  #define UPDATE_DIR "TFT28"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT28_NEW_GENIUS"
#endif

#if !defined(ST7920_EMULATOR) || defined(SPI3_PIN_SMART_USAGE)
  // Power Supply Auto Shutdown Detection pin
  #ifndef PS_ON_PIN
    #define PS_ON_PIN PB1
  #endif

  // Filament Runout Detection pin
  #ifndef FIL_RUNOUT_PIN
    #define FIL_RUNOUT_PIN PB0
  #endif
#endif

#include "pin_MKS_TFT32_V1_4.h"

#endif
