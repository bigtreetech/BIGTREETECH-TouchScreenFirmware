#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_
#define CONFIG_VERSION 20210730

//====================================================================================================
//=============================== Settings Configurable On config.ini ================================
//====================================================================================================

//================================================================================
//=============================== General Settings ===============================
//================================================================================

/**
 * Enable Status Screen
 * If enabled, the Status Screen menu will become the default home screen.
 * If disabled, the Main menu will become the default home screen.
 *
 * NOTE: Both the Status Screen and Main menus display the current temperature, fan and speeds.
 *       Furthermore, the Status Screen menu provides the status area reporting the printer notifications.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define ENABLE_STATUS_SCREEN 1  // Default: 1

/**
 * Baudrate / Connection Speed
 * This baudrate setting is used for serial connection to the printer and other serial
 * hosts like ESP8266.
 *
 * NOTE: 250000 works in most cases, but you might try a lower speed if you commonly
 *       experience drop-outs during host printing.
 *       You may try up to 1000000 to speed up SD file transfer.
 *
 *   Options: [2400: 0, 9600: 1, 19200: 2, 38400: 3, 57600: 4, 115200: 5, 250000: 6, 500000: 7, 1000000: 8]
 */
#define BAUDRATE 5  // Default: 5

/**
 * Default Touch Mode Colors
 * Set colors used in touchscreen mode.
 *
 *   Options: [ WHITE: 0,  BLACK: 1,  RED: 2,  GREEN: 3,      BLUE: 4,       CYAN: 5,  MAGENTA: 6,    YELLOW: 7,
 *             ORANGE: 8, PURPLE: 9, LIME: 10, BROWN: 11, DARKBLUE: 12, DARKGREEN: 13,    GRAY: 14, DARKGRAY: 15]
 */
#define TITLE_BACKGROUND_COLOR      1  // Title background color (Default: 1)
#define BACKGROUND_COLOR            1  // Background color (Default: 1)
#define FONT_COLOR                  0  // Font foreground color (Default: 0)
#define REMINDER_FONT_COLOR         2  // Reminder font color, such as: "No print attached", "Busy processing", etc (Default: 2)
#define VOLUME_REMINDER_FONT_COLOR  5  // Volume status/reminder font color, such as: "Card inserted", "Card removed" (Default: 5)
#define STATUS_XYZ_BG_COLOR        15  // Backgroud color for X Y Z position display in Status Screen menu (Default: 15)
#define LISTVIEW_BORDER_COLOR      15  // List View border color (Default: 15)
#define LISTVIEW_ICON_COLOR        15  // List View button background color (Default: 15)

// Mesh Leveling Display Colors (Mesh Editor)
// Set the colors used for drawing the mesh with the minimun and maximum value in the grid.
#define MESH_MIN_COLOR 7  // Default: 7
#define MESH_MAX_COLOR 2  // Default: 2

/**
 * Fan Speed As Percentage
 * Show fan speed as percentage. If disabled fan speeed will be displayed as PWM values.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define SHOW_FAN_PERCENTAGE 1  // Default: 1

/**
 * Notification Style For ACK Messages
 * Set the notification style to use for displaying the ACK messages which start with "echo:".
 *
 * NOTE: The OFF value is applied to any ACK message type (e.g. even to known echo ACK).
 *       It means that any kind of ACK message is silently discarded.
 *
 *   Options: [OFF: 0, POPUP: 1, TOAST: 2]
 *     OFF:   No notification. The message is ignored.
 *     POPUP: Display a popup window for user confirmation.
 *     TOAST: A non-blocking Toast notification is displayed for few seconds. No user interaction is needed.
 */
#define ACK_NOTIFICATION_STYLE 1  // Default: 1

/**
 * Emulate M600
 * The TFT intercepts the M600 gcode (filament change) and emulates the handling logic
 * otherwise provided by Marlin firmware.
 *
 * NOTE: Enable it, in case Marlin firmware does not properly support M600 on the mainboard.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define EMULATE_M600 1  // Default: 1

//================================================================================
//============================= Marlin Mode Settings =============================
//========== (only for TFT24 V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70 V3.0) ==========
//================================================================================

/**
 * Default Mode
 * Set Marlin/Touch mode as the default mode at startup.
 *
 * NOTE: Mode switching is possible only for Marlin Mode and Touch Mode by a long press of
 *       1.5 seconds on the display or holding down the encorder button for 1.5 seconds.
 *
 *   Options: [Marlin Mode: 0, Touch Mode: 1, Blocked Marlin Mode: 2, Blocked Touch Mode: 3]
 */
