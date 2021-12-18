#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "ff.h"

// after changing/adding/removing a keyword, change the CONFIG_FLASH_SIGN in Settings.h and PARA_SIGN in FlashStore.c

#define LINE_MAX_CHAR                 200
#define CONFIG_FILE_PATH              "0:config.ini"
#define LANG_FILE_PATH                "0:language.ini"

//-----------------------------General Settings
#define CONFIG_SERIAL_PORT            "serial_port:"
#define CONFIG_EMULATED_M600          "emulated_m600:"
#define CONFIG_EMULATED_M109_M190     "emulated_m109_m190:"
#define CONFIG_EVENT_LED              "event_led:"
#define CONFIG_FILE_COMMENT_PARSING   "file_comment_parsing:"

//-----------------------------UI Settings
#define CONFIG_ROTATED_UI             "rotated_ui:"
#define CONFIG_LANGUAGE               "language:"
#define CONFIG_STATUS_SCREEN          "status_screen:"
#define CONFIG_TITLE_BG_COLOR         "title_background_color:"
#define CONFIG_MENU_BG_COLOR          "menu_background_color:"
#define CONFIG_MENU_FONT_COLOR        "menu_font_color:"
#define CONFIG_REMINDER_FONT_COLOR    "reminder_font_color:"
#define CONFIG_STATUS_FONT_COLOR      "status_font_color:"
#define CONFIG_STATUS_XYZ_BG_COLOR    "status_xyz_bg_color:"
#define CONFIG_LIST_BORDER_COLOR      "list_border_color:"
#define CONFIG_LIST_BUTTON_BG_COLOR   "list_button_bg_color:"
#define CONFIG_MESH_MIN_COLOR         "mesh_min_color:"
#define CONFIG_MESH_MAX_COLOR         "mesh_max_color:"
#define CONFIG_TERMINAL_COLOR_SCHEME  "terminal_color_scheme:"

