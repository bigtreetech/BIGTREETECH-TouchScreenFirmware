#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"
#include "coordinate.h"  // for TOTAL_AXIS
#include "LED_Colors.h"  // for LED_COLOR_COMPONENT_COUNT

// Config version support
// change if new elements/keywords are added/removed/changed in the configuration.h Format YYYYMMDD
// this number should match CONFIG_VERSION in configuration.h
#define CONFIG_SUPPPORT 20220518

#define FONT_FLASH_SIGN       20210522  // (YYYYMMDD) change if fonts require updating
#define CONFIG_FLASH_SIGN     20220518  // (YYYYMMDD) change if any keyword(s) in config.ini is added or removed
#define LANGUAGE_FLASH_SIGN   20220712  // (YYYYMMDD) change if any keyword(s) in language pack is added or removed
#define ICON_FLASH_SIGN       20220712  // (YYYYMMDD) change if any icon(s) is added or removed

#define FONT_CHECK_SIGN       (FONT_FLASH_SIGN + WORD_UNICODE_ADDR + FLASH_SIGN_ADDR)
#define CONFIG_CHECK_SIGN     (CONFIG_FLASH_SIGN + STRINGS_STORE_ADDR + \
                               sizeof(SETTINGS) + sizeof(STRINGS_STORE) + sizeof(PREHEAT_STORE) + \
                               sizeof(CUSTOM_GCODES) + sizeof(PRINT_GCODES))
#define LANGUAGE_CHECK_SIGN   (LANGUAGE_FLASH_SIGN + LANGUAGE_ADDR + LABEL_NUM)
#define ICON_CHECK_SIGN       (ICON_FLASH_SIGN + ICON_ADDR(0) + ICON_PREVIEW)

#define MAX_SERIAL_PORT_COUNT 4
#define MAX_EXT_COUNT         6
#define MAX_HOTEND_COUNT      6
#define MAX_HEATER_COUNT      (MAX_HOTEND_COUNT + 2)  // hotends + bed + chamber
#define MAX_HEATER_PID_COUNT  (MAX_HOTEND_COUNT + 1)  // hotends + bed
#define MAX_COOLING_FAN_COUNT 6
#define MAX_CRTL_FAN_COUNT    2
#define MAX_FAN_COUNT         (MAX_COOLING_FAN_COUNT + MAX_CRTL_FAN_COUNT)

#define AXIS_NUM              (TOTAL_AXIS - 1)
#define SPEED_COUNT            3
#define PREHEAT_COUNT          6
#define CUSTOM_GCODES_COUNT   15
#define MAX_STRING_LENGTH     75
#define MAX_LABEL_LENGTH       7
#define MAX_GCODE_NAME_LENGTH 75
#define MAX_GCODE_LENGTH      75

#define MIN_STRING_LENGTH      3
#define MIN_LABEL_LENGTH       3
#define MIN_GCODE_LENGTH       3

#define DISABLED  0
#define ENABLED   1
#define AUTO      2
#define HIGH      1
#define LOW       0

enum
{
  font_sign,
  config_sign,
  lang_sign,
  icon_sign,
  sign_count
};

// General Settings

typedef enum
{
  INDEX_LISTENING_MODE = 0,
  INDEX_EMULATED_M600,
  INDEX_EMULATED_M109_M190,
  INDEX_EVENT_LED,
  INDEX_FILE_COMMENT_PARSING
} GENERAL_SETTINGS;

// UI Settings

typedef enum
{
  SORT_DATE_NEW_FIRST = 0,
  SORT_DATE_OLD_FIRST,
  SORT_NAME_ASCENDING,
  SORT_NAME_DESCENDING,
  SORT_BY_COUNT
} SORT_BY;

typedef enum
{
  PERCENTAGE_ELAPSED = 0,
  PERCENTAGE_REMAINING,
  ELAPSED_REMAINING
} PROGRESS_DISPLAY;

typedef enum
{
  SHOW_LAYER_HEIGHT,
  CLEAN_LAYER_HEIGHT,
  SHOW_LAYER_NUMBER,
  CLEAN_LAYER_NUMBER,
  SHOW_LAYER_BOTH,
  CLEAN_LAYER_BOTH
} LAYER_TYPE;

// Marlin Mode Settings (only for TFT24 V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)

typedef enum
{
  MODE_MARLIN = 0,
  MODE_SERIAL_TSC,
  MODE_COUNT,               // number of different modes
  MODE_BLOCKED_MARLIN = 2,
  MODE_BLOCKED_SERIAL_TSC,
  MAX_MODE_COUNT            // total number of modes
} LCD_MODE;

