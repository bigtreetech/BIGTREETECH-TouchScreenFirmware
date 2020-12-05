#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_
#define CONFIG_VERSION 20200810
//===========================================================================
//============================= General Settings ============================
//===========================================================================

/**
 * Default Mode
 *
 * Mode switching is still possible by holding down the encorder for two seconds.
 *
 * Options:
 * ST7920_SPI // Marlin Mode
 * SERIAL_TSC // Touch Mode
 */
#define DEFAULT_LCD_MODE SERIAL_TSC

/**
 * Default Startup Knob LED Color (TFT35 E3)
 *
 * Options:  0: LED_OFF,     1: LED_WHITE,   2: LED_RED,
 *           3: LED_ORANGE,  4: LED_YELLOW,  5: LED_GREEN,
 *           6: LED_BLUE,    7: LED_INDIGO,  8: LED_VIOLET
 *
 * Default value is: 1 for LED_OFF
*/
#define STARTUP_KNOB_LED_COLOR 0         // LED_OFF
#define KEEP_KNOB_LED_COLOR_MARLIN_MODE  // Keeps the LED state in Marlin Mode

/**
 * Default LCD Brightness and LCD IDLE Brightness
 *
 * Brightness:  0: LCD_0_PERCENT,    1: LCD_5_PERCENT,   2: LCD_10_PERCENT,
 *              3: LCD_20_PERCENT,   4: LCD_30_PERCENT,  5: LCD_40_PERCENT,
 *              6: LCD_50_PERCENT,   7: LCD_60_PERCENT,  8: LCD_70_PERCENT,
 *              9: LCD_80_PERCENT,  10: LCD_90_PERCENT, 11: LCD_100_PERCENT
 *
 * Settings for the idle dim timer. The LCD screen will dim to idle brightness,
 * if the display is not touched for the period of the lcd idle timer.
 *
 * Idle Timer:  0: LCD_DIM_OFF,          1: LCD_DIM_5_SECONDS.      2: LCD_DIM_10_SECONDS,
 *              3: LCD_DIM_30_SECONDS,   4: LCD_DIM_60_SECONDS,     5: LCD_DIM_120_SECONDS,
 *              6: LCD_DIM_300_SECONDS,  7: LCD_DIM_CUSTOM_SECONDS
 *
 */
#define DEFAULT_LCD_BRIGHTNESS      11  // 11: LCD_100_PERCENT - Brightness value from list
#define DEFAULT_LCD_IDLE_BRIGHTNESS 3   // 3: LCD_10_PERCENT - Brightness value from list
#define DEFAULT_LCD_IDLE_TIMER      0   // 0: LCD_DIM_OFF
#define LCD_DIM_CUSTOM_SECONDS      600 // Custom value in seconds. Will be used if LCD_DIM_CUSTOM_SECONDS is set as idle timer.

//===========================================================================
//=========================== Marlin Mode Settings ==========================
//===========================================================================

/**
 * Default Marlin Mode Background & Font Color Options
 *
 * These colors can be changed in Touch mode, but can also be set here.
 *
 * Options: 0: WHITE,      1: BLACK,       2: RED,     3: GREEN,     4: BLUE,   5: CYAN,
 *          6: MAGENTA,    7: YELLOW,      8: ORANGE,  9: PURPLE,   10: LIME,  11: BROWN,
 *         12: DARKBLUE,  13: DARKGREEN,  14: GRAY,   15: DARKGRAY
 */
#define MARLIN_BKCOLOR 1
#define MARLIN_FNCOLOR 0

/**
 * Text displayed at the top of the TFT in Marlin Mode.
 */
#define MARLIN_BANNER_TEXT "LCD12864 Simulator"

/**
 * show banner text at the top of the TFT in Marlin Mode.
 */
#define MARLIN_SHOW_BANNER  true //to enabled: true | to disabled: false
/**
 * Run Marlin Mode in Fullscreen
 *
 * Options:  0: Disabled. RECOMMENDED FOR TFT24
 *           1: Enabled Marlin Fullscreen mode.
 */
#define DEFAULT_ST7920_FULLSCREEN_MODE 0 // 0: Disabled. RECOMMENDED FOR TFT24

/**
 * Keep Serial always On (ONLY SUPPORTED ON TFT24 V1.1, TFT35 V3.0, AND TFT28 V3.0)
 *
 * Keep UART (Serial communication) alive in Marlin Mode
 *
 * Allow seamless OctoPrint UART connection to the TFT's UART/serial expansion port no matter which mode the TFT is in.
 *
 * Options:  0: Disabled    1: Enabled
 */