#define CONFIG_ACK_NOTIFICATION       "ack_notification:"
#define CONFIG_FILES_SORT_BY          "files_sort_by:"
#define CONFIG_FILES_LIST_MODE        "files_list_mode:"
#define CONFIG_FILENAME_EXTENSION     "filename_extension:"
#define CONFIG_FAN_SPEED_PERCENTAGE   "fan_speed_percentage:"
#define CONFIG_PERSISTENT_INFO        "persistent_info:"
#define CONFIG_TERMINAL_ACK           "terminal_ack:"
#define CONFIG_NOTIFICATION_M117      "notification_m117:"
#define CONFIG_PROG_DISP_TYPE         "prog_disp_type:"
#define CONFIG_LAYER_DISP_TYPE        "layer_disp_type:"
//-----------------------------Marlin Mode Settings (only for TFT24 V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
#define CONFIG_MODE                   "default_mode:"
#define CONFIG_SERIAL_ALWAYS_ON       "serial_always_on:"
#define CONFIG_MARLIN_BG_COLOR        "marlin_background_color:"
#define CONFIG_MARLIN_FONT_COLOR      "marlin_font_color:"
#define CONFIG_MARLIN_FULLSCREEN      "marlin_fullscreen:"
#define CONFIG_MARLIN_SHOW_TITLE      "marlin_show_title:"
#define CONFIG_MARLIN_TITLE           "marlin_title:"
#define CONFIG_MARLIN_TYPE            "marlin_type:"
//-----------------------------Printer / Machine Settings
#define CONFIG_HOTEND_COUNT           "hotend_count:"
#define CONFIG_HEATED_BED             "heated_bed:"
#define CONFIG_HEATED_CHAMBER         "heated_chamber:"
#define CONFIG_EXT_COUNT              "ext_count:"
#define CONFIG_FAN_COUNT              "fan_count:"
#define CONFIG_CONTROLLER_FAN         "controller_fan:"
#define CONFIG_MAX_TEMP               "max_temp:"
#define CONFIG_MIN_TEMP               "min_temp:"
#define CONFIG_FAN_MAX                "fan_max:"
#define CONFIG_SIZE_MIN               "size_min:"
#define CONFIG_SIZE_MAX               "size_max:"
#define CONFIG_XY_SPEED               "xy_speed:"
#define CONFIG_Z_SPEED                "z_speed:"
#define CONFIG_EXT_SPEED              "ext_speed:"
#define CONFIG_AUTO_LOAD_LEVELING     "auto_load_leveling:"
#define CONFIG_ONBOARD_SD             "onboard_sd:"
#define CONFIG_M27_REFRESH_TIME       "M27_refresh_time:"
#define CONFIG_M27_ALWAYS_ACTIVE      "M27_always_active:"
#define CONFIG_LONG_FILENAME          "long_filename:"
#define CONFIG_PAUSE_RETRACT          "pause_retract:"
#define CONFIG_PAUSE_POS              "pause_pos:"
#define CONFIG_PAUSE_Z_RAISE          "pause_z_raise:"
#define CONFIG_PAUSE_FEEDRATE         "pause_feedrate:"
#define CONFIG_LEVEL_EDGE_DISTANCE    "level_edge_distance:"
#define CONFIG_LEVEL_Z_POS            "level_z_pos:"
#define CONFIG_LEVEL_Z_RAISE          "level_z_raise:"
#define CONFIG_LEVEL_FEEDRATE         "level_feedrate:"
#define CONFIG_INVERTED_AXIS          "inverted_axis:"
#define CONFIG_PROBING_Z_OFFSET       "probing_z_offset:"
#define CONFIG_PROBING_Z_RAISE        "probing_z_raise:"
#define CONFIG_Z_STEPPERS_ALIGNMENT   "z_steppers_alignment:"
#define CONFIG_TOUCHMI_SENSOR         "touchmi_sensor:"
#define CONFIG_PREHEAT_NAME_1         "preheat_name_1:"
#define CONFIG_PREHEAT_NAME_2         "preheat_name_2:"
#define CONFIG_PREHEAT_NAME_3         "preheat_name_3:"
#define CONFIG_PREHEAT_NAME_4         "preheat_name_4:"
#define CONFIG_PREHEAT_NAME_5         "preheat_name_5:"
#define CONFIG_PREHEAT_NAME_6         "preheat_name_6:"
#define CONFIG_PREHEAT_TEMP_1         "preheat_temp_1:"
#define CONFIG_PREHEAT_TEMP_2         "preheat_temp_2:"
#define CONFIG_PREHEAT_TEMP_3         "preheat_temp_3:"
#define CONFIG_PREHEAT_TEMP_4         "preheat_temp_4:"
#define CONFIG_PREHEAT_TEMP_5         "preheat_temp_5:"
#define CONFIG_PREHEAT_TEMP_6         "preheat_temp_6:"
//-----------------------------Power Supply Settings (only if connected to TFT controller)
#define CONFIG_PS_ACTIVE_HIGH         "ps_active_high:"
#define CONFIG_PS_AUTO_SHUTDOWN       "ps_auto_shutdown:"
#define CONFIG_PS_AUTO_SHUTDOWN_TEMP  "ps_auto_shutdown_temp:"
//-----------------------------Filament Runout Settings (only if connected to TFT controller)
#define CONFIG_FIL_RUNOUT             "fil_runout:"
#define CONFIG_FIL_RUNOUT_INVERTED    "fil_runout_inverted:"
#define CONFIG_FIL_RUNOUT_NC          "fil_runout_nc:"
#define CONFIG_FIL_RUNOUT_NOISE       "fil_runout_noise_threshold:"
#define CONFIG_FIL_RUNOUT_DISTANCE    "fil_runout_distance:"
//-----------------------------Power Loss Recovery & BTT UPS Settings
#define CONFIG_PL_RECOVERY            "pl_recovery:"
#define CONFIG_PL_RECOVERY_HOME       "pl_recovery_home:"
#define CONFIG_PL_RECOVERY_Z_RAISE    "pl_recovery_z_raise:"
#define CONFIG_BTT_MINI_UPS           "btt_mini_ups:"
//-----------------------------Other Device-Specific Settings
#define CONFIG_TOUCH_SOUND            "touch_sound:"
#define CONFIG_TOAST_SOUND            "toast_sound:"
#define CONFIG_ALERT_SOUND            "alert_sound:"
#define CONFIG_HEATER_SOUND           "heater_sound:"
#define CONFIG_LCD_BRIGHTNESS         "lcd_brightness:"
#define CONFIG_LCD_IDLE_BRIGHTNESS    "lcd_idle_brightness:"
#define CONFIG_LCD_IDLE_TIME          "lcd_idle_time:"
#define CONFIG_LCD_LOCK_ON_IDLE       "lcd_lock_on_idle:"
#define CONFIG_KNOB_LED_COLOR         "knob_led_color:"
#define CONFIG_KNOB_LED_IDLE          "knob_led_idle:"
#define CONFIG_NEOPIXEL_PIXELS        "neopixel_pixels:"
//-----------------------------Custom G-code Commands
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
//-----------------------------Start, End & Cancel G-code Commands
#define CONFIG_START_GCODE_ENABLED    "start_gcode_enabled:"
#define CONFIG_END_GCODE_ENABLED      "end_gcode_enabled:"
#define CONFIG_CANCEL_GCODE_ENABLED   "cancel_gcode_enabled:"
#define CONFIG_START_GCODE            "start_gcode:"
#define CONFIG_END_GCODE              "end_gcode:"
#define CONFIG_CANCEL_GCODE           "cancel_gcode:"

