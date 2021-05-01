#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "coordinate.h"
#include "Configuration.h"

enum
{
  font_sign,
  config_sign,
  lang_sign,
  icon_sign,
  sign_count
};

typedef enum
{
  FEEDRATE_XY = 0,
  FEEDRATE_Z,
  FEEDRATE_E,
  FEEDRATE_COUNT,
} FEEDRATE_INDEX;

// Config version support
// change if new elements/keywords are added/removed/changed in the configuration.h Format YYYYMMDD
// this number should match CONFIG_VERSION in configuration.h
#define CONFIG_SUPPPORT 20210321

#define FONT_FLASH_SIGN       20200908  //(YYYYMMDD) change if fonts require updating
#define CONFIG_FLASH_SIGN     20210421  //(YYYYMMDD) change if any keyword(s) in config.ini is added or removed
#define LANGUAGE_FLASH_SIGN   20210417  //(YYYYMMDD) change if any keyword(s) in language pack is added or removed
#define ICON_FLASH_SIGN       20210217  //(YYYYMMDD) change if any icon(s) is added or removed

#define FONT_CHECK_SIGN       (FONT_FLASH_SIGN + WORD_UNICODE)
#define CONFIG_CHECK_SIGN     (CONFIG_FLASH_SIGN + STRINGS_STORE_ADDR)
#define LANGUAGE_CHECK_SIGN   (LANGUAGE_FLASH_SIGN + LANGUAGE_ADDR)
#define ICON_CHECK_SIGN       (ICON_FLASH_SIGN + ICON_ADDR(0))

#define MAX_EXT_COUNT         6
#define MAX_HOTEND_COUNT      6
#define MAX_HEATER_COUNT      (2 + MAX_HOTEND_COUNT)  // chamber + bed + hotend
#define MAX_FAN_CTRL_COUNT    2
#define MAX_FAN_COUNT         (6 + MAX_FAN_CTRL_COUNT)

#define AXIS_NUM              (TOTAL_AXIS - 1)
#define SPEED_COUNT           3
#define PREHEAT_COUNT         6
#define CUSTOM_GCODES_COUNT   15
#define MAX_STRING_LENGTH     20
#define MAX_LABEL_LENGTH      7
#define MAX_GCODE_NAME_LENGTH 24
#define MAX_GCODE_LENGTH      75

#define MIN_STRING_LENGTH     3
#define MIN_LABEL_LENGTH      3
#define MIN_GCODE_LENGTH      3

#define DISABLED  0
#define ENABLED   1
#define AUTO      2
#define HIGH      1
#define LOW       0

typedef enum
{
  MODE_MARLIN = 0,
  MODE_SERIAL_TSC,
  MODE_COUNT
} LCD_MODE;

typedef enum
{
  LCD12864 = 0,
  LCD2004,
} MARLIN_MODE_TYPE;