#define SERIAL_ALWAYS_ON 0  // Default: 0 (Disabled)

//===========================================================================
//========================== Touch Mode Settings ============================
//===========================================================================

/**
 * This setting determines the communication speed of the printer.
 * 250000 works in most cases, but you might try a lower speed if
 * you commonly experience drop-outs during host printing.
 * You may try up to 1000000 to speed up SD file transfer.
 *
 * :[2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000]
 */
#define BAUDRATE 115200

/**
 * Default Primary Language (for Touch-Mode only)
 * Select the language to display on the LCD while in Touch Mode.
 * Options: ENGLISH, CHINESE, RUSSIAN, JAPANESE, ARMENIAN, GERMAN, CZECH,
 *          SPANISH, FRENCH, PORTUGUESE, ITALIAN, POLISH, SLOVAK, DUTCH,
 *          HUNGARIAN, TURKISH, GREEK, SLOVENIAN, CATALAN, TRAD_CHINESE,
 *          UKRAINIAN,
 */
#define DEFAULT_LANGUAGE ENGLISH

/**
 * To add/flash a second language copy the required "language_xx.ini" file from
 * "Language Packs" folder to the SD root folder. Then preset the reset button
 * to load/flash the copied language file.
*/

/**
 * Show bootscreen when starting up
 */
#define SHOW_BTT_BOOTSCREEN

/**
 * The duration and frequency for the UI feedback sound.
 * Set these to 0 to disable audio feedback in the LCD menus.
 * Only valid for SERIAL_TSC LCD MODE and if BUZZER_PIN is set or available.
 *
 * Note: Test audio output with the G-Code:
 * M300 S<frequency Hz> P<duration ms>
 */
#define BUZZER_FREQUENCY_DURATION_MS 20 // Default 20
#define BUZZER_FREQUENCY_HZ 10000       // Default 10000, 20Hz to 60KHz
#define BUZZER_STOP_LEVEL false

/**
 * Toast notification duration (in ms)
 * Set the duration for displaying toast notification on top of the screen
 */
#define TOAST_DURATION 3000

/**
 * Notification style for ACK messages
 * Set the notification style to use for displaying the ACK messages which start with 'echo:'
 *
 * NOTE: The OFF value is applied to any ACK message type (e.g. even to known echo ACK).
 *      It means that any kind of ACK message is silently discard
 *
 * Options: [OFF: 0, POPUP: 1, TOAST: 2]
 *  OFF:   No notification. The message is ignored.
 *  POPUP: Display a popup window for user confirmation.
 *  TOAST: A Toast notification is displayed for few seconds. No user interaction is needed
 *
 */
#define ACK_NOTIFICATION_STYLE 1

/**
 * Default Touch Mode Color Options
 * Options: 0: WHITE,      1: BLACK,       2: RED,     3: GREEN,     4: BLUE,   5: CYAN,
 *          6: MAGENTA,    7: YELLOW,      8: ORANGE,  9: PURPLE,   10: LIME,  11: BROWN,
 *         12: DARKBLUE,  13: DARKGREEN,  14: GRAY,   15: DARKGRAY
 */
#define TITLE_BACKGROUND_COLOR     1  // Title background color // 0xD928
#define BACKGROUND_COLOR           1  // Background color // 0x0A29
#define FONT_COLOR                 0  // Font foreground color
#define REMINDER_FONT_COLOR        2  // Reminder font color, such as: "No print attached", "Busy processing", etc.
#define VOLUME_REMINDER_FONT_COLOR 5  // Volume reminder font color, such as: "Card inserted", "Card removed"
#define STATUS_XYZ_BG_COLOR        15 // Background color for X Y Z position display in Status Screen.
#define LISTVIEW_BORDER_COLOR      15 // Border color in List view
#define LISTVIEW_ICON_COLOR        15 // icon color in List view

#define HOTEND_NUM   1    // set in 1~6
#define EXTRUDER_NUM 1    // set in 1~6
#define FAN_NUM      1    // set in 1~6
#define FAN_CTRL_NUM 0    // set in 1~2
#define MIXING_EXTRUDER 0 // set default 0, for mixing_extruder 1 (this option turns off autodetection of the number of extruders)

