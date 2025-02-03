#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define CONFIG_FILE_PATH "config.ini"
#define LINE_MAX_CHAR    200

//-----------------------------Limits
#define MAX_TX_SLOTS           16  // tx slots over this will not be parsed
#define MAX_TX_DELAY           10  // tx delay over this will not be parsed
#define MAX_COLOR_SCHEME        2  // terminal color scheme over this will not be parsed
#define MAX_ACK_NOTIF           2  // ack notification over this will not be parsed
#define MAX_PROG_SOURCE         1  // prog source over this will not be parsed
#define MAX_PROG_DISP_TYPE      2  // prog disp type over this will not be parsed
#define MAX_LAYER_DISP_TYPE     2  // layer disp type over this will not be parsed
#define MAX_TOOL_TEMP        1000  // extruder temp over this will not be parsed
#define MAX_BED_TEMP          400  // bed temp over this will not be parsed
#define MAX_CHAMBER_TEMP      200  // bed temp over this will not be parsed
#define MAX_FAN_SPEED         255  // fan speed (PWM) over this will not be parsed
#define MAX_SIZE_LIMIT       2000  // machine size over this will not be parsed
#define MAX_SPEED_LIMIT     12000  // speed over this will not be parsed
#define MAX_ONBOARD_SD          2  // onboard sd over this will not be parsed
#define MAX_DELAY_SEC         100  // time delay (sec) over this will not be parsed
#define MAX_LONG_FILENAME       2  // long filename over this will not be parsed
#define MAX_RETRACT_LIMIT      20  // retract length over this will not be parsed
#define MAX_AUTO_SHUTDOWN       2  // ps auto shutdown mode over this will not be parsed
#define MAX_FIL_RUNOUT          3  // filament runout mode over this will not be parsed
#define MAX_DELAY_MS         1800  // time delay (ms) over this will not be parsed
#define MAX_RUNOUT_DISTANCE    50  // runout distance over this will not be parsed
#define MAX_LED_COLOR_COMP    255  // (neopixel) LED color component over this will not be parsed

#define MAX_NEOPIXEL_PIXELS   200  // neopixel pixels over this will not be parsed

#define MIN_TX_SLOTS            2  // tx slots less than this will not be parsed
#define MIN_TX_DELAY            0  // tx delay less than this will not be parsed
#define MIN_COLOR_SCHEME        0  // terminal color scheme less than this will not be parsed
#define MIN_ACK_NOTIF           0  // ack notification less than this will not be parsed
#define MIN_PROG_SOURCE         0  // prog source less than this will not be parsed
#define MIN_PROG_DISP_TYPE      0  // prog disp type less than this will not be parsed
#define MIN_LAYER_DISP_TYPE     0  // layer disp type less than this will not be parsed
#define MIN_TOOL_TEMP          20  // extruder temp less than this will not be parsed
#define MIN_BED_TEMP           20  // bed temp less than this will not be parsed
#define MIN_CHAMBER_TEMP       20  // chamber temp less than this will not be parsed
#define MIN_FAN_SPEED          25  // fan speed (PWM) less than this will not be parsed
#define MIN_SIZE_LIMIT      -2000  // machine size less than this will not be parsed
#define MIN_SPEED_LIMIT        10  // speed less than this will not be parsed
#define MIN_ONBOARD_SD          0  // onboard sd less than this will not be parsed
#define MIN_DELAY_SEC           1  // time delay (sec) less than this will not be parsed
#define MIN_LONG_FILENAME       0  // long filename less than this will not be parsed
#define MIN_RETRACT_LIMIT       0  // retract length less than this will not be parsed
#define MIN_AUTO_SHUTDOWN       0  // ps auto shutdown mode less than this will not be parsed
#define MIN_FIL_RUNOUT          0  // filament runout mode less than this will not be parsed
#define MIN_DELAY_MS           10  // time delay (ms) less than this will not be parsed
#define MIN_RUNOUT_DISTANCE     1  // runout distance less than this will not be parsed
#define MIN_LED_COLOR_COMP      0  // (neopixel) LED color component less than this will not be parsed

#define MIN_NAME_LENGTH         3  // name length less than this will not be parsed
#define MIN_GCODE_LENGTH        3  // gcode length less than this will not be parsed
#define MIN_XY_POS_LIMIT    -2000  // xy position less than this will not be parsed
#define MIN_Z_POS_LIMIT         0  // z position less than this will not be parsed
#define MIN_PROBING_Z_RAISE -2000  // probing z raise less than this will not be parsed

void resetConfig(void);
bool getConfigFromFile(char * configPath);
bool getLangFromFile(char * rootDir);

#ifdef __cplusplus
}
#endif

#endif