#define DEFAULT_LCD_MODE MODE_SERIAL_TSC  // Default: MODE_SERIAL_TSC

/**
 * Serial Always ON
 * Keep UART (serial communication) alive in Marlin Mode.
 * Allows seamless OctoPrint UART connection to the TFT's UART/serial expansion port
 * no matter which mode the TFT is in.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define SERIAL_ALWAYS_ON 0  // Default: 0

/**
 * Default Marlin Mode Background & Font Colors
 * Set colors used in Marlin mode.
 *
 *   Options: [ WHITE: 0,  BLACK: 1,  RED: 2,  GREEN: 3,      BLUE: 4,       CYAN: 5,  MAGENTA: 6,    YELLOW: 7,
 *             ORANGE: 8, PURPLE: 9, LIME: 10, BROWN: 11, DARKBLUE: 12, DARKGREEN: 13,    GRAY: 14, DARKGRAY: 15]
 */
#define MARLIN_BKCOLOR 1  // Marlin Mode background color (Default: 1)
#define MARLIN_FNCOLOR 0  // Marlin Mode font color (Default: 0)

/**
 * Marlin Mode Fullscreen Support
 * Run Marlin Mode in fullscreen.
 *
 * NOTE: Disable is recommended for TFT24.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define MARLIN_MODE_FULLSCREEN 0  // Default: 0

/**
 * Marlin Mode Title Support
 * Show banner text at the top of the TFT in Marlin Mode.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define MARLIN_SHOW_BANNER 1  // Default: 1

/**
 * Marlin Mode Title
 * Banner text displayed at the top of the TFT in Marlin Mode.
 *
 *   Value range: [min: 3, max: 20 characthers]
 */
#define MARLIN_BANNER_TEXT "Marlin Mode"  // Default: "Marlin Mode"

//================================================================================
//========================== Printer / Machine Settings ==========================
//================================================================================

#define HOTEND_NUM      1  // set in 1~6
#define EXTRUDER_NUM    1  // set in 1~6
#define MIXING_EXTRUDER 0  // Default: 0. For mixing_extruder set to 1 (This option turns off autodetection
                           // of the number of extruders)
#define FAN_NUM         1  // set in 1~6
#define ENABLE_CTRL_FAN 1  // Set 0 to disable & 1 to enable controller fan speed control for Idle and Active
                           // cooling if marlin supports ontroller fan (M710).

// PID autotune
#define PID_CMD_MARLIN      {"M303 U1 C8 E0", "M303 U1 C8 E1", "M303 U1 C8 E2", "M303 U1 C8 E3", "M303 U1 C8 E4", "M303 U1 C8 E5", "M303 U1 C8 E-1", ""}
#define PID_CMD_RRF         {"M303 T0",       "M303 T1",       "M303 T2",       "M303 T3",       "M303 T4",       "M303 T5",       "M303 H0",        ""}
#define PID_PROCESS_TIMEOUT (15 * 60000)  // (MilliSeconds, 1 minute = 60000 MilliSeconds)

// Preheat
#define PREHEAT_LABELS {"PLA", "PETG", "ABS", "WOOD", "TPU", "NYLON"}
#define PREHEAT_HOTEND {200,   240,    230,   170,    220,   250}
#define PREHEAT_BED    { 60,    70,     90,    50,     50,    90}

// Heat
#define HEAT_MAX_TEMP   {275,       275,       275,       275,       275,       275,       150,    60}
#define HEAT_SIGN_ID    {"T0:",     "T1:",     "T2:",     "T3:",     "T4:",     "T5:",     "B:",   "C:"}
#define HEAT_DISPLAY_ID {"T0",      "T1",      "T2",      "T3",      "T4",      "T5",      "Bed",  "Chamber"}
#define HEAT_CMD        {"M104 T0", "M104 T1", "M104 T2", "M104 T3", "M104 T4", "M104 T5", "M140", "M141"}
#define HEAT_WAIT_CMD   {"M109 T0", "M109 T1", "M109 T2", "M109 T3", "M109 T4", "M109 T5", "M190", "M191"}

#define TOOL_CHANGE {"T0", "T1", "T2", "T3", "T4", "T5"}
#define EXTRUDER_ID {"E0", "E1", "E2", "E3", "E4", "E5"}

// Prevent extrusion if the temperature is below set temperature
#define PREVENT_COLD_EXTRUSION_MINTEMP 180