#define PREHEAT_LABELS   {"PLA", "PETG", "ABS", "WOOD", "TPU", "NYLON"}
#define PREHEAT_HOTEND   {200,   240,    230,   170,    220,   250}
#define PREHEAT_BED      {60,    70,     90,    50,     50,    90}

#define HEAT_MAX_TEMP    {275,       275,       275,       275,       275,       275,       150,    60}   //max temperature can be set
#define HEAT_SIGN_ID     {"T0:",     "T1:",     "T2:",     "T3:",     "T4:",     "T5:",     "B:",   "C:"}
#define HEAT_DISPLAY_ID  {"T0",      "T1",      "T2",      "T3",      "T4",      "T5",      "Bed",  "Chamber"}
#define HEAT_CMD         {"M104 T0", "M104 T1", "M104 T2", "M104 T3", "M104 T4", "M104 T5", "M140", "M141"};
#define HEAT_WAIT_CMD    {"M109 T0", "M109 T1", "M109 T2", "M109 T3", "M109 T4", "M109 T5", "M190", "M191"};

#define TOOL_CHANGE      {"T0",   "T1",      "T2",      "T3",      "T4",      "T5"}
#define EXTRUDER_ID      {"E0",   "E1",      "E2",      "E3",      "E4",      "E5"}

/**
 * Fan control
 *
 * Fan type Options:
 *               0: FAN_TYPE_F       - default cooling fan speed (Check Marlin GCode M106)
 *               1: FAN_TYPE_CTRL_S  - Controller fan speed for stepper or hot bed ON (Check Marlin GCode M710)
 *               2: FAN_TYPE_CTRL_I  - Controller fan idle speed  (Check Marlin gcode - M710)
 *               8: FAN_TYPE_UNKNOWN - Unknown / Not defined
 */
#define FAN_MAX_PWM      {       255,       255,       255,       255,       255,       255,       255,       255 };
#define FAN_DISPLAY_ID   {      "F0",      "F1",      "F2",      "F3",      "F4",      "F5",     "CtL",     "CtI" };
#define FAN_CMD          { "M106 P0", "M106 P1", "M106 P2", "M106 P3", "M106 P4", "M106 P5",    "M710",    "M710" };
#define FAN_TYPE         {         0,         0,         0,         0,         0,         0,         1,         2 };

#define SPEED_ID         {"Sp.", "Fr."}

#define AXIS_DISPLAY_ID  {"X",  "Y",  "Z",   "E",  "E2"}

// Default move speed mm/min
#define DEFAULT_SPEED_MOVE   3000
#define SPEED_MOVE_SLOW      1000
#define SPEED_MOVE_FAST      5000

// Extrude speed mm/min
#define EXTRUDE_SLOW_SPEED   60
#define EXTRUDE_NORMAL_SPEED 600
#define EXTRUDE_FAST_SPEED   1200

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

/* M600, M601 ; pause print
 * PrusaSlicer can add M601 on certain height.
 * Acts here like manual pause
 */
#define NOZZLE_PAUSE_M600_M601

/* M701, M702 ; Marlin filament load unload gcodes support
 * FILAMENT_LOAD_UNLOAD_GCODES option on Marlin configuration_adv.h need to be uncommented
 * Adds a submenu to the movement menu for selecting load and unload actions
 */
#define LOAD_UNLOAD_M701_M702

/**
 * Auto save/load Bed Leveling data
 * The TFT will auto detect if BL data are available.
 * Enable this will send "M500" after "G29" to store leveling value
 * and send "M420 S1" to enable leveling state after startup
 *
 * Options:  0: Disabled    1: Enabled
 */
#define AUTO_SAVE_LOAD_BL_VALUE 1

/**
 * Enable Bed Leveling options
 * Will attempt to auto detect and/or enable specific BL options.
 *
 * WARNING - If you're not sure, leave on auto-detect or disabled.
 *           UBL has extra options other leveling systems might not have.
 *
 * Options:  0: Disabled    1: Auto-detect [default]    2: ABL    3: BBL    4: UBL    5: MBL
 *
 */
#define ENABLE_BL_VALUE 1

/**
 * Enable friendly probe offset language
 *
 * Decrease/increase and "-" & "+" icons are replaced with down/up and friendly icons
 *
 * Options:  0: Disabled    1: Enabled
 */
#define FRIENDLY_PROBE_OFFSET_LANGUAGE 1

