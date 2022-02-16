#ifndef _SANITY_CHECK_H_
#define _SANITY_CHECK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "Configuration.h"
#include "FlashStore.h"
#include "SerialConnection.h"
#include "Settings.h"
#include "ui_draw.h"

// check size of settings against max allocated size at compile time
#define SIZE_CHECK(object) ((void)sizeof(char[1 - 2*!!(object)]))

#if CONFIG_VERSION != CONFIG_SUPPPORT
  #error "the Configuration.h is old. please use the latest Configuration.h file"
#endif

//====================================================================================================
//=============================== Settings Configurable On config.ini ================================
//====================================================================================================

#if SP_1 < 1 || SP_1 >= BAUDRATE_COUNT
  #error "invalid SP_1 index. Please select a value only from options provided in Configuration.h"
#endif

#if SP_2 < 0 || SP_2 >= BAUDRATE_COUNT
  #error "invalid SP_2 index. Please select a value only from options provided in Configuration.h"
#endif

#if SP_3 < 0 || SP_3 >= BAUDRATE_COUNT
  #error "invalid SP_3 index. Please select a value only from options provided in Configuration.h"
#endif

#if SP_4 < 0 || SP_4 >= BAUDRATE_COUNT
  #error "invalid SP_4 index. Please select a value only from options provided in Configuration.h"
#endif

#ifdef TERMINAL_COLOR_SCHEME
  #if TERMINAL_COLOR_SCHEME > 2
    #error "TERMINAL_COLOR_SCHEME cannot be greater than 2"
  #endif

  #if TERMINAL_COLOR_SCHEME < 0
    #error "TERMINAL_COLOR_SCHEME cannot be less than 0"
  #endif
#else
  #define TERMINAL_COLOR_SCHEME 0
#endif

#ifndef ST7920_EMULATOR
  #if defined(_PIN_TFT35_V2_0_H_) || defined(_PIN_TFT35_V1_0_H_)
    #ifdef DEFAULT_MODE
      #undef DEFAULT_MODE
    #endif
    #define DEFAULT_MODE 1  // Just set hardcoded here.
    //#warning "DEFAULT_MODE supports only Touch Mode. Please update/check your configuration."
  #endif
#endif

#ifndef MARLIN_TITLE
  #define MARLIN_TITLE "Marlin Mode"
#endif

#if HOTEND_COUNT < 0 || HOTEND_COUNT > MAX_HOTEND_COUNT
  #error "HOTEND_COUNT cannot be less than 0 or greater than 6"
#endif

#if EXTRUDER_COUNT < 0 || EXTRUDER_COUNT > MAX_EXT_COUNT
  #error "EXTRUDER_COUNT cannot be less than 0 or greater than 6"
#endif

#if FAN_COUNT < 1 || FAN_COUNT > MAX_FAN_COUNT
  #error "FAN_COUNT cannot be less than 1 or greater than 6"
#endif

#if FIL_RUNOUT < 0 || FIL_RUNOUT > 3
  #error "FIL_RUNOUT cannot be less than 0 or greater than 3"
#endif

#ifdef KNOB_LED_COLOR_PIN
  #ifdef KNOB_LED_COLOR
    #if KNOB_LED_COLOR < 0
      #error "KNOB_LED_COLOR cannot be less than 1"
    #endif

    #if KNOB_LED_COLOR > 8
      #error "KNOB_LED_COLOR cannot be greater than 9"
    #endif
  #else
    #define KNOB_LED_COLOR 0
  #endif
#else
  #ifdef KNOB_LED_COLOR
    #if KNOB_LED_COLOR > 0
      //#warning "KNOB_LED_COLOR is not supported on this target and must be set to 0"
      #undef KNOB_LED_COLOR
    #endif
  #endif
  #define KNOB_LED_COLOR 0
#endif

#ifdef CUSTOM_LABEL_0
  #define ENABLE_CUSTOM_0 1
#else
  #define ENABLE_CUSTOM_0 0
  #define CUSTOM_LABEL_0  ""
  #define CUSTOM_GCODE_0  ""
#endif

#ifdef CUSTOM_LABEL_1
  #define ENABLE_CUSTOM_1 1
#else
  #define ENABLE_CUSTOM_1 0
  #define CUSTOM_LABEL_1  ""
  #define CUSTOM_GCODE_1  ""
#endif

#ifdef CUSTOM_LABEL_2
  #define ENABLE_CUSTOM_2 1
#else
  #define ENABLE_CUSTOM_2 0
  #define CUSTOM_LABEL_2  ""
  #define CUSTOM_GCODE_2  ""
#endif

#ifdef CUSTOM_LABEL_3
  #define ENABLE_CUSTOM_3 1
#else
  #define ENABLE_CUSTOM_3 0
  #define CUSTOM_LABEL_3  ""
  #define CUSTOM_GCODE_3  ""
#endif

#ifdef CUSTOM_LABEL_4
  #define ENABLE_CUSTOM_4 1
#else
  #define ENABLE_CUSTOM_4 0
  #define CUSTOM_LABEL_4  ""
  #define CUSTOM_GCODE_4  ""
