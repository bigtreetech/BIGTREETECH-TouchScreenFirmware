#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "ff.h"

//after changing/adding/removing a keyword, change the CONFIG_FLASH_SIGN in Settings.h and PARA_SIGN in flashStore.c

#define LINE_MAX_CHAR                 100
#define CONFIG_FILE_PATH              "0:config.ini"
#define LANG_FILE_PATH                "0:language.ini"

//-----------------------------General Settings
#define CONFIG_STATUS_SCREEN          "status_screen:"
#define CONFIG_UART_BAUDRATE          "baudrate:"
#define CONFIG_LANGUAGE               "language:"

#define CONFIG_TITLE_BG               "title_back_color:"
#define CONFIG_MENU_BG_COLOR          "background_color:"
#define CONFIG_MENU_FONT_COLOR        "font_color:"
#define CONFIG_NOTIFY_COLOR           "reminder_color:"
#define CONFIG_SD_NOTIFY_COLOR        "volume_status_color:"
#define CONFIG_SS_XYZ_BG_COLOR        "status_xyz_bg_color:"
#define CONFIG_LIST_BORDER_COLOR      "list_border_color:"
#define CONFIG_LIST_BUTTON_BG_COLOR   "list_button_bg_color:"
#define CONFIG_MESH_MIN_COLOR         "mesh_min_color:"
#define CONFIG_MESH_MAX_COLOR         "mesh_max_color:"
#define CONFIG_TERMINAL_COLOR_SCHEME  "terminal_color_scheme:"

