#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_
#define CONFIG_VERSION 20210513

//===========================================================================
//============================ Developer Settings ===========================
//===========================================================================

/**
 * Screenshot for documentation
 * Long press touch screen capture the current full screen to SD card
 * Only for documentation purposes, DON'T enable it for normal printing.
 */
//#define SCREEN_SHOT_TO_SD

/**
 * Serial Communication Debug
 * Uncomment/Enable to forward/dump all serial communication to SERIAL_DEBUG_PORT defined in board specific Pin_xx.h file.
 * Use it only for debugging purposes. DON'T enable it for normal printing.
 */
//#define DEBUG_SERIAL_COMM

//===========================================================================
//============================= General Settings ============================
//===========================================================================

// Enable Status Screen
// Enable this to show status screen as the default home screen. Disabling it will show a static menu.
#define ENABLE_STATUS_SCREEN true  // To enabled: true | To disabled: false (Default: true)

/**
 * Smart Home
 * If enabled, long press "Back" button triggers Home screen
 * It doesn't interfere with the "Screenshot" and "Marlin/Touch Mode" other than if
 * enabled, long press "Back" will not trigger "Screenshot" or "Marlin/Touch Mode"
 */
#define SMART_HOME

/**
 * This setting determines the communication speed of the printer.
 * 250000 works in most cases, but you might try a lower speed if
 * you commonly experience drop-outs during host printing.
 * You may try up to 1000000 to speed up SD file transfer.
 *
 * Options: [2400: 0, 9600: 1, 19200: 2, 38400: 3, 57600: 4, 115200: 5, 250000: 6, 500000: 7, 1000000: 8]
 */
#define BAUDRATE 5  // Default: 5

/**
 * Default Primary Language (for Touch-Mode only)
 * Select the language to display on the LCD while in Touch Mode.
 *
 * NOTE: To add/flash a second language copy the required "language_xx.ini" file from
 *       "Language Packs" folder to the SD root folder. Then preset the reset button
 *       to load/flash the copied language file.
 *
 * Options: ENGLISH,    CHINESE,  RUSSIAN,     JAPANESE,   ARMENIAN,  GERMAN,        CZECH,
 *          SPANISH,    FRENCH,   PORTUGUESE,  ITALIAN,    POLISH,    SLOVAK,        DUTCH,
 *          HUNGARIAN,  TURKISH,  GREEK,       SLOVENIAN,  CATALAN,   TRAD_CHINESE,  UKRAINIAN
 */
#define DEFAULT_LANGUAGE ENGLISH  // Default: ENGLISH

/**
 * Default Touch Mode Color Options
 *
 * Options:  0: WHITE,      1: BLACK,       2: RED,      3: GREEN,     4: BLUE,   5: CYAN,
 *           6: MAGENTA,    7: YELLOW,      8: ORANGE,   9: PURPLE,   10: LIME,  11: BROWN,
 *          12: DARKBLUE,  13: DARKGREEN,  14: GRAY,    15: DARKGRAY
 */
#define TITLE_BACKGROUND_COLOR      1  // Title background color (Default: 1)
#define BACKGROUND_COLOR            1  // Background color (Default: 1)
#define FONT_COLOR                  0  // Font foreground color (Default: 0)
#define REMINDER_FONT_COLOR         2  // Reminder font color, such as: "No print attached", "Busy processing", etc. (Default: 2)
#define VOLUME_REMINDER_FONT_COLOR  5  // Volume reminder font color, such as: "Card inserted", "Card removed" (Default: 5)
#define STATUS_XYZ_BG_COLOR        15  // Background color for X Y Z position display in Status Screen. (Default: 15)
#define LISTVIEW_BORDER_COLOR      15  // Border color in List view (Default: 15)
#define LISTVIEW_ICON_COLOR        15  // icon color in List view (Default: 15)

// Mesh Leveling Display Colors
// Set the colors used for drawing the mesh with the minimun and maximum value in the grid.
#define MESH_MIN_COLOR 7  // Default: 7
#define MESH_MAX_COLOR 2  // Default: 2

//===========================================================================
//=========================== Marlin Mode Settings ==========================
//===========================================================================

/**
 * Default UI Mode
 * Mode switching is still possible by holding down the encorder for two seconds.
 *
 * Options: MODE_MARLIN (Marlin Mode)
 *          MODE_SERIAL_TSC (Touch Mode)
 */
