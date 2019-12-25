#ifndef _CONFIGRATION_H_
#define _CONFIGRATION_H_

//===========================================================================
//=========================== Marlin Mode Settings ===========================
//===========================================================================

/**
 * Default Marlin Mode Background & Font Color Options
 *
 * These colors can be changed in TFT mode, but can also be set here.
 *
 * Current color options from lcd.h: BLACK, BLUE, BROWN, BRRED, CYAN, GBLUE, GRAY, GREEN, MAGENTA, RED, WHITE, YELLOW
 */

// Marlin Mode Background & Font Color Options
// Current color options from lcd.h: BLACK, BLUE, BROWN, BRRED, CYAN, GBLUE, GRAY, GREEN, MAGENTA, RED, WHITE, YELLOW
#define ST7920_BKCOLOR BLACK
#define ST7920_FNCOLOR GREEN

// Text displayed at the top of the LCD in Marlin Mode.
//#define ST7920_BANNER_TEXT "LCD12864 Simulator"

// Run Marlin Mode fullscreen. Not recommended for TFT24.
//#define ST7920_FULLSCREEN

//===========================================================================
//============================ TFT Mode Settings ============================
//===========================================================================

// Show BTT bootscreen when starting up
#define SHOW_BTT_BOOTSCREEN

// TFT mode color
#define TITLE_BACKGROUND_COLOR      BLACK  // Title background color // 0xD928
#define BACKGROUND_COLOR            BLACK  // Background color // 0x0A29
#define FONT_COLOR                  WHITE  // Font foreground color
#define REMINDER_FONT_COLOR         RED    // Reminder font color, such as: "No print attached", "Busy process", etc...
#define VOLUME_REMINDER_FONT_COLOR  GBLUE  // Volume reminder font color,such as: "Card inserted", "Card removed"


#define TOOL_NUM     1    // set in 1~6
#define EXTRUDER_NUM 1    // set in 1~6
#define FAN_NUM      1    // set in 1~6

//                       PLA      PETG       ABS
#define PREHEAT_BED      {60,      70,       100}
#define PREHEAT_HOTEND   {200,     250,      230}
#define PREHEAT_TEXT     {"PLA",  "PETG",   "ABS"}

#define HEAT_MAX_TEMP    {150,    275,       275,       275,       275,       275,       275}    //max temperature can be set
#define HEAT_SIGN_ID     {"B:",   "T0:",     "T1:",     "T2:",     "T3:",     "T4:",     "T5:"}
#define HEAT_DISPLAY_ID  {"Bed",  "T0",      "T1",      "T2",      "T3",      "T4",      "T5"}
#define HEAT_CMD         {"M140", "M104 T0", "M104 T1", "M104 T2", "M104 T3", "M104 T4", "M104 T5" };
#define HEAT_WAIT_CMD    {"M190", "M109 T0", "M109 T1", "M109 T2", "M109 T3", "M109 T4", "M109 T5" };

#define TOOL_CHANGE      {"T0",   "T1",      "T2",      "T3",      "T4",      "T5"}
#define EXTRUDER_ID      {"E0",   "E1",      "E2",      "E3",      "E4",      "E5"}

#define FAN_MAX_PWM      {255,       255,       255,       255,       255,       255}
#define FAN_ID           {"F0",    "F1",    "F2",    "F3",    "F4",    "F5"}
#define FAN_CMD          {"M106 P0", "M106 P1", "M106 P2", "M106 P3", "M106 P4", "M106 P5" };

#define SPEED_ID         {"Sp.", "Fr."}

// Default move speed mm/min
#define DEFAULT_SPEED_MOVE      3000
#define SPEED_MOVE_SLOW         1000
#define SPEED_MOVE_FAST         5000
// Extrude speed mm/min
#define EXTRUDE_SLOW_SPEED      60
#define EXTRUDE_NORMAL_SPEED    600
#define EXTRUDE_FAST_SPEED      1200

// Size of machine
#define X_MIN_POS 0
#define Y_MIN_POS 0
#define Z_MIN_POS 0
#define X_MAX_POS 235
#define Y_MAX_POS 235
#define Z_MAX_POS 250