/**
 * Cooling Fan & Controller Fan
 * Cooling fan have index from 0 to 5.
 * Controller fan have two speed (Active and Idle) index 6 and 7.
 */
#define FAN_MAX_PWM    {255,   255,   255,   255,   255,   255,   255,   255}
#define FAN_DISPLAY_ID {"F0 ", "F1 ", "F2 ", "F3 ", "F4 ", "F5 ", "CtS", "CtI"}
#define FAN_CMD        {"M106 P0 S%d\n", "M106 P1 S%d\n", "M106 P2 S%d\n", "M106 P3 S%d\n", "M106 P4 S%d\n", "M106 P5 S%d\n", \
                        "M710 S%d\n",    "M710 I%d\n" }

// Size of machine
#define X_MIN_POS   0
#define Y_MIN_POS   0
#define Z_MIN_POS   0
#define X_MAX_POS 235
#define Y_MAX_POS 235
#define Z_MAX_POS 250

// Speed/flow rate names displayed in status screen
#define SPEED_ID {"Sp.", "Fr."}  // (speed, flow rate)

// Axes names displayed in Parameter Settings menu
#define AXIS_DISPLAY_ID    {"X", "Y", "Z", "E0", "E1"}  // (X, Y, Z, E0, E1)
#define STEPPER_DISPLAY_ID {"X", "X2", "Y", "Y2", "Z", "Z2", "E0", "E1"}  // (X, X2, Y, Y2, Z, Z2, E0, E1)

// Default X & Y speed (mm/min)
#define SPEED_XY_SLOW   1000
#define SPEED_XY_NORMAL 3000
#define SPEED_XY_FAST   5000

// Default Z speed (mm/min)
#define SPEED_Z_SLOW   500
#define SPEED_Z_NORMAL 1000
#define SPEED_Z_FAST   2000

// Extrude speed (mm/min)
#define EXTRUDE_SLOW_SPEED     60
#define EXTRUDE_NORMAL_SPEED  600
#define EXTRUDE_FAST_SPEED   1200

// Pause Settings
#define NOZZLE_PAUSE_RETRACT_LENGTH               15  // (mm)
#define NOZZLE_RESUME_PURGE_LENGTH                16  // (mm)
#define NOZZLE_PAUSE_X_POSITION     (X_MIN_POS + 10)  // (mm) Must be an integer
#define NOZZLE_PAUSE_Y_POSITION     (Y_MIN_POS + 10)  // (mm) Must be an integer
#define NOZZLE_PAUSE_Z_RAISE                      20  // (mm)
#define NOZZLE_PAUSE_E_FEEDRATE                  600  // (mm/min) retract & purge feedrate
#define NOZZLE_PAUSE_XY_FEEDRATE                6000  // (mm/min) X and Y axes feedrate
#define NOZZLE_PAUSE_Z_FEEDRATE                  600  // (mm/min) Z axis feedrate

/**
 * Manual Leveling
 * Move to four corner points to Leveling manually (Point 1, Point 2, Point 3, Point 4).
 */
#define LEVELING_EDGE_DISTANCE        20  // Inset distance from bed's edge for calculating leveling point location
#define LEVELING_POINT_Z            0.2f  // Z-axis position when nozzle stays for leveling
#define LEVELING_POINT_MOVE_Z      10.0f  // Z-axis position when nozzle move to next point
#define LEVELING_POINT_XY_FEEDRATE  6000  // (mm/min) X and Y axes move feedrate
#define LEVELING_POINT_Z_FEEDRATE    600  // (mm/min) Z axis move feedrate

#define LEVELING_EDGE_DISTANCE_DISPLAY_ID "X/Y"
#define LEVELING_EDGE_DISTANCE_MIN           0
#define LEVELING_EDGE_DISTANCE_MAX         100
#define LEVELING_EDGE_DISTANCE_DEFAULT    LEVELING_EDGE_DISTANCE

// Z Fade limits
#define Z_FADE_MIN_VALUE      0.0f
#define Z_FADE_MAX_VALUE     20.0f
#define Z_FADE_DEFAULT_VALUE 10.0f

// Probe Offset limits
#define PROBE_Z_OFFSET_MIN_VALUE     -20.0f
#define PROBE_Z_OFFSET_MAX_VALUE      20.0f
#define PROBE_Z_OFFSET_DEFAULT_VALUE   0.0f

