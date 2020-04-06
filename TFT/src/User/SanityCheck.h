#ifndef _SANITYCHECK_H_
#define _SANITYCHECK_H_

#include "Configuration.h"

#ifndef ST7920_SPI
  #ifdef CLEAN_MODE_SWITCHING_SUPPORT
  #error "CLEAN_MODE_SWITCHING_SUPPORT can only be enabled for TFT controllers which support ST7920 Emulator/Marlin Mode. Disable CLEAN_MODE_SWITCHING_SUPPORT in Configuration.h"
  #endif
#endif

#ifdef LED_COLOR_PIN
  #ifdef STARTUP_KNOB_LED_COLOR
    #if STARTUP_KNOB_LED_COLOR < 1
    #error "STARTUP_knob_LED_COLOR cannot be less than 1"
    #endif

    #if STARTUP_KNOB_LED_COLOR > 9
    #error "STARTUP_knob_LED_COLOR cannot be greater than 9"
    #endif
  #else
  #define STARTUP_KNOB_LED_COLOR 1
  #endif
  #else
  #define STARTUP_KNOB_LED_COLOR 1
#endif

#ifdef EXTRUDE_STEPS
  #error "NEED NOT THIS, E Steps will be auto get by M92 after start up"
#endif
#ifdef AUTO_BED_LEVELING
  #error "AUTO_BED_LEVELING is now auto configured by the TFT Controller with 'M115' command. Please update your Configuration.h file"
#endif
#endif