typedef enum
{
  LCD12864 = 0,
  LCD2004,
  MODE_TYPE_COUNT
} MARLIN_MODE_TYPE;

// Printer / Machine Settings

typedef enum
{
  FEEDRATE_XY = 0,
  FEEDRATE_Z,
  FEEDRATE_E,
  FEEDRATE_COUNT
} FEEDRATE_INDEX;

// Filament Runout Settings (only if connected to TFT controller)

typedef enum
{
  RUNOUT_ENABLED = 0,
  RUNOUT_SENSOR_TYPE,
  RUNOUT_INVERTED,
  RUNOUT_NO_NC
} RUNOUT_SETTINGS;

// Other Device-Specific Settings

typedef enum
{
  SOUND_TYPE_TOUCH = 0,
  SOUND_TYPE_TOAST,
  SOUND_TYPE_ALERT,
  SOUND_TYPE_HEATER,
  SOUND_TYPE_COUNT
} SOUND_TYPE;

// Start, End & Cancel G-code Commands

typedef enum
{
  SEND_GCODES_START_PRINT = 0,
  SEND_GCODES_END_PRINT,
  SEND_GCODES_CANCEL_PRINT,
  SEND_GCODES_COUNT
} SEND_GCODES_TYPE;

typedef struct
{
  // General Settings
  uint8_t  serial_port[MAX_SERIAL_PORT_COUNT];
  uint8_t  general_settings;  // emulated M600 / emulated M109-M190 / file comment parsing toggles (Bit Values)

  // UI Settings
  uint8_t  rotated_ui;
  uint8_t  language;
  uint8_t  status_screen;
  uint16_t title_bg_color;
  uint16_t bg_color;
  uint16_t font_color;
  uint16_t reminder_color;
  uint16_t status_color;
  uint16_t status_xyz_bg_color;
  uint16_t list_border_color;
  uint16_t list_button_bg_color;
  uint16_t mesh_min_color;
  uint16_t mesh_max_color;
  uint8_t  terminal_color_scheme;

  uint8_t  ack_notification;
  uint8_t  files_sort_by;
  uint8_t  files_list_mode;
  uint8_t  filename_extension;
  uint8_t  fan_percentage;
  uint8_t  persistent_info;
  uint8_t  terminal_ack;
  uint8_t  notification_m117;
  uint8_t  prog_source;
  uint8_t  prog_disp_type;
  uint8_t  layer_disp_type;

  // Marlin Mode Settings (only for TFT24 V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
  uint8_t  mode;
  uint8_t  serial_always_on;
  uint16_t marlin_bg_color;
  uint16_t marlin_font_color;
  uint8_t  marlin_fullscreen;
  uint8_t  marlin_show_title;
  uint8_t  marlin_type;

  // Printer / Machine Settings
  uint8_t  hotend_count;
  uint8_t  bed_en;
  uint8_t  chamber_en;
  uint8_t  ext_count;
  uint8_t  fan_count;
  uint8_t  ctrl_fan_en;
  uint16_t max_temp[MAX_HEATER_COUNT];  // hotends + bed + chamber
  uint16_t min_ext_temp;
  uint8_t  fan_max[MAX_FAN_COUNT];
  int16_t  machine_size_min[AXIS_NUM];  // X, Y, Z
  int16_t  machine_size_max[AXIS_NUM];  // X, Y, Z
  uint16_t xy_speed[SPEED_COUNT];
  uint16_t z_speed[SPEED_COUNT];
  uint16_t ext_speed[SPEED_COUNT];
  uint8_t  auto_load_leveling;
  uint8_t  onboard_sd;
  uint8_t  m27_refresh_time;
  uint8_t  m27_active;
  uint8_t  long_filename;
  float    pause_retract_len;
  float    resume_purge_len;
  float    pause_pos[AXIS_NUM - 1];  // X, Y
  float    pause_z_raise;
  uint16_t pause_feedrate[FEEDRATE_COUNT];  // XY, Z, E
  uint8_t  level_edge;
  float    level_z_pos;
  float    level_z_raise;
  uint16_t level_feedrate[FEEDRATE_COUNT - 1];  // XY, Z

  uint8_t  move_speed;  // index on infoSettings.axis_speed, infoSettings.ext_speed

  uint8_t  inverted_axis;  // invert X Y Z axis and leveling Y axis (Bit Values)
  uint8_t  probing_z_offset;
  float    probing_z_raise;
  uint8_t  z_steppers_alignment;
  uint8_t  touchmi_sensor;

  // Power Supply Settings (only if connected to TFT controller)
  uint8_t  ps_active_high;
  uint8_t  auto_shutdown;
  uint8_t  auto_shutdown_temp;

  // Filament Runout Settings (only if connected to TFT controller)
  uint8_t  runout;
  uint16_t runout_noise;
  uint8_t  runout_distance;

  // Power Loss Recovery & BTT UPS Settings
  uint8_t  plr;
  uint8_t  plr_home;
  float    plr_z_raise;
  uint8_t  btt_ups;

  // Other Device-Specific Settings
  uint8_t  sounds;  // sound alert toggles (Bit Values)
  uint8_t  lcd_brightness;
  uint8_t  lcd_idle_brightness;
  uint8_t  lcd_idle_time;
  uint8_t  lcd_lock_on_idle;
  uint8_t  led_color[LED_COLOR_COMPONENT_COUNT];
  uint8_t  led_always_on;
  uint8_t  knob_led_color;
  uint8_t  knob_led_idle;
  uint8_t  neopixel_pixels;

  // Start, End & Cancel G-code Commands
  uint8_t  send_gcodes;  // send printing gcodes toggles (Bit Values)
} SETTINGS;