// Home Offset limits
#define HOME_Z_OFFSET_MIN_VALUE     -20.0f
#define HOME_Z_OFFSET_MAX_VALUE      20.0f
#define HOME_Z_OFFSET_DEFAULT_VALUE   0.0f

// Babystep limits
#define BABYSTEP_MIN_VALUE     -5.0f
#define BABYSTEP_MAX_VALUE      5.0f
#define BABYSTEP_DEFAULT_VALUE  0.0f
#define BABYSTEP_MAX_STEP       1.0f

/**
 * Auto Save/Load Bed Leveling Data
 * The TFT will auto-detect if Auto Bed Level is available.
 * Enable this will send "M500" after "G29" to store leveling value and send "M420 S1"
 * to enable leveling state after startup.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define AUTO_SAVE_LOAD_BL_VALUE 1  // Default: 1

/**
 * TouchMI settings (ABL)
 * Enable this option for displaying TouchMI sensor settings in ABL menu (Init, Z Offset, Save, Test).
 *
 *   Options: [disable: 0, enable: 1]
 */
#define TOUCHMI_SENSOR_VALUE 0  // Default: 0

/**
 * Onboard / Printer SD
 * Starting from Marlin Bugfix 2.0.x Distribution Date: 2020-04-27 & above, the TFT will auto detect
 * On-Board SD Card and auto-configure M27 AutoReport with M115 command.
 * Set the time interval to poll SD Printing status if Marlin reports M27 AutoReport as disabled.
 */
#define M27_REFRESH             3  // Time in sec for M27 command
#define M27_WATCH_OTHER_SOURCES 1  // if "1" the polling on M27 report is always active. Case: SD print
                                   // started not from TFT35

/**
 * Z Raise Probing Support (Probe Offset, Mesh Editor)
 * Used by the Probe Offset / Mesh Editor menu for the Z offset / Z height tuning process.
 * Z height to raise / drop after homing (G28) before starting to probe a point.
 *
 * NOTE: It MUST BE a value >= 0 (e.g. 20) for a Cartesian printer to avoid crashing into the bed.
 *       It MUST BE a value <= 0 (e.g. -50) for a Delta printer to avoid crashing into the top of the tower.
 */
#define PROBING_Z_RAISE 20.0f

//================================================================================
//============================ Power Supply Settings =============================
//==================== (only if connected to TFT controller) =====================
//================================================================================

/**
 * Power Supply Active HIGH
 * Enable the logic of the mode on HIGH signal.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define PS_ON_ACTIVE_HIGH 1  // Default: 1 ("0" for ATX (1), "1" for X-Box (2))

/**
 * Power Supply Auto Shutdown Temperature
 * Maximum hotend temperature for automatic shutdown after printing, if automatic shutdown is enabled.
 * The printer will shutdown automatically if the hotend temperature is below this value.
 * If the hotend temperature is higher than this value the fans will be turned on to cooldown and it
 * will wait for the hotend temperature to drop below this value before shutting down automatically.
 */
#define AUTO_SHUT_DOWN_MAXTEMP 50  // Default: 50 (*C)

//================================================================================
//=========================== Filament Runout Settings ===========================
//==================== (only if connected to TFT controller) =====================
//================================================================================

/**
 * NOTE for users having a filament sensor connected to the mainboard:
 * 1) Define "FIL_SENSOR_TYPE 0" below to disable the sensor handling on the TFT.
 * 2) Configure the sensor in the firmware of your mainboard.
 * 3) Add M75 to "PRINT_START_GCODE" and M77 to "PRINT_END_GCODE" of the TFT (or your slicer).
 */

/**
 * Default Filament Runout Sensor
 * Select the type of filament/runout sensor and its default enabled/disabled state.
 *
 *   Options: [Normal Disabled: 0, Normal Enabled: 1, Smart Disabled: 2, Smart Enabled: 3]
 */
#define FIL_SENSOR_TYPE 0  // Default: 0

/**
 * Inverted Filament Runout Logic
 * Invert the logic of the sensor.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define FIL_RUNOUT_INVERTING 1  // Default: 1

// Filament Noise Threshold
// Pause print when filament runout is detected at least for this time period.
#define FIL_NOISE_THRESHOLD 100   // Default: 100 (ms)

// Smart Filament Runout Detection
// For use with an encoder disc that toggles runout pin as filament moves.
#define FILAMENT_RUNOUT_DISTANCE_MM 7  // Default: 7 (mm)

//================================================================================
//==================== Power Loss Recovery & BTT UPS Settings ====================
//================================================================================

/**
 * Most suitable for Delta printers since most printers will
 * crash into printed model when homing after powerloss.
 */

