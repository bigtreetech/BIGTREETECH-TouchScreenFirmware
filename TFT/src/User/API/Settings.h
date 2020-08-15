#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdint.h"
#include "coordinate.h"
#include "Configuration.h"

#define CONFIG_SUPPPORT 20200810

#define ITEM_BAUDRATE_NUM     9

#define MAX_EXT_COUNT         6
#define MAX_HOTEND_COUNT      6
#define MAX_HEATER_COUNT      (2 + MAX_HOTEND_COUNT) // chamber + bed + hotend
#define MAX_FAN_COUNT         6

#define AXIS_NUM              (TOTAL_AXIS - 1)
#define SPEED_COUNT           3
#define PREHEAT_COUNT         4
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

typedef enum
{
  MARLIN = 0,
  SERIAL_TSC,
  MODE_COUNT
}LCD_MODE;

typedef enum
{
  LCD2004 = 0,
  LCD12864
}LCD_MARLIN_MODE;

typedef struct
{
  uint32_t baudrate;
  uint8_t  language;
  uint8_t  mode;
  uint8_t  unified_menu;
  uint8_t  rotate_ui;

  uint16_t bg_color;
  uint16_t font_color;
  uint16_t title_bg_color;
  uint16_t reminder_color;
  uint16_t sd_reminder_color;
  uint16_t status_xyz_bg_color;
  uint16_t list_border_color;
  uint16_t list_button_color;

  uint8_t  touchSound;
  uint8_t  alertSound;
  uint8_t  toastSound;
  uint8_t  auto_off;
  uint8_t  terminalACK;
  uint8_t  invert_axis[AXIS_NUM];
  uint8_t  move_speed;
  uint8_t  knob_led_color;
  uint8_t  knob_led_idle;
  uint8_t  persistent_info;
  uint8_t  file_listmode;
  uint8_t  ack_notification;

  uint8_t  lcd_brightness;
  uint8_t  lcd_idle_brightness;
  uint8_t  lcd_idle_timer;

  uint8_t  serial_alwaysOn;
  uint8_t  marlin_type;
  uint16_t marlin_mode_bg_color;
  uint16_t marlin_mode_font_color;
  uint8_t  marlin_mode_showtitle;
  uint8_t  marlin_mode_fullscreen;

  uint8_t  send_start_gcode;
  uint8_t  send_end_gcode;
  uint8_t  send_cancel_gcode;
  uint8_t  ps_active_high;
  uint8_t  auto_off_temp;

  uint8_t  runout;
  uint8_t  runout_invert;
  uint16_t runout_noise_ms;
  uint8_t  runout_distance;

  uint8_t  powerloss_en;
  uint8_t  powerloss_home;
  uint8_t  powerloss_invert;
  uint8_t  powerloss_z_raise;
  uint8_t  btt_ups;

//machine specific settings

  uint8_t   hotend_count;
  uint8_t   bed_en;
  uint8_t   chamber_en;
  uint8_t   ext_count;
  uint8_t   fan_count;
  uint8_t   auto_load_leveling;
  uint8_t   autoLevelState;
  uint8_t   onboardSD;
  uint8_t   m27_refresh_time;
  uint8_t   m27_active;
  uint8_t   longFileName;
  uint16_t  max_temp[MAX_HEATER_COUNT];  // chamber + bed + hotend
  uint16_t  min_ext_temp;
  uint8_t   fan_max[MAX_FAN_COUNT];
  uint8_t   fan_percentage;
  int16_t   machine_size_min[AXIS_NUM];  // X, Y, Z
  int16_t   machine_size_max[AXIS_NUM];  // X, Y, Z
  uint16_t  axis_speed[SPEED_COUNT];
  uint16_t  ext_speed[SPEED_COUNT];
  float     pause_retract_len;
  float     resume_purge_len;
  float     pause_pos[AXIS_NUM-1];  // X, Y
  float     pause_z_raise;
  uint16_t  pause_feedrate[TOTAL_AXIS]; // X, Y, Z, E
  uint8_t   level_edge;
  float     level_z_pos;
  float     level_z_raise;
  uint16_t  level_feedrate[AXIS_NUM];  // X, Y, Z
  uint16_t  preheat_temp[PREHEAT_COUNT];
  uint16_t  preheat_bed[PREHEAT_COUNT];

}SETTINGS;

typedef struct{
char     marlin_title[MAX_GCODE_LENGTH+1];
char     preheat_name[PREHEAT_COUNT][MAX_GCODE_LENGTH+1];
}STRINGS_STORE;


typedef struct
{
uint8_t  count;
char     name[CUSTOM_GCODES_COUNT][MAX_GCODE_LENGTH+1];
char     gcode[CUSTOM_GCODES_COUNT][MAX_GCODE_LENGTH+1];
}CUSTOM_GCODES;

typedef struct
{
char     start_gcode[MAX_GCODE_LENGTH+1];
char     end_gcode[MAX_GCODE_LENGTH+1];
char     cancel_gcode[MAX_GCODE_LENGTH+1];
}PRINT_GCODES;


typedef struct
{
  uint8_t isMarlinFirmware;
  uint8_t EEPROM;
  uint8_t autoReportTemp;
  uint8_t autoLevel;
  uint8_t enableubl;
  uint8_t zProbe;
  uint8_t levelingData;
  uint8_t softwarePower;
  uint8_t toggleLights;
  uint8_t caseLightsBrightness;
  uint8_t emergencyParser;
  uint8_t promptSupport;
  uint8_t onboard_sd_support;
  uint8_t long_filename_support;
  uint8_t autoReportSDStatus;
  uint8_t babyStepping;
}MACHINESETTINGS;


extern SETTINGS infoSettings;
extern MACHINESETTINGS infoMachineSettings;

void initMachineSetting(void);
void infoSettingsReset(void);
void setupMachine(void);
float flashUsedPercentage(void);




#endif