typedef struct
{
  char marlin_title[MAX_STRING_LENGTH + 1];
} STRINGS_STORE;

typedef struct
{
  char preheat_name[PREHEAT_COUNT][MAX_STRING_LENGTH + 1];
  uint16_t preheat_temp[PREHEAT_COUNT];
  uint16_t preheat_bed[PREHEAT_COUNT];
} PREHEAT_STORE;

typedef struct
{
  uint8_t count;
  char name[CUSTOM_GCODES_COUNT][MAX_GCODE_NAME_LENGTH + 1];
  char gcode[CUSTOM_GCODES_COUNT][MAX_GCODE_LENGTH + 1];
} CUSTOM_GCODES;

typedef struct
{
  char start_gcode[MAX_GCODE_LENGTH + 1];
  char end_gcode[MAX_GCODE_LENGTH + 1];
  char cancel_gcode[MAX_GCODE_LENGTH + 1];
} PRINT_GCODES;

/**
 * Firmware type
 */
typedef enum
{
  FW_NOT_DETECTED,
  FW_MARLIN,
  FW_REPRAPFW,
  FW_KLIPPER,
  FW_SMOOTHIEWARE,
  FW_UNKNOWN,
} FW_TYPE;

/**
 * Bed Leveling type
 */
typedef enum
{
  BL_DISABLED = DISABLED,  // Bed Leveling Diabled
  BL_ABL,                  // Auto Bed Leveling (ABL)
  BL_BBL,                  // Bilinear Bed Leveling (BBL)
  BL_UBL,                  // Unified Bed Leveling (UBL)
  BL_MBL,                  // Mesh Bed Leveling (MBL)
} BL_TYPE;

typedef struct
{
  FW_TYPE firmwareType;
  uint8_t EEPROM;
  uint8_t autoReportTemp;
  uint8_t autoReportPos;
  BL_TYPE leveling;
  uint8_t zProbe;
  uint8_t levelingData;
  uint8_t softwarePower;
  uint8_t toggleLights;
  uint8_t caseLightsBrightness;
  uint8_t emergencyParser;
  uint8_t promptSupport;
  uint8_t onboardSD;
  uint8_t multiVolume;
  uint8_t autoReportSDStatus;
  uint8_t longFilename;
  uint8_t babyStepping;
  uint8_t buildPercent;
  uint8_t softwareEndstops;
} MACHINE_SETTINGS;

extern SETTINGS infoSettings;
extern MACHINE_SETTINGS infoMachineSettings;

extern const uint16_t default_max_temp[];
extern const uint16_t default_max_fanPWM[];
extern const uint16_t default_size_min[];
extern const uint16_t default_size_max[];
extern const uint16_t default_move_speed[];
extern const uint16_t default_ext_speed[];
extern const uint16_t default_level_speed[];
extern const uint16_t default_pause_speed[];
extern const uint16_t default_preheat_ext[];
extern const uint16_t default_preheat_bed[];
extern const uint8_t default_custom_enabled[];

void initSettings(void);
void initMachineSettings(void);
void setupMachine(FW_TYPE fwType);
float flashUsedPercentage(void);
void checkflashSign(void);
bool getFlashSignStatus(int index);

#ifdef __cplusplus
}
#endif

#endif