#define DEFAULT_LCD_MODE MODE_SERIAL_TSC  // Default: MODE_SERIAL_TSC

/**
 * Default Marlin Mode Background & Font Color Options
 * These colors can be changed in Touch mode, but can also be set here.
 *
 * Options:  0: WHITE,      1: BLACK,       2: RED,      3: GREEN,     4: BLUE,   5: CYAN,
 *           6: MAGENTA,    7: YELLOW,      8: ORANGE,   9: PURPLE,   10: LIME,  11: BROWN,
 *          12: DARKBLUE,  13: DARKGREEN,  14: GRAY,    15: DARKGRAY
 */
#define MARLIN_BKCOLOR 1  // Default: 1
#define MARLIN_FNCOLOR 0  // Default: 0

// Text displayed at the top of the TFT in Marlin Mode
#define MARLIN_BANNER_TEXT "Marlin Mode"  // Default: "Marlin Mode"

// Show banner text at the top of the TFT in Marlin Mode
#define MARLIN_SHOW_BANNER true  // To enabled: true | To disabled: false (Default: true)

/**
 * Run Marlin Mode in Fullscreen
 *
 * Options: 0: Disabled (RECOMMENDED FOR TFT24)
 *          1: Enabled
 */
#define MARLIN_MODE_FULLSCREEN 0  // Default: 0

/**
 * Keep Serial always On (ONLY SUPPORTED ON TFT24 V1.1, TFT35 V3.0, AND TFT28 V3.0)
 * Keep UART (Serial communication) alive in Marlin Mode.
 * Allow seamless OctoPrint UART connection to the TFT's UART/serial expansion port no matter which
 * mode the TFT is in.
 *
 * Options: [0: Disabled, 1: Enabled]
 */
#define SERIAL_ALWAYS_ON 0  // Default: 0

/**
 * LCD/Touch Encoder
 * In case LCD/Touch Encoder's sliding buttons (pin LCD_ENCA_PIN and LCD_ENCB_PIN)
 * don't produce any movement on menu, try to increase the delay (e.g. 64).
 */
#define LCD_ENCODER_DELAY 8  // (MicroSeconds) 1 MicroSeconds = 0.000001 Seconds (Default: 8)

//===========================================================================
//=========================== Machine Settings ==============================
//===========================================================================

#define HOTEND_NUM      1  // set in 1~6
#define EXTRUDER_NUM    1  // set in 1~6
#define MIXING_EXTRUDER 0  // Default: 0. For mixing_extruder set to 1 (This option turns off autodetection
                           // of the number of extruders)
#define FAN_NUM         1  // set in 1~6
#define ENABLE_CTRL_FAN 1  // Set 0 to disable & 1 to enable controller fan speed control for Idle and Active
                           // cooling if marlin supports ontroller fan (M710).

#define PREHEAT_LABELS {"PLA", "PETG", "ABS", "WOOD", "TPU", "NYLON"}
#define PREHEAT_HOTEND {200,   240,    230,   170,    220,   250}
#define PREHEAT_BED    { 60,    70,     90,    50,     50,    90}

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
#define FAN_MAX_PWM    {255,  255,  255,  255,  255,  255,  255,   255}
#define FAN_DISPLAY_ID {"F0", "F1", "F2", "F3", "F4", "F5", "CtS", "CtI"}
#define FAN_CMD        {"M106 P0 S%d\n", "M106 P1 S%d\n", "M106 P2 S%d\n", "M106 P3 S%d\n", "M106 P4 S%d\n", "M106 P5 S%d\n", \
                        "M710 S%d\n",    "M710 I%d\n" }

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

// Size of machine
#define X_MIN_POS   0
#define Y_MIN_POS   0
#define Z_MIN_POS   0
#define X_MAX_POS 235
#define Y_MAX_POS 235
#define Z_MAX_POS 250

/**
 * Raised Z height for probing
 * Z height to raise / drop after homing (G28) before starting to probe a point.
 *
 * WARNING: It MUST be negative (e.g. -50mm) for a Delta printer to avoid crashing into the top of the tower.
 */
#define PROBING_Z_RAISE 20.0f

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
 * SD support
 * Starting from Marlin Bugfix 2.0.x Distribution Date: 2020-04-27 & above, the TFT will auto detect
 * On-Board SD Card and auto-configure M27 AutoReport with M115 command.
 * Set the time interval to poll SD Printing status if Marlin reports M27 AutoReport as disabled.
 */