#define CONFIG_ROTATE_UI              "rotate_ui:"
#define CONFIG_TERMINAL_ACK           "terminal_ack:"
#define CONFIG_INVERT_AXIS            "invert_axis:"
#define CONFIG_PERSISTENT_TEMP        "persistent_info:"
#define CONFIG_LIST_MODE              "files_list_mode:"
#define CONFIG_ACK_NOTIFICATION       "ack_notification:"
#define CONFIG_NOTIFICATION_M117      "notification_m117:"
#define CONFIG_EMULATE_M600           "emulate_m600:"
//-----------------------------Marlin Mode Settings (only for TFT24_V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70_V3.0)
#define CONFIG_MODE                   "default_mode:"
#define CONFIG_SERIAL_ON              "serial_always_on:"
#define CONFIG_MARLIN_BG_COLOR        "marlin_bg_color:"
#define CONFIG_MARLIN_FONT_COLOR      "marlin_fn_color:"
#define CONFIG_MARLIN_FULLSCREEN      "marlin_fullscreen:"
#define CONFIG_MARLIN_SHOW_TITLE      "marlin_show_title:"
#define CONFIG_MARLIN_TITLE           "marlin_title:"
#define CONFIG_MARLIN_TYPE            "marlin_type:"
//-----------------------------RRF Mode Settings
#define CONFIG_RRF_MACROS_ON          "rrf_macros_on:"
//-----------------------------Printer / Machine Settings
#define CONFIG_HOTEND_COUNT           "hotend_count:"
#define CONFIG_HEATED_BED             "heated_bed:"
#define CONFIG_HEATED_CHAMBER         "heated_chamber:"
#define CONFIG_EXT_COUNT              "ext_count:"
#define CONFIG_FAN_COUNT              "fan_count:"
#define CONFIG_FAN_CTRL_COUNT         "fan_ctrl_count:"
#define CONFIG_MAX_TEMP               "max_temp:"
#define CONFIG_MIN_TEMP               "min_temp:"
#define CONFIG_FAN_MAX                "fan_max:"
#define CONFIG_BUILD_MIN              "size_min:"
#define CONFIG_BUILD_MAX              "size_max:"
#define CONFIG_XY_SPEED               "xy_speed:"
#define CONFIG_Z_SPEED                "z_speed:"
#define CONFIG_EXT_SPEED              "ext_speed:"
#define CONFIG_AUTO_LEVEL             "auto_load_leveling:"
#define CONFIG_TOUCHMI_SENSOR         "touchmi_sensor:"
#define CONFIG_ONBOARD_SD             "onboard_sd_support:"
#define CONFIG_M27_DELAY              "M27_refresh_time:"
#define CONFIG_M27_KEEP_ON            "M27_always_active:"
#define CONFIG_LONG_FILENAME          "long_filename_support:"
#define CONFIG_FAN_PERCENT            "fan_speed_percent:"
#define CONFIG_PAUSE_RETRACT          "pause_retract:"
#define CONFIG_PAUSE_POS              "pause_pos:"
#define CONFIG_PAUSE_Z_RAISE          "pause_z_raise:"
#define CONFIG_PAUSE_FEEDRATE         "pause_feedrate:"
#define CONFIG_LEVEL_EDGE             "level_edge_distance:"
#define CONFIG_LEVEL_Z_POS            "level_z_pos:"
#define CONFIG_LEVEL_Z_RAISE          "level_z_raise:"
#define CONFIG_LEVEL_FEEDRATE         "level_feedrate:"
#define CONFIG_XY_OFFSET_PROBING      "xy_offset_probing:"
#define CONFIG_Z_RAISE_PROBING        "z_raise_probing:"
#define CONFIG_Z_STEPPERS_ALIGNMENT   "z_steppers_alignment:"
#define CONFIG_PREHEAT_NAME_1         "preheat_name1:"
#define CONFIG_PREHEAT_NAME_2         "preheat_name2:"
#define CONFIG_PREHEAT_NAME_3         "preheat_name3:"
#define CONFIG_PREHEAT_NAME_4         "preheat_name4:"
#define CONFIG_PREHEAT_NAME_5         "preheat_name5:"
#define CONFIG_PREHEAT_NAME_6         "preheat_name6:"
#define CONFIG_PREHEAT_TEMP_1         "preheat_temp1:"
#define CONFIG_PREHEAT_TEMP_2         "preheat_temp2:"
#define CONFIG_PREHEAT_TEMP_3         "preheat_temp3:"
#define CONFIG_PREHEAT_TEMP_4         "preheat_temp4:"
#define CONFIG_PREHEAT_TEMP_5         "preheat_temp5:"
#define CONFIG_PREHEAT_TEMP_6         "preheat_temp6:"
//-----------------------------Power Supply Settings (if connected to TFT controller)
#define CONFIG_PS_ON                  "ps_on:"
#define CONFIG_PS_LOGIC               "ps_on_active_high:"
#define CONFIG_SHUTDOWN_TEMP          "auto_shutdown_temp:"
//-----------------------------Filament Runout Settings (if connected to TFT controller)
#define CONFIG_RUNOUT                 "fil_runout:"
#define CONFIG_RUNOUT_LOGIC           "fil_runout_inverting:"
#define CONFIG_RUNOUT_NOISE           "fil_noise_threshold:"
#define CONFIG_RUNOUT_DISTANCE        "fil_runout_distance:"
//-----------------------------Power Loss Recovery & BTT UPS Settings (if connected to TFT controller)
#define CONFIG_POWERLOSS_EN           "pl_recovery_en:"
#define CONFIG_POWERLOSS_HOME         "pl_recovery_home:"
#define CONFIG_POWERLOSS_ZRAISE       "pl_z_raise:"
#define CONFIG_BTT_MINIUPS            "btt_mini_ups:"
//-----------------------------Other Device-Specific Settings
#define CONFIG_TOUCH_SOUND            "touch_sound:"
#define CONFIG_TOAST_SOUND            "toast_sound:"
#define CONFIG_ALERT_SOUND            "alert_sound:"
#define CONFIG_HEATER_SOUND           "heater_sound:"
#define CONFIG_KNOB_COLOR             "knob_led_color:"
#define CONFIG_KNOB_LED_IDLE          "knob_led_idle:"
#define CONFIG_NEOPIXEL_PIXELS        "neopixel_pixels:"
#define CONFIG_BRIGHTNESS             "lcd_brightness:"
#define CONFIG_BRIGHTNESS_IDLE        "lcd_idle_brightness:"
#define CONFIG_BRIGHTNESS_IDLE_DELAY  "lcd_idle_delay:"
//-----------------------------Custom Gcode Commands
#define CONFIG_CUSTOM_LABEL_1         "custom_label_1:"
#define CONFIG_CUSTOM_LABEL_2         "custom_label_2:"
#define CONFIG_CUSTOM_LABEL_3         "custom_label_3:"
#define CONFIG_CUSTOM_LABEL_4         "custom_label_4:"
#define CONFIG_CUSTOM_LABEL_5         "custom_label_5:"
#define CONFIG_CUSTOM_LABEL_6         "custom_label_6:"
#define CONFIG_CUSTOM_LABEL_7         "custom_label_7:"
#define CONFIG_CUSTOM_LABEL_8         "custom_label_8:"
#define CONFIG_CUSTOM_LABEL_9         "custom_label_9:"
#define CONFIG_CUSTOM_LABEL_10        "custom_label_10:"
#define CONFIG_CUSTOM_LABEL_11        "custom_label_11:"
#define CONFIG_CUSTOM_LABEL_12        "custom_label_12:"
#define CONFIG_CUSTOM_LABEL_13        "custom_label_13:"
#define CONFIG_CUSTOM_LABEL_14        "custom_label_14:"
#define CONFIG_CUSTOM_LABEL_15        "custom_label_15:"
#define CONFIG_CUSTOM_GCODE_1         "custom_gcode_1:"
#define CONFIG_CUSTOM_GCODE_2         "custom_gcode_2:"
#define CONFIG_CUSTOM_GCODE_3         "custom_gcode_3:"
#define CONFIG_CUSTOM_GCODE_4         "custom_gcode_4:"
#define CONFIG_CUSTOM_GCODE_5         "custom_gcode_5:"
#define CONFIG_CUSTOM_GCODE_6         "custom_gcode_6:"
#define CONFIG_CUSTOM_GCODE_7         "custom_gcode_7:"
#define CONFIG_CUSTOM_GCODE_8         "custom_gcode_8:"
#define CONFIG_CUSTOM_GCODE_9         "custom_gcode_9:"
#define CONFIG_CUSTOM_GCODE_10        "custom_gcode_10:"
#define CONFIG_CUSTOM_GCODE_11        "custom_gcode_11:"
#define CONFIG_CUSTOM_GCODE_12        "custom_gcode_12:"
#define CONFIG_CUSTOM_GCODE_13        "custom_gcode_13:"
#define CONFIG_CUSTOM_GCODE_14        "custom_gcode_14:"
#define CONFIG_CUSTOM_GCODE_15        "custom_gcode_15:"
//-----------------------------Start, End & Cancel Gcode Commands
#define CONFIG_START_GCODE_ON         "start_gcode_enabled:"
#define CONFIG_END_GCODE_ON           "end_gcode_enabled:"
#define CONFIG_CANCEL_GCODE_ON        "cancel_gcode_enabled:"
#define CONFIG_START_GCODE            "start_gcode:"
#define CONFIG_END_GCODE              "end_gcode:"
#define CONFIG_CANCEL_GCODE           "cancel_gcode:"