#endif

#ifdef CUSTOM_LABEL_5
  #define ENABLE_CUSTOM_5 1
#else
  #define ENABLE_CUSTOM_5 0
  #define CUSTOM_LABEL_5  ""
  #define CUSTOM_GCODE_5  ""
#endif

#ifdef CUSTOM_LABEL_6
  #define ENABLE_CUSTOM_6 1
#else
  #define ENABLE_CUSTOM_6 0
  #define CUSTOM_LABEL_6  ""
  #define CUSTOM_GCODE_6  ""
#endif

#ifdef CUSTOM_LABEL_7
  #define ENABLE_CUSTOM_7 1
#else
  #define ENABLE_CUSTOM_7 0
  #define CUSTOM_LABEL_7  ""
  #define CUSTOM_GCODE_7  ""
#endif

#ifdef CUSTOM_LABEL_8
  #define ENABLE_CUSTOM_8 1
#else
  #define ENABLE_CUSTOM_8 0
  #define CUSTOM_LABEL_8  ""
  #define CUSTOM_GCODE_8  ""
#endif

#ifdef CUSTOM_LABEL_9
  #define ENABLE_CUSTOM_9 1
#else
  #define ENABLE_CUSTOM_9 0
  #define CUSTOM_LABEL_9  ""
  #define CUSTOM_GCODE_9  ""
#endif

#ifdef CUSTOM_LABEL_10
  #define ENABLE_CUSTOM_10 1
#else
  #define ENABLE_CUSTOM_10 0
  #define CUSTOM_LABEL_10  ""
  #define CUSTOM_GCODE_10  ""
#endif

#ifdef CUSTOM_LABEL_11
  #define ENABLE_CUSTOM_11 1
#else
  #define ENABLE_CUSTOM_11 0
  #define CUSTOM_LABEL_11  ""
  #define CUSTOM_GCODE_11  ""
#endif

#ifdef CUSTOM_LABEL_12
  #define ENABLE_CUSTOM_12 1
#else
  #define ENABLE_CUSTOM_12 0
  #define CUSTOM_LABEL_12  ""
  #define CUSTOM_GCODE_12  ""
#endif

#ifdef CUSTOM_LABEL_13
  #define ENABLE_CUSTOM_13 1
#else
  #define ENABLE_CUSTOM_13 0
  #define CUSTOM_LABEL_13  ""
  #define CUSTOM_GCODE_13  ""
#endif

#ifdef CUSTOM_LABEL_14
  #define ENABLE_CUSTOM_14 1
#else
  #define ENABLE_CUSTOM_14 0
  #define CUSTOM_LABEL_14  ""
  #define CUSTOM_GCODE_14  ""
#endif

#define CUSTOM_GCODE_ENABLED {ENABLE_CUSTOM_0,  ENABLE_CUSTOM_1,  ENABLE_CUSTOM_2,  ENABLE_CUSTOM_3,  ENABLE_CUSTOM_4,\
                              ENABLE_CUSTOM_5,  ENABLE_CUSTOM_6,  ENABLE_CUSTOM_7,  ENABLE_CUSTOM_8,  ENABLE_CUSTOM_9,\
                              ENABLE_CUSTOM_10, ENABLE_CUSTOM_11, ENABLE_CUSTOM_12, ENABLE_CUSTOM_13, ENABLE_CUSTOM_14}

#define CUSTOM_GCODE_LIST    {CUSTOM_GCODE_0,  CUSTOM_GCODE_1,  CUSTOM_GCODE_2,  CUSTOM_GCODE_3,  CUSTOM_GCODE_4,\
                              CUSTOM_GCODE_5,  CUSTOM_GCODE_6,  CUSTOM_GCODE_7,  CUSTOM_GCODE_8,  CUSTOM_GCODE_9,\
                              CUSTOM_GCODE_10, CUSTOM_GCODE_11, CUSTOM_GCODE_12, CUSTOM_GCODE_13, CUSTOM_GCODE_14}

#define CUSTOM_GCODE_LABELS  {CUSTOM_LABEL_0,  CUSTOM_LABEL_1,  CUSTOM_LABEL_2,  CUSTOM_LABEL_3,  CUSTOM_LABEL_4,\
                              CUSTOM_LABEL_5,  CUSTOM_LABEL_6,  CUSTOM_LABEL_7,  CUSTOM_LABEL_8,  CUSTOM_LABEL_9,\
                              CUSTOM_LABEL_10, CUSTOM_LABEL_11, CUSTOM_LABEL_12, CUSTOM_LABEL_13, CUSTOM_LABEL_14}

//====================================================================================================
//============================ Settings Configurable At Compile Time Only ============================
//====================================================================================================

#if defined(TFT35_V2_0) || defined(TFT35_V3_0) || defined(TFT35_B1_V3_0) || defined(TFT35_E3_V3_0)
  #ifdef SCREEN_SHOT_TO_SD
    #error "WARNING! This feature requires that pin 39 on the 40 pins TFT connector is switched from GND to 3.3V. Otherwise, the color read out is incorrect"
  #endif
