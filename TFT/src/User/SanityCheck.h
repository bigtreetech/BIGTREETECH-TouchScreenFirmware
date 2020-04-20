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
  #error "EXTRUDE_STEPS is now auto-configured with 'M92'. Please remove EXTRUDE_STEPS from your Configuration.h file."
#endif

#ifdef AUTO_BED_LEVELING
  #error "AUTO_BED_LEVELING is now auto-configured with 'M115'. Please remove AUTO_BED_LEVELING from your Configuration.h file."
#endif

#ifdef CANCEL_PRINT_GCODE
  #error "CANCEL_PRINT_GCODE is now PRINT_CANCEL_GCODE. Please update your Configuration.h file."
#endif

#endif