/**
 * Power Loss Recovery Homing
 * Home before power loss recovery.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define HOME_BEFORE_PLR 0  // Default: 0

// Power Loss Z Raise
// Raise Z axis on resume (on power loss with UPS).
#define POWER_LOSS_ZRAISE 10  // Default: 10 (mm)

/**
 * BTT UPS Support
 * Enable backup power/UPS to move Z axis on power loss.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define BTT_MINI_UPS 0  // Default: 0

//================================================================================
//======================== Other Device-Specific Settings ========================
//================================================================================

/**
 * Default LCD Brightness Levels (only for TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
 * Default brightness values for LCD.
 *
 *   Options: [off: 0, 5%: 1, 10%: 2, 20%: 3, 30%: 4, 40%: 5, 50%: 6, 60%: 7, 70%: 8, 80%: 9, 90%: 10, 100%: 11]
 */
#define DEFAULT_LCD_BRIGHTNESS      11  // Default display brightness (Default: 11)
#define DEFAULT_LCD_IDLE_BRIGHTNESS  3  // Display brightness when device is idle (Default: 3)

/**
 * Default LCD Idle Time (only for TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
 * The LCD screen will dim to idle brightness, if the display is not touched for the
 * period of the LCD idle time.
 *
 *   Options: [off: 0, 5sec: 1, 10sec: 2, 30sec: 3, 1min: 4, 2min: 5, 5min: 6, CUSTOM: 7]
 */
#define DEFAULT_LCD_IDLE_TIME 0  // Default: 0

// Custom value in seconds. This will be used if DEFAULT_LCD_IDLE_TIME is set to 7 (CUSTOM Seconds)
#define IDLE_TIME_CUSTOM (10 * 60)

/**
 * Knob LED Color (only for TFT28/TFT35_E3/TFT43/TFT50/TFT70 V3.0)
 * Knob LED color at startup.
 *
 *   Options: [OFF: 0, WHITE: 1, RED: 2, ORANGE: 3, YELLOW: 4, GREEN: 5, BLUE: 6, INDIGO: 7, VIOLET: 8]
 */
#define STARTUP_KNOB_LED_COLOR 1  // Default: 1

// Keep the LED state in Marlin Mode
#define KEEP_KNOB_LED_COLOR_MARLIN_MODE

//================================================================================
//============================ Custom Gcode Commands =============================
//================================================================================

/**
 * Custom Gcode Commands
 * Up to 15 custom gcode commands that will be available in the Custom menu.
 *
 * Usage:
 *   To enable a custom command, remove "//" at the begining of custom commands label & gcode.
 *   To disable a custom command, add "//" at the begining of custom commands label & gcode.
 *
 * NOTE: If the values are left blank then default name and gcode will be used.
 *
 *   Value range: label: [min: 3, max: 75 characters]
 *                gcode: [min: 3, max: 75 characters]
 */
#define CUSTOM_0_LABEL "Disable Steppers"
#define CUSTOM_0_GCODE "M84\n"
#define CUSTOM_1_LABEL "Init SD Card"
#define CUSTOM_1_GCODE "M21\n"
#define CUSTOM_2_LABEL "Release SD Card"
#define CUSTOM_2_GCODE "M22\n"
#define CUSTOM_3_LABEL "Enable Leveling State"
#define CUSTOM_3_GCODE "M420 S1\n"
#define CUSTOM_4_LABEL "Save to EEPROM"
#define CUSTOM_4_GCODE "M500\n"
#define CUSTOM_5_LABEL "Restore from EEPROM"
#define CUSTOM_5_GCODE "M501\n"
#define CUSTOM_6_LABEL "EEPROM Defaults"
#define CUSTOM_6_GCODE "M502\n"
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

//================================================================================
//====================== Start, End & Cancel Gcode Commands ======================
//================================================================================

/**
 * Start/End/Cancel Gcode
 * Gcode that runs after a print starts, ends, or canceled.
 * Enable Start/End/Cancel gcode in SETTINGS -> FEATURE menu.
 *
 * NOTE for users having a filament sensor connected to the mainboard:
 * 1) Enable the start/end gcode.
 * 2) Add the following commands to the start/end gcode:
 *    PRINT_START_GCODE: M75\n
 *    PRINT_END_GCODE: M77\n
 *
 *   Value range: [min: 3, max: 75 characters]
 */

