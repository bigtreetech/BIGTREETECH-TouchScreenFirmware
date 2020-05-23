#ifndef _SANITYCHECK_H_
#define _SANITYCHECK_H_

#include "includes.h"
#include "Configuration.h"
#include "flashStore.h"
#include "Settings.h"
//
//check size of settings against max allocated size at compile time
#define SIZE_CHECK(object) ((void)sizeof(char[1 - 2*!!(object)]))


#if CONFIG_VERSION != CONFIG_SUPPPORT
    #error "the Configuration.h is old. please use the latest Configuration.h file"
#endif

#ifdef ST7920_SPI
    #ifdef CLEAN_MODE_SWITCHING_SUPPORT
    #error "CLEAN_MODE_SWITCHING_SUPPORT is now SERIAL_ALWAYS_ON. Please update your configuration."
    #endif
#endif

#ifdef LED_COLOR_PIN

  #ifdef STARTUP_KNOB_LED_COLOR
    #if STARTUP_KNOB_LED_COLOR < 0
        #error "STARTUP_knob_LED_COLOR cannot be less than 1"
    #endif

    #if STARTUP_KNOB_LED_COLOR > 8
        #error "STARTUP_knob_LED_COLOR cannot be greater than 9"
    #endif
  #else
        #define STARTUP_KNOB_LED_COLOR 0
  #endif

#else

    #define STARTUP_KNOB_LED_COLOR 0

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

#ifndef ST7920_BANNER_TEXT
    #define ST7920_BANNER_TEXT "LCD12864 Simulator"
#endif

#if TOOL_NUM > MAX_TOOL_COUNT
    #error "TOOL_NUM can not be more than 6"
#endif

#if EXTRUDER_NUM > MAX_EXT_COUNT
    #error "EXTRUDER_NUM can not be more than 6"
#endif

#if FAN_NUM > MAX_FAN_COUNT
    #error "FAN_NUM can not be more than 6"
#endif

#ifdef CUSTOM_0_LABEL
    #define ENABLE_CUSTOM0 1
#else
    #define ENABLE_CUSTOM0 0
    #define CUSTOM_0_LABEL ""
    #define CUSTOM_0_GCODE ""
#endif

#ifdef CUSTOM_1_LABEL
    #define ENABLE_CUSTOM1 1
#else
    #define ENABLE_CUSTOM1 0
    #define CUSTOM_1_LABEL ""
    #define CUSTOM_1_GCODE ""
#endif

#ifdef CUSTOM_2_LABEL
    #define ENABLE_CUSTOM2 1
#else
    #define ENABLE_CUSTOM2 0
    #define CUSTOM_2_LABEL ""
    #define CUSTOM_2_GCODE ""
#endif

#ifdef CUSTOM_3_LABEL
    #define ENABLE_CUSTOM3 1
#else
    #define ENABLE_CUSTOM3 0
    #define CUSTOM_3_LABEL ""
    #define CUSTOM_3_GCODE ""
#endif

#ifdef CUSTOM_4_LABEL
    #define ENABLE_CUSTOM4 1
#else
    #define ENABLE_CUSTOM4 0
    #define CUSTOM_4_LABEL ""
    #define CUSTOM_4_GCODE ""
#endif

#ifdef CUSTOM_5_LABEL
    #define ENABLE_CUSTOM5 1
#else
    #define ENABLE_CUSTOM5 0
    #define CUSTOM_5_LABEL ""
    #define CUSTOM_5_GCODE ""
#endif

#ifdef CUSTOM_6_LABEL
    #define ENABLE_CUSTOM6 1
#else
    #define ENABLE_CUSTOM6 0
    #define CUSTOM_6_LABEL ""
    #define CUSTOM_6_GCODE ""
#endif

#ifdef CUSTOM_7_LABEL
    #define ENABLE_CUSTOM7 1
#else
    #define ENABLE_CUSTOM7 0
    #define CUSTOM_7_LABEL ""
    #define CUSTOM_7_GCODE ""
#endif

#ifdef CUSTOM_8_LABEL
    #define ENABLE_CUSTOM8 1
#else
    #define ENABLE_CUSTOM8 0
    #define CUSTOM_8_LABEL ""
    #define CUSTOM_8_GCODE ""
#endif

#ifdef CUSTOM_9_LABEL
    #define ENABLE_CUSTOM9 1
#else
    #define ENABLE_CUSTOM9 0
    #define CUSTOM_9_LABEL ""
    #define CUSTOM_9_GCODE ""
#endif

#ifdef CUSTOM_10_LABEL
    #define ENABLE_CUSTOM10 1
#else
    #define ENABLE_CUSTOM10 0
    #define CUSTOM_10_LABEL ""
    #define CUSTOM_10_GCODE ""
#endif

#ifdef CUSTOM_11_LABEL
    #define ENABLE_CUSTOM11 1
#else
    #define ENABLE_CUSTOM11 0
    #define CUSTOM_11_LABEL ""
    #define CUSTOM_11_GCODE ""
#endif

#ifdef CUSTOM_12_LABEL
    #define ENABLE_CUSTOM12 1
#else
    #define ENABLE_CUSTOM12 0
    #define CUSTOM_12_LABEL ""
    #define CUSTOM_12_GCODE ""
#endif

#ifdef CUSTOM_13_LABEL
    #define ENABLE_CUSTOM13 1
#else
    #define ENABLE_CUSTOM13 0
    #define CUSTOM_13_LABEL ""
    #define CUSTOM_13_GCODE ""
#endif

#ifdef CUSTOM_14_LABEL
    #define ENABLE_CUSTOM14 1
#else
    #define ENABLE_CUSTOM14 0
    #define CUSTOM_14_LABEL ""
    #define CUSTOM_14_GCODE ""
#endif


#define CUSTOM_GCODE_ENABLED {ENABLE_CUSTOM0, ENABLE_CUSTOM1, ENABLE_CUSTOM2, ENABLE_CUSTOM3, ENABLE_CUSTOM4,\
                              ENABLE_CUSTOM5, ENABLE_CUSTOM6, ENABLE_CUSTOM7, ENABLE_CUSTOM8, ENABLE_CUSTOM9,\
                              ENABLE_CUSTOM10,ENABLE_CUSTOM11,ENABLE_CUSTOM12,ENABLE_CUSTOM13,ENABLE_CUSTOM14}

#define CUSTOM_GCODE_LIST    {CUSTOM_0_GCODE, CUSTOM_1_GCODE, CUSTOM_2_GCODE, CUSTOM_3_GCODE, CUSTOM_4_GCODE,\
                              CUSTOM_5_GCODE, CUSTOM_6_GCODE, CUSTOM_7_GCODE, CUSTOM_8_GCODE, CUSTOM_9_GCODE,\
                              CUSTOM_10_GCODE,CUSTOM_11_GCODE,CUSTOM_12_GCODE,CUSTOM_13_GCODE,CUSTOM_14_GCODE }

#define CUSTOM_GCODE_LABELS    {CUSTOM_0_LABEL, CUSTOM_1_LABEL, CUSTOM_2_LABEL, CUSTOM_3_LABEL, CUSTOM_4_LABEL,\
                              CUSTOM_5_LABEL, CUSTOM_6_LABEL, CUSTOM_7_LABEL, CUSTOM_8_LABEL, CUSTOM_9_LABEL,\
                              CUSTOM_10_LABEL,CUSTOM_11_LABEL,CUSTOM_12_LABEL,CUSTOM_13_LABEL,CUSTOM_14_LABEL }


#endif //_SANITYCHECK_H_
