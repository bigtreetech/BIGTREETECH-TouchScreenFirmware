#ifndef _CONFIGRATION_H_
#define _CONFIGRATION_H_

#define TOOL_NUM     1    //set in 1~6
#define EXTRUDER_NUM 1    //set in 1~6
#define FAN_NUM      1    //set in 1~6

#define HEAT_MAX_TEMP    {150,    275,       275,       275,       275,       275,       275}    //max temperature can be set
#define HEAT_SIGN_ID     {"B:",   "T0:",     "T1:",     "T2:",     "T3:",     "T4:",     "T5:"}
#define HEAT_DISPLAY_ID  {"Bed",  "T0",      "T1",      "T2",      "T3",      "T4",      "T5"}
#define HEAT_CMD         {"M140", "M104 T0", "M104 T1", "M104 T2", "M104 T3", "M104 T4", "M104 T5" };
#define HEAT_WAIT_CMD    {"M190", "M109 T0", "M109 T1", "M109 T2", "M109 T3", "M109 T4", "M109 T5" };

#define TOOL_CHANGE      {"T0",   "T1",      "T2",      "T3",      "T4",      "T5"}
#define EXTRUDER_ID      {"E0",   "E1",      "E2",      "E3",      "E4",      "E5"}

#define FAN_MAX_PWM      {255,       255,       255,       255,       255,       255}
#define FAN_ID           {"Fan0",    "Fan1",    "Fan2",    "Fan3",    "Fan4",    "Fan5"}
#define FAN_CMD          {"M106 P0", "M106 P1", "M106 P2", "M106 P3", "M106 P4", "M106 P5" };

#define DEFAULT_SPEED_MOVE      3000  //Move default speed  mm/min

#define EXTRUDE_SLOW_SPEED      60   //Extrude speed  mm/min
#define EXTRUDE_NORMAL_SPEED    600  //
#define EXTRUDE_FAST_SPEED      1200 //

//size of machine
#define X_MIN_POS 0
#define Y_MIN_POS 0
#define Z_MIN_POS 0
#define X_MAX_POS 235
#define Y_MAX_POS 235
#define Z_MAX_POS 300

//Specify a Pause position as { X, Y, Z_raise }
#define NOZZLE_PAUSE_RETRACT_LENGTH 15   // (mm)
#define NOZZLE_PAUSE_PURGE_LENGTH   16   // (mm)
#define NOZZLE_PAUSE_X_POSITION     (X_MIN_POS + 10)  // (mm) Must be an integer
#define NOZZLE_PAUSE_Y_POSITION     (Y_MIN_POS + 10)  // (mm) Must be an integer
#define NOZZLE_PAUSE_Z_RAISE        20   // (mm)
#define NOZZLE_PAUSE_E_FEEDRATE     6000 // (mm/min) retract & purge feedrate
#define NOZZLE_PAUSE_XY_FEEDRATE    6000 // (mm/min) X and Y axes feedrate
#define NOZZLE_PAUSE_Z_FEEDRATE     600  // (mm/min) Z axis feedrate

#define AUTO_BED_LEVELING
//Move to four corner points to Leveling manually (Point 1, Point 2, Point 3, Point 4)
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


//Power Supply
#define PS_ON_ACTIVE_HIGH    true   // Set 'false' for ATX (1), 'true' for X-Box (2)
  
//Filament run out detect
#define FIL_RUNOUT_INVERTING true  // Set to false to invert the logic of the sensor.
#define FIL_NOISE_THRESHOLD  5     // 5*10 = 50ms,  Pause print when filament runout is detected for 50ms.

/**
 * 12864 Mode Background & Font Color Options
 * Current color options from lcd.h: BLACK, BLUE, BROWN, BRRED, CYAN, GBLUE, GRAY, GREEN, MAGENTA, RED, WHITE, YELLOW
 */
#define ST7920_BKCOLOR BLACK
#define ST7920_FNCOLOR GREEN

// Show BTT bootscreen when starting up
#define SHOW_BTT_BOOTSCREEN

// Text displayed at the top of the LCD in 12864 mode
#define ST7920_BANNER_TEXT "LCD12864 Simulator"

// Make the simulator run fullscreen, Not recommended for TFT24
//#define ST7920_FULLSCREEN

// Ability to print gcode from Board SD via Gcode functions.
#define ONBOARD_SD_SUPPORT
#ifdef ONBOARD_SD_SUPPORT
  #define M27_AUTOREPORT                      // Disable the M27 polling if you enable enable Marlin AUTO_REPORT_SD_STATUS
  #define M27_REFRESH                 3       // Time in sec for M27 command 
  #define M27_WATCH_OTHER_SOURCES    true     // if true the polling on M27 report is always active. Case: SD print start not from TFT35
#endif

/**
 * Home before power loss recovery
 * Many printer will crash printed model when homing, which is not suitable for home before PLR.
 * This function is suitable for Delta Printer.
 */
//#define HOME_BEFORE_PLR
  
#endif
