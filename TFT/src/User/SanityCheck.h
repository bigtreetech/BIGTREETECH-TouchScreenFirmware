#ifndef _SANITYCHECK_H_
#define _SANITYCHECK_H_

#include "includes.h"
#include "Configuration.h"
#include "flashStore.h"
#include "Settings.h"

//check size of settings against max allocated size at compile time
#define SIZE_CHECK(object) ((void)sizeof(char[1 - 2*!!(object)]))

#if defined(TFT35_V2_0) || defined(TFT35_V3_0) || defined(TFT35_B1_V3_0) || defined(TFT35_E3_V3_0)
  #ifdef SCREEN_SHOT_TO_SD
    #error "Hardware error, need to change the TFT Pin39 from GND to 3.3V to use this feature. Otherwise, the color read out is incorrect"
  #endif
#endif

#if CONFIG_VERSION != CONFIG_SUPPPORT
  #error "the Configuration.h is old. please use the latest Configuration.h file"
#endif

#if BAUDRATE < 0 || BAUDRATE >= BAUDRATE_COUNT
  #error "invalid Baudrate index. Pleas select a value only from options provided in configuration.h"
#endif

#ifdef ST7920_EMULATOR
  #ifdef CLEAN_MODE_SWITCHING_SUPPORT
    #error "CLEAN_MODE_SWITCHING_SUPPORT is now SERIAL_ALWAYS_ON. Please update your configuration."
  #endif
#else
  #if defined(_PIN_TFT35_V2_0_H_) || defined(_PIN_TFT35_V1_0_H_)
    #ifdef DEFAULT_LCD_MODE
      #undef DEFAULT_LCD_MODE
    #endif
    #define DEFAULT_LCD_MODE MODE_SERIAL_TSC  // Just set hardcoded here.
    //#warning "DEFAULT_LCD_MODE supports only SERIAL_TSC. Please update/check your configuration."
  #endif
#endif

#ifdef LED_COLOR_PIN
  #ifdef STARTUP_KNOB_LED_COLOR
    #if STARTUP_KNOB_LED_COLOR < 0
      #error "STARTUP_KNOB_LED_COLOR cannot be less than 1"
    #endif

    #if STARTUP_KNOB_LED_COLOR > 8
      #error "STARTUP_KNOB_LED_COLOR cannot be greater than 9"
    #endif
  #else
    #define STARTUP_KNOB_LED_COLOR 0
  #endif
#else
  #ifdef STARTUP_KNOB_LED_COLOR
    #if STARTUP_KNOB_LED_COLOR > 0
      //#warning "STARTUP_KNOB_LED_COLOR is not supported on this target and must be set to 0"
      #undef STARTUP_KNOB_LED_COLOR
    #endif
  #endif
  #define STARTUP_KNOB_LED_COLOR 0
#endif

#ifdef EXTRUDE_STEPS
  #error "EXTRUDE_STEPS is now auto-configured with 'M92'. Please remove EXTRUDE_STEPS from your Configuration.h file."
#endif

#ifdef AUTO_BED_LEVELING
  #error "AUTO_BED_LEVELING is now auto-configured with 'M115'. Please remove AUTO_BED_LEVELING from your Configuration.h file."
#endif

#ifdef ENABLE_BL_VALUE
  #if ENABLE_BL_VALUE > 5
    #error "ENABLE_BL_VALUE cannot be greater than 5"
  #endif

  #if ENABLE_BL_VALUE < 0
    #error "ENABLE_BL_VALUE cannot be less than 0"
  #endif
#endif
#ifndef ENABLE_BL_VALUE
  #define ENABLE_BL_VALUE 1
#endif

#ifdef MESH_GRID_MAX_POINTS_X
  #if MESH_GRID_MAX_POINTS_X > 15
    #error "MESH_GRID_MAX_POINTS_X cannot be greater than 15"
  #endif

  #if MESH_GRID_MAX_POINTS_X < 1
    #error "MESH_GRID_MAX_POINTS_X cannot be less than 1"
  #endif
#endif
#ifndef MESH_GRID_MAX_POINTS_X
  #define MESH_GRID_MAX_POINTS_X 10
#endif

#ifdef MESH_GRID_MAX_POINTS_Y
  #if MESH_GRID_MAX_POINTS_Y > 15
    #error "MESH_GRID_MAX_POINTS_Y cannot be greater than 15"
  #endif

  #if MESH_GRID_MAX_POINTS_Y < 1
    #error "MESH_GRID_MAX_POINTS_Y cannot be less than 1"
  #endif
#endif
#ifndef MESH_GRID_MAX_POINTS_Y
  #define MESH_GRID_MAX_POINTS_Y 10
#endif

#ifdef KEYBOARD_COLOR_LAYOUT
  #if KEYBOARD_COLOR_LAYOUT > 2
    #error "KEYBOARD_COLOR_LAYOUT cannot be greater than 2"
  #endif

  #if KEYBOARD_COLOR_LAYOUT < 0
    #error "KEYBOARD_COLOR_LAYOUT cannot be less than 0"
  #endif
#endif
#ifndef KEYBOARD_COLOR_LAYOUT
  #define KEYBOARD_COLOR_LAYOUT 0
#endif
#if KEYBOARD_COLOR_LAYOUT == 2 && defined(KEYBOARD_MATERIAL_THEME)
  #undef KEYBOARD_MATERIAL_THEME
#endif

#ifdef CANCEL_PRINT_GCODE
  #error "CANCEL_PRINT_GCODE is now PRINT_CANCEL_GCODE. Please update your Configuration.h file."
#endif

#ifndef MARLIN_BANNER_TEXT
  #define MARLIN_BANNER_TEXT "Marlin Mode"
#endif

#ifdef TOOL_NUM
  #error "TOOL_NUM is now HOTEND_NUM. Please update your Configuration.h file."
#endif

#if HOTEND_NUM > MAX_HOTEND_COUNT
  #error "HOTEND_NUM can not be more than 6"
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
                              CUSTOM_10_GCODE,CUSTOM_11_GCODE,CUSTOM_12_GCODE,CUSTOM_13_GCODE,CUSTOM_14_GCODE}

#define CUSTOM_GCODE_LABELS  {CUSTOM_0_LABEL, CUSTOM_1_LABEL, CUSTOM_2_LABEL, CUSTOM_3_LABEL, CUSTOM_4_LABEL,\
                              CUSTOM_5_LABEL, CUSTOM_6_LABEL, CUSTOM_7_LABEL, CUSTOM_8_LABEL, CUSTOM_9_LABEL,\
                              CUSTOM_10_LABEL,CUSTOM_11_LABEL,CUSTOM_12_LABEL,CUSTOM_13_LABEL,CUSTOM_14_LABEL}

#endif //_SANITYCHECK_H_