#define M27_REFRESH             3     // Time in sec for M27 command
#define M27_WATCH_OTHER_SOURCES true  // if true the polling on M27 report is always active. Case: SD print
                                      // started not from TFT35

/**
 * Bed Leveling
 * The TFT will attempt to auto detect Bed Leveing type with marlin firmware.
 * Set to 1 to enable auto-detect (Marlin only) or choose one of the options below if auto-detect
 * does not work.
 *
 * WARNING: If you're not sure, leave on auto-detect or disabled.
 *          UBL has extra options other leveling systems might not have.
 *
 * Options: [0: Disabled, 1: Auto-detect, 2: ABL, 3: BBL, 4: UBL, 5: MBL]
 */
#define ENABLE_BL_VALUE 1  // Default: 1

/**
 * TouchMI settings (on ABL menu)
 * Enable this option for settings TouchMI sensor on ABL Menu (Init, Z Offset, Save, Test).
 *
 * Options: [0: Disabled, 1: Enabled]
 */
#define TOUCHMI_SENSOR_VALUE 0  // Default: 0

/**
 * MBL settings
 * Apply the "level_z_pos" configurable parameter value as the
 * starting Z height for each point during MBL process.
 * If not enabled, you can set the desired starting Z height
 * in Marlin fw (MANUAL_PROBE_START_Z in Configuration.h).
 */
#define ENABLE_MBL_START_Z

// Mesh Leveling Max Grid points
// Set the maximum number of grid points per dimension.
#define MESH_GRID_MAX_POINTS_X 15  // (Minimum 1, Maximum 15)
#define MESH_GRID_MAX_POINTS_Y 15  // (Minimum 1, Maximum 15)

/**
 * Auto save/load Bed Leveling data
 * The TFT will auto detect if BL data are available.
 * Enable this will send "M500" after "G29" to store leveling value
 * and send "M420 S1" to enable leveling state after startup.
 *
 * Options: [0: Disabled, 1: Enabled]
 */
#define AUTO_SAVE_LOAD_BL_VALUE 1  // Default: 1

// PID autotune
#define PID_CMD {"M303 U1 C8 E0", "M303 U1 C8 E1", "M303 U1 C8 E2", "M303 U1 C8 E3", "M303 U1 C8 E4", "M303 U1 C8 E5", "M303 U1 C8 E-1", ""}
#define PID_PROCESS_TIMEOUT (15 * 60000)  // (MilliSeconds, 1 minute = 60000 MilliSeconds)

/**
 * M600: Emulate M600
 * The TFT intercepts the M600 gcode (filament change) and emulates the logic instead of passing it to Marlin firmware.
 *
 * NOTE: Enable it, in case Marlin firmware does not properly support M600 on the mainboard.
 */
#define EMULATE_M600 true  // To enabled: true | To disabled: false (Default: true)

/**
 * M601: Pause Print
 * PrusaSlicer can add M601 on certain height.
 * Acts here like manual pause.
 */
#define NOZZLE_PAUSE_M601

/**
 * M701, M702: Marlin filament load unload gcodes support
 * FILAMENT_LOAD_UNLOAD_GCODES option on Marlin configuration_adv.h need to be uncommented.
 * Adds a submenu to the movement menu for selecting load and unload actions.
 */
#define LOAD_UNLOAD_M701_M702


//===========================================================================
//========================== Other UI Settings ==============================
//===========================================================================

// Show bootscreen when starting up
#define SHOW_BTT_BOOTSCREEN

// Bootscreen logo time in ms
#define BTT_BOOTSCREEN_TIME 3000

// Enable alternative Move Menu Buttons Layout matching the direction of actual printer axis
// Update the icons from alternate icon folder.
#define ALTERNATIVE_MOVE_MENU

/**
 * Friendly Z offset language
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
 * Startup Knob LED Color (TFT28 V3.0, TFT35 E3.0, TFT43 V3.0, TFT50 V3.0 & TFT70 V3.0)
 *
 * Options: 0: LED_OFF,     1: LED_WHITE,   2: LED_RED,
 *          3: LED_ORANGE,  4: LED_YELLOW,  5: LED_GREEN,
 *          6: LED_BLUE,    7: LED_INDIGO,  8: LED_VIOLET
*/
#define STARTUP_KNOB_LED_COLOR 1  // Default: 1