/**
 * Enable quick EEPROM save/load/reset button
 *
 * This will enable a handy button in the (settings > machine) menu
 *
 * Note: if disabled, EEPROM operations can also be accessed in the (settings > machine > parameters) menu
 *
 * Options:  0: Disabled    1: Enabled
 */
#define QUICK_EEPROM_BUTTON 1

/**
 * Manual Leveling
 * Move to four corner points to Leveling manually (Point 1, Point 2, Point 3, Point 4)
 */
#define LEVELING_EDGE_DISTANCE     20    // Inset distance from bed's edge for calculating leveling point location.
#define LEVELING_POINT_Z           0.2f  // Z-axis position when nozzle stays for leveling
#define LEVELING_POINT_MOVE_Z      10.0f // Z-axis position when nozzle move to next point
#define LEVELING_POINT_XY_FEEDRATE 6000  // (mm/min) X and Y axes move feedrate
#define LEVELING_POINT_Z_FEEDRATE  600   // (mm/min) Z axis move feedrate

#define LEVELING_EDGE_DISTANCE_DISPLAY_ID   "X/Y"
#define LEVELING_EDGE_DISTANCE_MIN          0
#define LEVELING_EDGE_DISTANCE_MAX          100
#define LEVELING_EDGE_DISTANCE_DEFAULT      LEVELING_EDGE_DISTANCE

/**
 * Popup
 */
#define POPUP_NOTIFICATION_DURATION 3000;                  // expressed in ms. E.g. 1500 corresponds to 1.5 seconds

/**
 * Z Fade
 */
#define Z_FADE_MIN_VALUE     0.0f
#define Z_FADE_MAX_VALUE     20.0f
#define Z_FADE_DEFAULT_VALUE 10.0f

/**
 * Probe Offset
 */
#define PROBE_OFFSET_MIN_VALUE     -20.0f
#define PROBE_OFFSET_MAX_VALUE     20.0f
#define PROBE_OFFSET_DEFAULT_VALUE 0.0f

/**
 * Home Offset
 */
#define HOME_Z_OFFSET_MIN_VALUE     -20.0f
#define HOME_Z_OFFSET_MAX_VALUE     20.0f
#define HOME_Z_OFFSET_DEFAULT_VALUE 0.0f

/**
 * Babystep
 */
#define BABYSTEP_MIN_VALUE     -5.0f
#define BABYSTEP_MAX_VALUE     5.0f
#define BABYSTEP_DEFAULT_VALUE 0.0f
#define BABYSTEP_MAX_UNIT      1.0f

/**
 * Mesh Editor settings
 */

// Set the maximum number of grid points per dimension
//
// NOTE: It must be in range 1 - 15
//
#define MESH_GRID_MAX_POINTS_X 10
#define MESH_GRID_MAX_POINTS_Y 10

// Set the color used for drawing the mesh with the minimun and maximum value in the grid
//
// Options: 0: WHITE,      1: BLACK,       2: RED,     3: GREEN,     4: BLUE,   5: CYAN,
//          6: MAGENTA,    7: YELLOW,      8: ORANGE,  9: PURPLE,   10: LIME,  11: BROWN,
//         12: DARKBLUE,  13: DARKGREEN,  14: GRAY,   15: DARKGRAY
//
#define MESH_MIN_COLOR 7
#define MESH_MAX_COLOR 2

// Enable keyboard drawn on left side
//
// If disabled, the keyboard is drawn on right side
//
// Options:  0: Disabled    1: Enabled
//
#define MESH_LEFT_KEYBOARD 0

/**
 * Terminal settings
 */

// Enable keyboard color layout for the Terminal menu
//
// If standard, the keyboard is drawn using the standard configurable background and font colors.
// If alternative, the keyboard is drawn using the alternative embedded colors (white and black colors).
//
// Options:  0: standard 1    1: standard 2    2: alternative
//
#define TERMINAL_KEYBOARD_COLOR_LAYOUT 0

// Enable keyboard QWERTY layout for the Terminal menu with at least 10 columns
//
// If enabled, the QWERTY keyboard layout is used
//
// Options:  0: Disabled    1: Enabled
//
#define TERMINAL_KEYBOARD_QWERTY_LAYOUT 1

/**
 * PID autotune
 */
#define PID_CMD             {"M303 U1 C8 E0", "M303 U1 C8 E1", "M303 U1 C8 E2", "M303 U1 C8 E3", "M303 U1 C8 E4", "M303 U1 C8 E5", "M303 U1 C8 E-1", ""};
#define PID_PROCESS_TIMEOUT 900000                         // expressed in ms. E.g. 900000 corresponds to 15 minutes