typedef struct
{
  // General Settings
  uint8_t status_screen;
  uint8_t baudrate;
  uint8_t language;

  uint16_t title_bg_color;
  uint16_t bg_color;
  uint16_t font_color;
  uint16_t reminder_color;
  uint16_t sd_reminder_color;
  uint16_t status_xyz_bg_color;
  uint16_t list_border_color;
  uint16_t list_button_color;
  uint16_t mesh_min_color;
  uint16_t mesh_max_color;
  uint8_t  terminal_color_scheme;

  uint8_t rotate_ui;
  uint8_t terminalACK;
  uint8_t invert_axis[AXIS_NUM];
  uint8_t leveling_invert_y_axis;
  uint8_t persistent_info;
  uint8_t file_listmode;
  uint8_t ack_notification;
  uint8_t notification_m117;
  uint8_t emulate_m600;

  // Marlin Mode Settings
  uint8_t  mode;
  uint8_t  serial_alwaysOn;
  uint16_t marlin_mode_bg_color;
  uint16_t marlin_mode_font_color;
  uint8_t  marlin_mode_fullscreen;
  uint8_t  marlin_mode_showtitle;
  uint8_t  marlin_type;

  // rrf mode settings
  uint8_t  rrf_macros_enable;

  // Printer / Machine Settings
  uint8_t  hotend_count;
  uint8_t  bed_en;
  uint8_t  chamber_en;
  uint8_t  ext_count;
  uint8_t  fan_count;
  uint8_t  fan_ctrl_count;
  uint16_t max_temp[MAX_HEATER_COUNT];  // chamber + bed + hotend
  uint16_t min_ext_temp;
  uint8_t  fan_max[MAX_FAN_COUNT];
  int16_t  machine_size_min[AXIS_NUM];  // X, Y, Z
  int16_t  machine_size_max[AXIS_NUM];  // X, Y, Z
  uint16_t xy_speed[SPEED_COUNT];
  uint16_t z_speed[SPEED_COUNT];
  uint16_t ext_speed[SPEED_COUNT];
  uint8_t  auto_load_leveling;
  uint8_t  touchmi_sensor;
  uint8_t  onboardSD;
  uint8_t  m27_refresh_time;
  uint8_t  m27_active;
  uint8_t  longFileName;
  uint8_t  fan_percentage;
  float    pause_retract_len;
  float    resume_purge_len;
  float    pause_pos[AXIS_NUM-1];  // X, Y
  float    pause_z_raise;
  uint16_t pause_feedrate[FEEDRATE_COUNT];  // XY, Z, E
  uint8_t  level_edge;
  float    level_z_pos;
  float    level_z_raise;

  uint8_t  move_speed;  // index on infoSettings.axis_speed, infoSettings.ext_speed

  uint8_t  xy_offset_probing;
  float    z_raise_probing;
  uint8_t  z_steppers_alignment;

  uint16_t level_feedrate[FEEDRATE_COUNT - 1];  // XY, Z
  uint16_t preheat_temp[PREHEAT_COUNT];
  uint16_t preheat_bed[PREHEAT_COUNT];

  // Power Supply Settings
  uint8_t  auto_off;
  uint8_t  ps_active_high;
  uint8_t  auto_off_temp;

  // Filament Runout Settings
  uint8_t  runout;
  uint8_t  runout_invert;
  uint16_t runout_noise_ms;
  uint8_t  runout_distance;

  // Power Loss Recovery & BTT UPS Settings
  uint8_t  powerloss_en;
  uint8_t  powerloss_home;
  uint8_t  powerloss_invert;
  uint8_t  powerloss_z_raise;
  uint8_t  btt_ups;

  // Other Device-Specific Settings
  uint8_t  touchSound;
  uint8_t  toastSound;
  uint8_t  alertSound;
  uint8_t  heaterSound;

#ifdef LED_COLOR_PIN
  uint8_t  knob_led_color;
  uint8_t  knob_led_idle;
  uint8_t  neopixel_pixels;
#endif
  uint8_t  lcd_brightness;
  uint8_t  lcd_idle_brightness;
  uint8_t  lcd_idle_timer;

  // Start, End & Cancel Gcode Commands
  uint8_t  send_start_gcode;
  uint8_t  send_end_gcode;
  uint8_t  send_cancel_gcode;
} SETTINGS;

typedef struct
{
  char marlin_title[MAX_GCODE_LENGTH + 1];
  char preheat_name[PREHEAT_COUNT][MAX_GCODE_LENGTH + 1];
} STRINGS_STORE;

typedef struct
{
  uint8_t count;
  char name[CUSTOM_GCODES_COUNT][MAX_GCODE_LENGTH + 1];
  char gcode[CUSTOM_GCODES_COUNT][MAX_GCODE_LENGTH + 1];
} CUSTOM_GCODES;

typedef struct
{
  char start_gcode[MAX_GCODE_LENGTH + 1];
  char end_gcode[MAX_GCODE_LENGTH + 1];
  char cancel_gcode[MAX_GCODE_LENGTH + 1];
} PRINT_GCODES;

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

typedef struct
{
  FW_TYPE firmwareType;
  uint8_t EEPROM;
  uint8_t autoReportTemp;
  BL_TYPE leveling;
  uint8_t zProbe;
  uint8_t levelingData;
  uint8_t softwarePower;
  uint8_t toggleLights;
  uint8_t caseLightsBrightness;
  uint8_t emergencyParser;
  uint8_t promptSupport;
  uint8_t onboard_sd_support;
  uint8_t autoReportSDStatus;
  uint8_t long_filename_support;
  uint8_t babyStepping;
  uint8_t softwareEndstops;
} MACHINESETTINGS;

extern SETTINGS infoSettings;
extern MACHINESETTINGS infoMachineSettings;

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

void initMachineSetting(void);
void infoSettingsReset(void);
void setupMachine(void);
float flashUsedPercentage(void);
void checkflashSign(void);
bool getFlashSignStatus(int index);

#ifdef __cplusplus
}
#endif

#endif