// Keep the LED state in Marlin Mode
#define KEEP_KNOB_LED_COLOR_MARLIN_MODE

/**
 * LCD Brightness and LCD IDLE Brightness (TFT28 V3.0, TFT35 E3.0, TFT43 V3.0, TFT50 V3.0 & TFT70 V3.0)
 *
 * Options: 0: OFF,          1:  5 Percent,   2: 10 Percent,
 *          3: 20 Percent,   4: 30 Percent,   5: 40 Percent,
 *          6: 50 Percent,   7: 60 Percent,   8: 70 Percent,
 *          9: 80 Percent,  10: 90 Percent,  11: 100 Percent
 */
#define DEFAULT_LCD_BRIGHTNESS       11  // Default: 11
#define DEFAULT_LCD_IDLE_BRIGHTNESS   3  // Default: 3

/**
 * Idle LCD diming Timer (TFT28 V3.0, TFT35 E3.0, TFT43 V3.0, TFT50 V3.0 & TFT70 V3.0)
 * The LCD screen will dim to idle brightness,
 * if the display is not touched for the period of the lcd idle timer.
 *
 * Options: 0: OFF,          1: 5 Seconds,      2: 10 Seconds,
 *          3: 30 Seconds,   4: 60 Seconds,     5: 120 Seconds,
 *          6: 300 Seconds,  7: CUSTOM Seconds
 */
#define DEFAULT_LCD_IDLE_TIMER  0  // Default: 0
#define LCD_DIM_CUSTOM_SECONDS (10 * 60)  // Custom value in seconds. This will be used if DEFAULT_LCD_IDLE_TIMER
                                          // is set to 7 (CUSTOM Seconds).

/**
 * Buzzer Settings
 * The duration and frequency for the UI feedback sound.
 * Set these to 0 to disable audio feedback in the LCD menus.
 * Only valid for SERIAL_TSC LCD MODE and if BUZZER_PIN is set or available.
 *
 * NOTE: Test audio output with the G-Code:
 *       M300 S<frequency Hz> P<duration MilliSeconds>
 */
#define BUZZER_FREQUENCY_DURATION_MS     20  // (MilliSeconds) Default: 20
#define BUZZER_FREQUENCY_HZ           10000  // (Hz) Default: 10000 (20Hz to 60000Hz)

/**
 * Buzzer pin state when buzzer is idle or no sound is playing
 *
 * Options: [HIGH, LOW]
 */
#define BUZZER_STOP_LEVEL LOW  // Default: LOW

/**
 * Toast notification duration (in MilliSeconds)
 * Set the duration for displaying toast notification on top of the screen.
 */
#define TOAST_DURATION (3 * 1000)  // (MilliSeconds)  1000 MilliSeconds = 1 Second

/**
 * Notification style for ACK messages
 * Set the notification style to use for displaying the ACK messages which start with 'echo:'.
 *
 * NOTE: The OFF value is applied to any ACK message type (e.g. even to known echo ACK).
 *       It means that any kind of ACK message is silently discard.
 *
 * Options: [0: OFF, 1: POPUP, 2: TOAST]
 *  OFF:   No notification. The message is ignored.
 *  POPUP: Display a popup window for user confirmation.
 *  TOAST: A Toast notification is displayed for few seconds. No user interaction is needed.
 */
#define ACK_NOTIFICATION_STYLE 1  // Default: 1

// Fan speed as a percentage instead of PWM value
#define SHOW_FAN_PERCENTAGE true  // To enabled: true | To disabled: false (Default: true)

/**
 * Keyboard on left side (Mesh Editor, RGB Settings Custom)
 * By default the keyboard is drawn on right side of the screen.
 * Enable KEYBOARD_ON_LEFT to draw the keyboard on left side of the screen.
 */
//#define KEYBOARD_ON_LEFT

//
// Terminal Keyboard / Numpad settings
//

// Terminal Keyboard / Numpad theme
// Uncomment to enable Material theme for keyboard and Numpad
#define KEYBOARD_MATERIAL_THEME  // Default: disabled

/**
 * Color scheme for the Terminal Keyboard / Numpad
 *
 * Options: [0: Default, 1: Invert, 2: High-Contrast]
 *  Default: The keyboard follows system background and border colors. (Material Dark
 *           if KEYBOARD_MATERIAL_THEME is enabled).
 *  Invert: The keyboard is drawn with background swapped with border colors. (Material
 *          Light if KEYBOARD_MATERIAL_THEME is enabled).
 *  High-Contrast: Use white and black colors for high contrast.
 */