// Specify a pause position as { X, Y, Z_raise }
#define NOZZLE_PAUSE_RETRACT_LENGTH 15   // (mm)
#define NOZZLE_RESUME_PURGE_LENGTH  16   // (mm)
#define NOZZLE_PAUSE_X_POSITION     (X_MIN_POS + 10)  // (mm) Must be an integer
#define NOZZLE_PAUSE_Y_POSITION     (Y_MIN_POS + 10)  // (mm) Must be an integer
#define NOZZLE_PAUSE_Z_RAISE        20   // (mm)
#define NOZZLE_PAUSE_E_FEEDRATE     6000 // (mm/min) retract & purge feedrate
#define NOZZLE_PAUSE_XY_FEEDRATE    6000 // (mm/min) X and Y axes feedrate
#define NOZZLE_PAUSE_Z_FEEDRATE     600  // (mm/min) Z axis feedrate

// Send G29 for auto bed leveling
#define AUTO_BED_LEVELING
#ifdef AUTO_BED_LEVELING
  // Enable this will send "M500" after "G29" to store leveling value
  // and send "M420 S1" to enable leveling state after startup
  #define AUTO_SAVE_LOAD_LEVELING_VALUE
#endif

// Move to four corner points to Leveling manually (Point 1, Point 2, Point 3, Point 4)
#define LEVELING_POINT_1_X         (X_MIN_POS + 20)
#define LEVELING_POINT_1_Y         (Y_MIN_POS + 20)
#define LEVELING_POINT_2_X         (X_MAX_POS - 20)
#define LEVELING_POINT_2_Y         (Y_MIN_POS + 20)
#define LEVELING_POINT_3_X         (X_MAX_POS - 20)
#define LEVELING_POINT_3_Y         (Y_MAX_POS - 20)
#define LEVELING_POINT_4_X         (X_MIN_POS + 20)
#define LEVELING_POINT_4_Y         (Y_MAX_POS - 20)
#define LEVELING_POINT_Z           0.2f  // Z-axis position when nozzle stays for leveling
#define LEVELING_POINT_MOVE_Z      10.0f // Z-axis position when nozzle move to next point
#define LEVELING_POINT_XY_FEEDRATE 6000  // (mm/min) X and Y axes move feedrate
#define LEVELING_POINT_Z_FEEDRATE  600   // (mm/min) Z axis move feedrate

// Power Supply
#define PS_ON_ACTIVE_HIGH    true   // Set 'false' for ATX (1), 'true' for X-Box (2)

// Filament runout detection
#define FIL_RUNOUT_INVERTING true  // Set to false to invert the logic of the sensor.
#define FIL_NOISE_THRESHOLD  10     // 10*10 = 100ms,  Pause print when filament runout is detected for 100ms.

// Smart filament runout detection
// For use with an encoder disc that toggles runout pin as filament moves
#define FILAMENT_RUNOUT_DISTANCE_MM 7

// Enable alternative Move Menu Buttons Layout matching the direction of actual printer axis.
// update the icons from alternate icon folder
#define ALTERNATIVE_MOVE_MENU

// Invert the Y Axis move Direction
// this does not work if LIST MODE is enabled. To invert y axis in LIST MODE go to setting->feature settings
//#define INVERT_YAXIS

//Invert the Z Axis move Direction
//#define INVERT_ZAXIS

// Enable Unified Move Menu
// Move, Home, Extrude, ABL at one Place and bring Gcode Menu on Home Menu
//#define UNIFIED_MENU

/**
 * Enable list mode in Files menu and settings menu
 * It is friendly to display long file name, but the model preview feature is not available
 * Disable this if you want to use the model preview feature
 */
#define MENU_LIST_MODE


//-------RESET SETTINGS & TOUCH SCREEN CALIBRATION------||
// To reset the touch screen create a text file with name 'reset.txt' in root folder of the sd card and press reset button.