//-----------------------------Limits
#define MAX_SIZE_LIMIT         2000  // machine size over this will not be parsed.
#define MAX_EXT_SPEED_LIMIT    5000  // Extruder speed over this will not pe parsed.
#define MAX_TOOL_TEMP          1000  // extruder temp over this will not pe parsed.
#define MAX_BED_TEMP            200  // bed temp over this will not pe parsed.
#define MAX_CHAMBER_TEMP        100  // bed temp over this will not pe parsed.
#define MAX_SPEED_LIMIT       12000  // speed over this will not pe parsed.
#define MAX_RUNOUT_DISTANCE      50  // runout distance over this will not pe parsed.
#define MAX_DELAY_MS           1800  // time delay (ms) over this will not pe parsed.
#define MAX_DELAY_SEC           100  // time delay (sec) over this will not pe parsed.
#define MAX_RETRACT_LIMIT        20  // retract length over this will not be parsed.
#define MAX_FAN_SPEED           255  // fan speed (PWM) over this will not be parsed.
#define MAX_NEOPIXEL_PIXELS     200  // neopixel pixels over this will not be parsed.

#define MIN_SIZE_LIMIT        -2000  // machine size less than this will not be parsed.
#define NAME_MIN_LENGTH           3  // minimum name length
#define GCODE_MIN_LENGTH          3  // gcode length less than this will not pe parsed.
#define MIN_XY_POS_LIMIT      -2000  // Set a negative minimum position for Delta printers
#define MIN_Z_POS_LIMIT           0
#define MIN_Z_RAISE_PROBING   -2000  // Set a negative minimum position for Delta printers
#define MIN_TOOL_TEMP            20  // extruder temp less than this will not pe parsed.
#define MIN_BED_TEMP             20  // bed temp less than this will not pe parsed.
#define MIN_CHAMBER_TEMP         20  // chamber temp less than this will not pe parsed.
#define MIN_RUNOUT_DISTANCE       1  // runout distance less than this will not be parsed.
#define MIN_DELAY_MS             10  // time delay (ms) less than this will not pe parsed.
#define MIN_DELAY_SEC             1  // time delay (sec) less than this will not pe parsed.
#define MIN_SPEED_LIMIT          10  // speed less than this will not pe parsed.
#define MIN_RETRACT_LIMIT         0  // retract length less than this will not be parsed.
#define MIN_FAN_SPEED            25  // fan speed (PWM) less than this will not be parsed.

typedef struct
{
  FIL file;
  uint32_t size;  // total size of config file
  uint32_t cur;   // total size of
} CONFIGFILE;

typedef enum
{
  CSTAT_FILE_NOTOPEN = 0,
  CSTAT_FILE_INVALID,
  CSTAT_UNKNOWN_KEYWORD,
  CSTAT_INVALID_VALUE,
  CSTAT_OLD_VERSION,
  CSTAT_STORAGE_LOW,
  CSTAT_SPI_WRITE_FAIL,
} CONFIG_STATS;

bool getConfigFromFile(void);
bool getLangFromFile(void);

bool readConfigFile(const char * path, void (*lineParser)(), uint16_t maxLineLen);

void parseConfigLine(void);
void parseLangLine(void);

void parseConfigKey(uint16_t index);
void writeConfig(uint8_t* dataBytes, uint16_t numBytes, uint32_t addr, uint32_t maxSize);
void saveConfig(void);
void resetConfig(void);

void drawProgressPage(uint8_t * title);
void drawProgress(void);
void showError(CONFIG_STATS stat);

//This List is Auto-Generated. Please add new icons in icon_list.inc only
enum
{
  #define X_CONFIG(NAME) C_INDEX_##NAME ,
  #include "config.inc"
  #undef  X_CONFIG

  CONFIG_COUNT,
};

#ifdef __cplusplus
}
#endif

#endif