#define KEYBOARD_COLOR_LAYOUT 0  // Default: 0

/**
 * Color scheme for the Terminal text display
 *
 * Options: [0: Material Dark, 1: Material Light, 2: High-Contrast]
 *  Material Dark: Dark background with light font color and orange command font color.
 *  Material Light: Light background with dark font color and orange command font color.
 *  High-Contrast: Black background with white font color and orange command font color.
 */
#define TERMINAL_COLOR_SCHEME 0  // Default: 0

/**
 * QWERTY/QWERTZ keyboard layout
 * Keyboard layout for Terminal Keyboard (Only for TFT70 V3.0).
 *
 * Options: [0: qwerty, 1: qwertz, 2: azerty]
 *  default: The keyboard has an alphabetically order.
 *  qwerty: The typically keyboard Layout for english.
 *  qwertz: The typically keyboard Layout for german.
 *  azerty: The typically keyboard Layout for french.
 */
#define TERMINAL_KEYBOARD_LAYOUT 0  // Default: 0

/**
 * Progress bar layout
 * Uncomment to enable progress bar with 10% markers.
 * Comment to enable standard progress bar.
 */
//#define MARKED_PROGRESS_BAR  // Default: disabled


//===========================================================================
//=========================== Other Settings ================================
//===========================================================================

// Power Supply
#define PS_ON_ACTIVE_HIGH true  // Set 'false' for ATX (1), 'true' for X-Box (2)

/**
 * Maximum hotend temperature for automatic shutdown after printing
 * If automatic shutdown(Auto Power) is enabled, the printer will shutdown automatically if the
 * hotend temperature is below this value. If the hotend temperature is higher than this value
 * the fans will be turned on to cool down and it will wait for the hotend temperature to drop below
 * this value before shutting down automatically.
 */
#define AUTO_SHUT_DOWN_MAXTEMP 50

//
// Filament Runout Settings (if connected to TFT controller)
//

// Filament runout detection
#define FIL_RUNOUT_INVERTING true  // Set to false to invert the logic of the sensor. (Default: true)
#define FIL_NOISE_THRESHOLD  100   // (MilliSeconds) Pause print when filament runout is detected atleast for this duration.

// Smart filament runout detection
// For use with an encoder disc that toggles runout pin as filament moves.
#define FILAMENT_RUNOUT_DISTANCE_MM 7

//
// Power Loss Recovery
// Most suitable for Delta printers since most printers will crash into printed model when homing after powerloss.
//

// Home before power loss recovery
#define HOME_BEFORE_PLR false  // To enabled: true | To disabled: false (Default: false)

// Backup power / UPS to move Z axis on power loss
#define BTT_MINI_UPS false  // To enabled: true | To disabled: false (Default: false)

// Raise Z axis on resume (on power loss with UPS)
#define POWER_LOSS_ZRAISE 10  // (mm) Default: 10

/**
 * Rapid Serial Communication
 * More frequent Serial communicaiton while printing.
 * Send and parse G-codes more frequently while drawing on screen to prevent printer idling and stuttering due to
 * empty printer buffer.
 *
 * NOTE: This may slow down graphics while switching menus while printing.
*/
#define RAPID_SERIAL_COMM

/**
 * Custom G-code Commands
 *
 * Support for up to 15 custom G-codes. Uncomment CUSTOM_*_LABEL and CUSTOM_*_GCODE to enable custom G-code.
 * CUSTOM_X_LABEL is the name of the custom button, CUSTOM_X_GCODE is the G-code to be sent by the custom button,
 * this should always end with a New-Line character '\n'.
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
 * Enable Start & End G-code in SETTINGS -> FEATURE menu.
 */
// Start G-code - run this G-code before starting print
#define PRINT_START_GCODE "G28 XY R10\n"  // Raise Z 10mm before homing X & Y

// End G-code - run this G-code after finishing print
#define PRINT_END_GCODE "G90\nG1 E-4\nG92 E0\nM18\n"  // Switch to absolute positioning, reduce filament pressure by
                                                      // performing small retract, reset extruder position, disable steppers

// Cancel G-code - run this G-code after canceling print
#define PRINT_CANCEL_GCODE "M104 S0\nM140 S0\nG28 XY R10\nM107\nM18\n"  // Home XY and raise Z 10mm

#endif