// Start Gcode
// This gcode will runs before starting a print.
#define PRINT_START_GCODE "G28 XY R10\n"  // Raise Z 10mm before homing X & Y

// End Gcode
// This gcode will runs after a print is completed.
#define PRINT_END_GCODE "G90\nG1 E-4\nG92 E0\nM18\n"  // Switch to absolute positioning, reduce filament pressure by
                                                      // performing small retract, reset extruder position, disable steppers

// Cancel Gcode
// This gcode will runs when a print is canceled.
#define PRINT_CANCEL_GCODE "M104 S0\nM140 S0\nG28 XY R10\nM107\nM18\n"  // Home XY and raise Z 10mm

//====================================================================================================
//============================ Settings Configurable At Compile Time Only ============================
//====================================================================================================

//================================================================================
//========================== Developer / Debug Settings ==========================
//================================================================================

/**
 * Screenshot For Documentation
 * Long press touch screen capture the current full screen to SD card
 * Only for documentation purposes, DON'T enable it for normal printing.
 */
//#define SCREEN_SHOT_TO_SD  // Default: commented (disabled)

/**
 * If one of the DEBUG_x below is defined the SERIAL_DEBUG_PORT defined in board specific Pin_xx.h file
 * will be used for debugging purposes. DON'T enable one of the DEBUG_x below for normal printing.
 *
 * Warning: SERIAL_DEBUG_PORT needs to be activated specifically. Please consider settings MULTI_SERIAL > 0
 * in the Configuration.h or set e.g. "multi_serial: 2" in the config.ini.
 * If the TFT is only showing the boot logo and is not responding anymore, check the MULTI_SERIAL setting.
 * If you update the config.ini you need to reset the TFT *twice* (first reset to update the config, second
 * reset to enable the SERIAL_DEBUG_PORT due to the changed firmware config).
 */

/**
 * Generic Debug
 * Uncomment/Enable to enable arbitrary debug serial communication to SERIAL_DEBUG_PORT defined in board specific Pin_xx.h file.
 */
//#define DEBUG_SERIAL_GENERIC  // Default: commented (disabled)

/**
 * Serial Communication Debug
 * Uncomment/Enable to forward/dump all serial communication to SERIAL_DEBUG_PORT defined in board specific Pin_xx.h file.
 */
//#define DEBUG_SERIAL_COMM  // Default: commented (disabled)

/**
 * Config File Debug
 * Uncomment/Enable to show debug information during config file processing.
 */
//#define DEBUG_SERIAL_CONFIG  // Default: commented (disabled)

#if defined(DEBUG_SERIAL_GENERIC) || defined(DEBUG_SERIAL_CONFIG) || defined(DEBUG_SERIAL_COMM)
  #define SERIAL_DEBUG_ENABLED
#else
  #undef SERIAL_DEBUG_ENABLED
#endif

//================================================================================
//========================== Printer / Machine Settings ==========================
//================================================================================

// Mesh Leveling Max Grid Points (Mesh Editor)
// Set the maximum number of grid points per dimension.
#define MESH_GRID_MAX_POINTS_X 15  // (Minimum 1, Maximum 15)
#define MESH_GRID_MAX_POINTS_Y 15  // (Minimum 1, Maximum 15)

/**
 * Bed Leveling
 * The TFT will attempt to auto detect Bed Leveing type with marlin firmware.
 * Set to 1 to enable auto-detect (Marlin only) or choose one of the options
 * below if auto-detect does not work.
 *
 * WARNING: If you're not sure, leave on auto-detect or disabled.
 *          UBL has extra options other leveling systems might not have.
 *
 *   Options: [disable: 0, auto-detect: 1, ABL: 2, BBL: 3, UBL: 4, MBL: 5]
 */
#define ENABLE_BL_VALUE 1  // Default: 1

/**
 * MBL Settings
 * Apply the "level_z_pos" configurable parameter value as the
 * starting Z height for each point during MBL process.
 * If not enabled, you can set the desired starting Z height
 * in Marlin fw (MANUAL_PROBE_START_Z in Configuration.h).
 */
#define ENABLE_MBL_START_Z

/**
 * M601: Pause Print
 * PrusaSlicer can add M601 on certain height.
 * Acts here like manual pause.
 */
#define NOZZLE_PAUSE_M601

/**
 * M701, M702: Marlin Filament Load / Unload Gcodes Support
 * FILAMENT_LOAD_UNLOAD_GCODES option on Marlin configuration_adv.h need to be uncommented.
 * Adds a submenu to the movement menu for selecting load and unload actions.
 */