// LCD Encoder
// In case LCD Encoder's sliding buttons (pin LCD_ENCA_PIN and LCD_ENCB_PIN)
// don't produce any movement on menu, try to increase the delay (e.g. 64)
#define LCD_ENCODER_DELAY 8                                // expressed in us. E.g. 8 corresponds to 0,000008 seconds

// Power Supply
#define PS_ON_ACTIVE_HIGH    true   // Set 'false' for ATX (1), 'true' for X-Box (2)

// Filament runout detection
#define FIL_RUNOUT_INVERTING true  // Set to false to invert the logic of the sensor.
#define FIL_NOISE_THRESHOLD  100   // 100ms,  Pause print when filament runout is detected for 100ms.

// Smart filament runout detection
// For use with an encoder disc that toggles runout pin as filament moves
#define FILAMENT_RUNOUT_DISTANCE_MM 7

// Enable alternative Move Menu Buttons Layout matching the direction of actual printer axis.
// update the icons from alternate icon folder
#define ALTERNATIVE_MOVE_MENU

// Enable Unified Move Menu
// Move, Home, Extrude, ABL at one Place and bring G-code Menu on Home Menu
#define UNIFIED_MENU true //for unified menu: true | for classic menu: false

/**
 * SD support
 * Starting from Marlin Bugfix 2.0.x Distribution Date: 2020-04-27 & above, The TFT will auto detect
 * On-Board SD Card and auto-configure M27 AutoReport with M115 command
 * Set the time interval to poll SD Printing status if Marlin reports M27 AutoReport as disabled.
 */
#define M27_REFRESH                3        // Time in sec for M27 command
#define M27_WATCH_OTHER_SOURCES    true     // if true the polling on M27 report is always active. Case: SD print started not from TFT35

/**
 * Power Loss Recovery
 *
 * Most suitable for Delta printers since most printers will crash into printed model when homing after powerloss.
 */
// Home before power loss recovery
#define HOME_BEFORE_PLR false   //to enabled: true | to disabled: false

// Backup power / UPS to move Z axis on power loss
#define BTT_MINI_UPS    false   //to enabled: true | to disabled: false

// (mm) Raise Z axis on resume (on power loss with UPS)
#define POWER_LOSS_ZRAISE 10

// Prevent extrusion if the temperature is below set temperature
#define PREVENT_COLD_EXTRUSION_MINTEMP 170

/**
 * Maximum hotend temperature of automatic shut down after printing.
 * When enable automatic shutdown(Auto Power), when the hotend temperature is higher than this value
 * turn on the fan to cool down, wait for the hotend temperature to be lower than this value, then turn off the power automatically
 */
#define AUTO_SHUT_DOWN_MAXTEMP 50

#define SHOW_FAN_PERCENTAGE true // enable to show fan speed as a percentage instead of a value. to enabled: true | to disabled: false

/**
 * Rapid Serial Communication
 *
 * More frequent Serial communicaiton while printing.
 *
 * Send and parse G-codes more frequently while drawing on screen to prevent printer idling and stuttering due to empty printer buffer.
 *
 * Note: This may slow down graphics while switching menus while printing.
*/
#define RAPID_SERIAL_COMM

/**
 * Custom G-code Commands
 *
 * Support for up to 15 custom G-codes. Uncomment CUSTOM_*_LABEL and CUSTOM_*_GCODE to enable custom G-code.
 *
 * CUSTOM_X_LABEL is the name of the custom button, CUSTOM_X_GCODE is the G-code to be sent by the custom button, this should always end with a New-Line character '\n'
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

/**
 * Start/End/Cancel G-code
 *
 * G-code that runs after a print starts, ends, or canceled.
 *
 * Enable Start & End G-code in SETTINGS -> FEATURE menu.
 */
// Start G-code - run this G-code before starting print
#define PRINT_START_GCODE "G28 XY R10\n" // Raise Z 10mm before homing X & Y

// End G-code - run this G-code after finishing print
#define PRINT_END_GCODE "G90\nG1 E-4\nG92 E0\nM18\n" // Switch to absolute positioning, reduce filament pressure by performing small retract, reset extruder position, disable steppers

// Cancel G-code - run this G-code after canceling print
#define PRINT_CANCEL_GCODE "G28 XY R10\n" // Home XY and raise Z 10mm

#endif
