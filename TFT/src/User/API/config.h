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
#define MAX_SIZE_LIMIT       2000  // machine size over this will not be parsed
#define MAX_EXT_SPEED_LIMIT  5000  // extruder speed over this will not be parsed
#define MAX_TOOL_TEMP        1000  // extruder temp over this will not be parsed
#define MAX_BED_TEMP          400  // bed temp over this will not be parsed
#define MAX_CHAMBER_TEMP      200  // bed temp over this will not be parsed
#define MAX_SPEED_LIMIT     12000  // speed over this will not be parsed
#define MAX_RUNOUT_DISTANCE    50  // runout distance over this will not be parsed
#define MAX_DELAY_MS         1800  // time delay (ms) over this will not be parsed
#define MAX_DELAY_SEC         100  // time delay (sec) over this will not be parsed
#define MAX_RETRACT_LIMIT      20  // retract length over this will not be parsed
#define MAX_FAN_SPEED         255  // fan speed (PWM) over this will not be parsed
#define MAX_LED_COLOR_COMP    255  // (neopixel) LED color component over this will not be parsed
#define MAX_NEOPIXEL_PIXELS   200  // neopixel pixels over this will not be parsed

#define MIN_TX_SLOTS            2  // tx slots less than this will not be parsed
#define MIN_SIZE_LIMIT      -2000  // machine size less than this will not be parsed
#define NAME_MIN_LENGTH         3  // minimum name length
#define GCODE_MIN_LENGTH        3  // gcode length less than this will not be parsed
#define MIN_XY_POS_LIMIT    -2000  // set a negative minimum position for Delta printers
#define MIN_Z_POS_LIMIT         0
#define MIN_PROBING_Z_RAISE -2000  // set a negative minimum position for Delta printers
#define MIN_TOOL_TEMP          20  // extruder temp less than this will not be parsed
#define MIN_BED_TEMP           20  // bed temp less than this will not be parsed
#define MIN_CHAMBER_TEMP       20  // chamber temp less than this will not be parsed
#define MIN_RUNOUT_DISTANCE     1  // runout distance less than this will not be parsed
#define MIN_DELAY_MS           10  // time delay (ms) less than this will not be parsed
#define MIN_DELAY_SEC           1  // time delay (sec) less than this will not be parsed
#define MIN_SPEED_LIMIT        10  // speed less than this will not be parsed
#define MIN_RETRACT_LIMIT       0  // retract length less than this will not be parsed
#define MIN_FAN_SPEED          25  // fan speed (PWM) less than this will not be parsed
#define MIN_LED_COLOR_COMP      0  // (neopixel) LED color component less than this will not be parsed

void resetConfig(void);
bool getConfigFromFile(char * configPath);
bool getLangFromFile(char * rootDir);

#ifdef __cplusplus
}
#endif

#endif