//-----------------------------Limits
#define MAX_SIZE_LIMIT         2000  // machine size over this will not be parsed.
#define MAX_EXT_SPEED_LIMIT    5000  // Extruder speed over this will not be parsed.
#define MAX_TOOL_TEMP          1000  // extruder temp over this will not be parsed.
#define MAX_BED_TEMP            400  // bed temp over this will not be parsed.
#define MAX_CHAMBER_TEMP        200  // bed temp over this will not be parsed.
#define MAX_SPEED_LIMIT       12000  // speed over this will not be parsed.
#define MAX_RUNOUT_DISTANCE      50  // runout distance over this will not be parsed.
#define MAX_DELAY_MS           1800  // time delay (ms) over this will not be parsed.
#define MAX_DELAY_SEC           100  // time delay (sec) over this will not be parsed.
#define MAX_RETRACT_LIMIT        20  // retract length over this will not be parsed.
#define MAX_FAN_SPEED           255  // fan speed (PWM) over this will not be parsed.
#define MAX_NEOPIXEL_PIXELS     200  // neopixel pixels over this will not be parsed.

#define MIN_SIZE_LIMIT        -2000  // machine size less than this will not be parsed.
#define NAME_MIN_LENGTH           3  // minimum name length
#define GCODE_MIN_LENGTH          3  // gcode length less than this will not be parsed.
#define MIN_XY_POS_LIMIT      -2000  // Set a negative minimum position for Delta printers
#define MIN_Z_POS_LIMIT           0
#define MIN_PROBING_Z_RAISE   -2000  // Set a negative minimum position for Delta printers
#define MIN_TOOL_TEMP            20  // extruder temp less than this will not be parsed.
#define MIN_BED_TEMP             20  // bed temp less than this will not be parsed.
#define MIN_CHAMBER_TEMP         20  // chamber temp less than this will not be parsed.
#define MIN_RUNOUT_DISTANCE       1  // runout distance less than this will not be parsed.
#define MIN_DELAY_MS             10  // time delay (ms) less than this will not be parsed.
#define MIN_DELAY_SEC             1  // time delay (sec) less than this will not be parsed.
#define MIN_SPEED_LIMIT          10  // speed less than this will not be parsed.
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

// This List is Auto-Generated. Please add new config in config.inc only
enum
{
  #define X_CONFIG(NAME) C_INDEX_##NAME ,
    #include "config.inc"
  #undef X_CONFIG

  CONFIG_COUNT,
};

#ifdef __cplusplus
}
#endif

#endif