#define LOAD_UNLOAD_M701_M702

/**
 * Delta Probe Type
 * Probe type in case of a Delta printer.
 * Set to 0 in case of not Delta printer.
 *
 *   Options: [disable: 0, fixed-probe: 1, removable-probe: 2]
 */
#define DELTA_PROBE_TYPE 0  // Default: 0

//================================================================================
//================================= UI Settings ==================================
//================================================================================

/**
 * Default Primary Language (for Touch-Mode only)
 * Select the language to display on the LCD while in Touch Mode.
 *
 * NOTE: To add/flash a second language copy the required "language_xx.ini" file from
 *       "Language Packs" folder to the SD root folder.
 *       Then press the reset button to load/flash the copied language file.
 *
 *   Options: [ENGLISH,    CHINESE,  RUSSIAN,     JAPANESE,   ARMENIAN,  GERMAN,        CZECH,
 *             SPANISH,    FRENCH,   PORTUGUESE,  ITALIAN,    POLISH,    SLOVAK,        DUTCH,
 *             HUNGARIAN,  TURKISH,  GREEK,       SLOVENIAN,  CATALAN,   TRAD_CHINESE,  UKRAINIAN]
 */
#define DEFAULT_LANGUAGE ENGLISH  // Default: ENGLISH

/**
 * Rapid Serial Communication
 * More frequent Serial communicaiton while printing.
 * Send and parse G-codes more frequently while drawing on screen to
 * prevent printer idling and stuttering due to empty printer buffer.
 *
 * NOTE: This may slow down graphics while switching menus while printing.
*/
#define RAPID_SERIAL_COMM

/**
 * LCD/Touch Encoder
 * In case LCD/Touch Encoder's sliding buttons (pin LCD_ENCA_PIN and LCD_ENCB_PIN)
 * don't produce any movement on menu, try to increase the delay in ms (e.g. 64).
 */
#define LCD_ENCODER_DELAY 8  // Default: 8 (ms) (1 ms = 0.000001 seconds)

/**
 * Buzzer Settings
 * The duration and frequency for the UI feedback sound.
 * Set these to 0 to disable audio feedback in the LCD menus.
 * Only valid for SERIAL_TSC LCD MODE and if BUZZER_PIN is set or available.
 *
 * NOTE: Test audio output with the G-Code:
 *       M300 S<frequency Hz> P<duration MilliSeconds>
 */
#define BUZZER_FREQUENCY_DURATION_MS    20  // (MilliSeconds) Default: 20
#define BUZZER_FREQUENCY_HZ          10000  // (Hz) Default: 10000 (20Hz to 60000Hz)

/**
 * Buzzer Stop Level
 * Buzzer pin state when buzzer is idle or no sound is playing.
 *
 *   Options: [HIGH, LOW]
 */
#define BUZZER_STOP_LEVEL LOW  // Default: LOW

// Show bootscreen when starting up
#define SHOW_BTT_BOOTSCREEN

// Bootscreen logo time in ms
#define BTT_BOOTSCREEN_TIME 3000

/**
 * Smart Home
 * If enabled, long press "Back" button triggers Home screen
 * It doesn't interfere with the "Screenshot" and "Marlin/Touch Mode" other than if
 * enabled, long press "Back" will not trigger "Screenshot" or "Marlin/Touch Mode"
 */
#define SMART_HOME

/**
 * Alternative Move Menu Layout
 * Enable alternative Move Menu Buttons Layout matching the direction
 * of actual printer axis Update the icons from alternate icon folder.
 */
#define ALTERNATIVE_MOVE_MENU

/**
 * Friendly Z Offset Language
 * Replace decrease/increase and "-" & "+" icons with down/up and friendly icons.
 */
#define FRIENDLY_Z_OFFSET_LANGUAGE

/**
 * Quick EEPROM Menu
 * Enable EEPROM menu (save/load/reset buttons) in Settings > Machine Menu.
 *
 * NOTE: If disabled, EEPROM operations can also be accessed in the (settings > machine > parameters) menu.
 */
#define QUICK_EEPROM_BUTTON

/**
 * Toast Notification Duration (in MilliSeconds)
 * Set the duration for displaying toast notification on top of the screen.
 */
#define TOAST_DURATION (3 * 1000)  // (MilliSeconds)  1000 MilliSeconds = 1 Second

/**
 * Keyboard On Left Side (Mesh Editor, RGB Settings Custom)
 * By default the keyboard is drawn on right side of the screen.
 * Enable KEYBOARD_ON_LEFT to draw the keyboard on left side of the screen.
 */