// SD support
#define ONBOARD_SD_SUPPORT
#ifdef ONBOARD_SD_SUPPORT
  #define M27_AUTOREPORT                      // Disable M27 polling if you enable enable AUTO_REPORT_SD_STATUS in Marlin
  #define M27_REFRESH                3        // Time in sec for M27 command
  #define M27_WATCH_OTHER_SOURCES    true     // if true the polling on M27 report is always active. Case: SD print started not from TFT35
#endif

/**
 * Home before power loss recovery
 * Many printer will crash printed model when homing, which is not suitable for home before PLR.
 * This function is suitable for Delta Printer.
 */
//#define HOME_BEFORE_PLR
//#define BTT_MINI_UPS // Backup power / UPS to move the Z axis steppers on power loss
#define POWER_LOSS_ZRAISE 10 // (mm) Z axis raise on resume (on power loss with UPS)


// Prevent extrusion if the temperature is below set temperature
#define PREVENT_COLD_EXTRUSION_MINTEMP 170

/**
  * Maximum hotend temperature of automatic shut down after printing.
  * When enable automatic shutdown(Auto Power), when the hotend temperature is higher than this value
  * turn on the fan to cool down, wait for the hotend temperature to be lower than this value, then turn off the power automatically
  */
#define AUTO_SHUT_DOWN_MAXTEMP 50

#define EXTRUDE_STEPS  100.0f

#define SHOW_FAN_PERCENTAGE // enable to show fan speed as a percentage instead of a value

/**
 * Support up to 7 custom gcodes, uncomment CUSTOM_X_LABEL and CUSTOM_X_GCODE to enable custom gcode
 * CUSTOM_X_LABEL is the name of the custom button, CUSTOM_X_GCODE
 * CUSTOM_X_GCODE is the gcode to be sent by the custom button, end with '\n'
 * You also need to customize the icon corresponding to the command
 * Copy your custom icon to the SD card to be updated, such as:"TFT35/bmp/Custom0.bmp", "TFT24/bmp/Custom1.bmp", etc...
 * The format of the custom icon is as follows
 * Bit depth: 24 / 32 bit, Pixel size: 95*95(for TFT35), 70*70(for TFT28/TFT24)
 */
#define CUSTOM_0_LABEL "Restore EEPROM"
#define CUSTOM_0_GCODE "M501\n"
//#define CUSTOM_1_LABEL "Custom2"
//#define CUSTOM_1_GCODE "M105\n"
//#define CUSTOM_2_LABEL "Custom2"
//#define CUSTOM_2_GCODE "M105\n"
//#define CUSTOM_3_LABEL "Custom3"
//#define CUSTOM_3_GCODE "M105\n"
//#define CUSTOM_4_LABEL "Custom4"
//#define CUSTOM_4_GCODE "M105\n"
//#define CUSTOM_5_LABEL "Custom5"
//#define CUSTOM_5_GCODE "M105\n"
//#define CUSTOM_6_LABEL "Custom6"
//#define CUSTOM_6_GCODE "M105\n"

/*
custom gcode below are compatible only in if LIST_MODE is active
*/
#ifdef MENU_LIST_MODE
//#define CUSTOM_7_LABEL "Custom7"
//#define CUSTOM_7_GCODE "M105\n"
//#define CUSTOM_8_LABEL "Custom8"
//#define CUSTOM_8_GCODE "M105\n"
//#define CUSTOM_9_LABEL "Custom9"
//#define CUSTOM_9_GCODE "M105\n"
//#define CUSTOM_10_LABEL "Custom10"
//#define CUSTOM_10_GCODE "M105\n"
//#define CUSTOM_11_LABEL "Custom11"
//#define CUSTOM_11_GCODE "M105\n"
//#define CUSTOM_12_LABEL "Custom12"
//#define CUSTOM_12_GCODE "M105\n"
//#define CUSTOM_13_LABEL "Custom13"
//#define CUSTOM_13_GCODE "M105\n"
//#define CUSTOM_14_LABEL "Custom14"
//#define CUSTOM_14_GCODE "M105\n"
#endif

#define CANCEL_PRINT_GCODE "G28 X0 Y0\n"

#endif