#endif

#ifdef MESH_GRID_MAX_POINTS_X
  #if MESH_GRID_MAX_POINTS_X > 15
    #error "MESH_GRID_MAX_POINTS_X cannot be greater than 15"
  #endif

  #if MESH_GRID_MAX_POINTS_X < 1
    #error "MESH_GRID_MAX_POINTS_X cannot be less than 1"
  #endif
#else
  #define MESH_GRID_MAX_POINTS_X 10
#endif

#ifdef MESH_GRID_MAX_POINTS_Y
  #if MESH_GRID_MAX_POINTS_Y > 15
    #error "MESH_GRID_MAX_POINTS_Y cannot be greater than 15"
  #endif

  #if MESH_GRID_MAX_POINTS_Y < 1
    #error "MESH_GRID_MAX_POINTS_Y cannot be less than 1"
  #endif
#else
  #define MESH_GRID_MAX_POINTS_Y 10
#endif

#ifdef BED_LEVELING_TYPE
  #if BED_LEVELING_TYPE > 5
    #error "BED_LEVELING_TYPE cannot be greater than 5"
  #endif

  #if BED_LEVELING_TYPE < 0
    #error "BED_LEVELING_TYPE cannot be less than 0"
  #endif
#else
  #define BED_LEVELING_TYPE 1
#endif

#ifdef DELTA_PROBE_TYPE
  #if DELTA_PROBE_TYPE > 2
    #error "DELTA_PROBE_TYPE cannot be greater than 2"
  #endif

  #if DELTA_PROBE_TYPE < 0
    #error "DELTA_PROBE_TYPE cannot be less than 0"
  #endif
#else
  #define DELTA_PROBE_TYPE 0
#endif

#ifdef KEYBOARD_COLOR_LAYOUT
  #if KEYBOARD_COLOR_LAYOUT > 2
    #error "KEYBOARD_COLOR_LAYOUT cannot be greater than 2"
  #endif

  #if KEYBOARD_COLOR_LAYOUT < 0
    #error "KEYBOARD_COLOR_LAYOUT cannot be less than 0"
  #endif
#else
  #define KEYBOARD_COLOR_LAYOUT 0
#endif

#if KEYBOARD_COLOR_LAYOUT == 2 && defined(KEYBOARD_MATERIAL_THEME)
  #undef KEYBOARD_MATERIAL_THEME
#endif

#ifdef TERMINAL_KEYBOARD_LAYOUT
  #if TERMINAL_KEYBOARD_LAYOUT > 2
    #error "TERMINAL_KEYBOARD_LAYOUT cannot be greater than 2"
  #endif

  #if TERMINAL_KEYBOARD_LAYOUT < 0
    #error "TERMINAL_KEYBOARD_LAYOUT cannot be less than 0"
  #endif
#else
  #define TERMINAL_KEYBOARD_LAYOUT 0
#endif

#ifdef PROGRESS_BAR_COLOR
  #if PROGRESS_BAR_COLOR > 9
    #error "PROGRESS_BAR_COLOR cannot be greater than 9"
  #endif

  #if PROGRESS_BAR_COLOR < 0
    #error "PROGRESS_BAR_COLOR cannot be less than 0"
  #endif
#else
  #define PROGRESS_BAR_COLOR 0
#endif

#ifdef LIVE_TEXT_BG_COLOR_PRINTING
  #if LIVE_TEXT_BG_COLOR_PRINTING > 2
    #error "LIVE_TEXT_BG_COLOR_PRINTING cannot be greater than 2"
  #endif

  #if LIVE_TEXT_BG_COLOR_PRINTING < 0
    #error "LIVE_TEXT_BG_COLOR_PRINTING cannot be less than 0"
  #endif
#else
  #define LIVE_TEXT_BG_COLOR_PRINTING 0
#endif

#ifdef LIVE_TEXT_BG_COLOR_STATUS
  #if LIVE_TEXT_BG_COLOR_STATUS > 6
    #error "LIVE_TEXT_BG_COLOR_STATUS cannot be greater than 6"
  #endif

  #if LIVE_TEXT_BG_COLOR_STATUS < 0
    #error "LIVE_TEXT_BG_COLOR_STATUS cannot be less than 0"
  #endif
#else
  #define LIVE_TEXT_BG_COLOR_STATUS 0
#endif

#if THUMBNAIL_PARSER == PARSER_BASE64PNG
  #if RAM_SIZE < 96
    // Decoding Base64-encoded PNGs is not possible due to memory requirements. Downgrading to the "RGB565 bitmap" option.
    #undef THUMBNAIL_PARSER
    #define THUMBNAIL_PARSER PARSER_RGB565
  #endif
#endif

#ifdef THUMBNAIL_PARSER
  #if (THUMBNAIL_PARSER != PARSER_CLASSIC) && (THUMBNAIL_PARSER != PARSER_RGB565) && (THUMBNAIL_PARSER != PARSER_BASE64PNG)
    #error "Configuration error: THUMBNAIL_PARSER is not set to a valid value of 0, 1 or 2."
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif  // _SANITY_CHECK_H_