//#define KEYBOARD_ON_LEFT

//
// Terminal Keyboard / Numpad Settings
//

/**
 * Terminal Keyboard / Numpad Theme
 * Uncomment to enable Material theme for keyboard and Numpad.
 */
#define KEYBOARD_MATERIAL_THEME  // Default: commented (disabled)

/**
 * Terminal Keyboard / Numpad Color Layout
 * Color layout for the Terminal Keyboard / Numpad.
 *
 *   Options: [Default: 0, Invert: 1, High Contrast: 2]
 *     Default:       The keyboard follows system background and border colors.
 *                    (Material Dark if KEYBOARD_MATERIAL_THEME is enabled).
 *     Invert:        The keyboard is drawn with background swapped with border colors.
 *                    (Material Light if KEYBOARD_MATERIAL_THEME is enabled).
 *     High Contrast: Use white and black colors for high contrast.
 */
#define KEYBOARD_COLOR_LAYOUT 0  // Default: 0

/**
 * Terminal Text Color Scheme
 * Color scheme for displaying text in Terminal menu.
 *
 *   Options: [Material Dark: 0, Material Light: 1, High Contrast: 2]
 *     Material Dark:  Dark background with light font color and orange command font color.
 *     Material Light: Light background with dark font color and orange command font color.
 *     High Contrast:  Black background with white font color and orange command font color.
 */
#define TERMINAL_COLOR_SCHEME 0  // Default: 0

/**
 * QWERTY/QWERTZ Keyboard Layout
 * Keyboard layout for Terminal Keyboard (Only for TFT70 V3.0).
 *
 *   Options: [qwerty: 0, qwertz: 1, azerty: 2]
 *     default: The keyboard has an alphabetically order.
 *     qwerty:  The typically keyboard Layout for english.
 *     qwertz:  The typically keyboard Layout for german.
 *     azerty:  The typically keyboard Layout for french.
 */
#define TERMINAL_KEYBOARD_LAYOUT 0  // Default: 0

/**
 * Progress Bar Layout (Printing menu)
 * Uncomment to enable a progress bar with 10% markers.
 * Comment to enable a standard progress bar.
 */
//#define MARKED_PROGRESS_BAR  // Default: commented (disabled)

/**
 * Live Text Background Color Rendering Technique (Printing menu)
 * Uncomment to enable the sampling and use of a uniform background color across all the icons.
 * Comment to enable a standard rendering based on the sampling and use, in a pixel by pixel basis,
 * of the underlying icon background colors.
 *
 * NOTE: Enable it only in case all the icons have the same and uniform background color under all
 *       the live text areas (e.g. applicable to Unified, Round Miracle etc... menu themes).
 *       If enabled, it speeds up the rendering of the live text and the responsiveness of the TFT,
 *       so it can improve the print quality.
 *       Suitable in particular for the TFTs with a not fast HW (e.g. 24, 48 MHz).
 */
//#define UNIFORM_LIVE_TEXT_BG_COLOR  // Default: commented (disabled)

/**
 * Show Embedded Thumbnails Of Gcode Files
 *
 * NOTE: "Base64 PNG" option utilizes about 43kb statically allocated RAM and about 1kb dynamically
 *       allocated RAM. Therefore this option is only suitable for devices >96KB RAM.
 *       If you choose "Base64 PNG" on such a low RAM device it will automatically downgraded to
 *       "RGB565 bitmap" option.
 *
 *   Options: [Classic: 0, RGB565 Bitmap: 1, Base64 PNG: 2]
 *     Classic:       RGB565 bitmaps for all possible thumbnail sizes are embedded in the gcode
 *                    file at fixed file offsets. It is fastest to parse but least flexible.
 *     RGB565 Bitmap: A specific thumbnail comment identifies the location of a single "Classic"
 *                    embedded RB565 bitmap thumbnail. It is almost as fast as classic and
 *                    flexible but requires a dedicated post-processing of gcode files for
 *                    most slicers. "Classic" is used as fallback.
 *     Base64 PNG:    A specific thumbnail comment identifies the location of a Base64-encoded
 *                    PNG thumbnail. It is slower as classic but most flexible. It does _not_
 *                    require dedicated post-processing of gcode files for most slicers.
 *                    "RGB565 bitmap" and "Classic" are used as fallback.
 */
#define THUMBNAIL_PARSER 0  // Default: 0

#endif
