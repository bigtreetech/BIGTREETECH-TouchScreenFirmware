#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define CONFIG_VERSION 20211213

//====================================================================================================
//=============================== Settings Configurable On config.ini ================================
//====================================================================================================

//================================================================================
//=============================== General Settings ===============================
//================================================================================

/**
 * Serial Ports (Primary and Supplementary)
 * Serial ports connected to devices such as Printer, ESP3D, OctoPrint, other TFTs etc.
 * In order to successfully establish a communication through a serial port, set a baudrate
 * matching the baudrate configured on the connected device.
 * Disable the serial port when it is not in use and/or not connected to a device (floating) to
 * avoid to receive and process wrong data due to possible electromagnetic interference (EMI).
 *
 * NOTES:
 *   - Serial port P1 is the primary serial connection to the printer and cannot be disabled.
 *   - A baudrate of 250000 works in most cases, but you might try a lower speed if you
 *     commonly experience drop-outs during host printing.
 *     You may try up to 1000000 to speed up SD file transfer.
 *
 *   Format: [serial_port: P1:<baudrate> P2:<baudrate> P3:<baudrate> P4:<baudrate>
 *   Target port: P1: Printer
 *                P2: WIFI    (e.g. ESP3D)
 *                P3: UART 3  (e.g. OctoPrint)
 *                P4: UART 4
 *   Value range: P1: [min: 1, max: 9]
 *                P2: [min: 0, max: 9]
 *                P3: [min: 0, max: 9]
 *                P4: [min: 0, max: 9]
 *   Options: [OFF (port disabled): 0, 2400: 1, 9600: 2, 19200: 3, 38400: 4, 57600: 5, 115200: 6, 250000: 7, 500000: 8, 1000000: 9]
 */
#define SP_1 6  // Default: 6
#define SP_2 0  // Default: 0
#define SP_3 0  // Default: 0
#define SP_4 0  // Default: 0

/**
 * Emulated M600
 * The TFT intercepts the M600 G-code (filament change) and emulates the handling logic
 * otherwise provided by Marlin firmware.
 *
 * NOTE: Enable it in case Marlin firmware does not properly support M600 on the mainboard.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define EMULATED_M600 1  // Default: 1

/**
 * Emulated M109 And M190
 * The TFT intercepts the blocking M109 and M190 G-codes (set target hotend and bed temperatures)
 * and converts them to the non blocking M104 and M140 G-codes respectively.
 *
 * NOTE: Enable it so the TFT can still communicate with Marlin firmware even if the target
 *       temperature is not reached yet. Otherwise the communication (TFT<->Marlin) will be
 *       frozen until desired/set temperatures are obtained.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define EMULATED_M109_M190 1  // Default: 1

/**
 * Event LED
 * When printing from TFT SD / TFT U Disk, the TFT periodically sets the printer's (neopixel) LED color
 * and TFT's knob LED color, if any, according to the current nozzle and bed temperatures.
 *
 * NOTE: If "EMULATED_M109_M190" is disabled (heating controlled by printer), the TFT cannot control the
 *       printer's (neopixel) LED during heating. It will control only the TFT's knob LED, if any.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define EVENT_LED 1  // Default: 1

/**
 * G-code File Comment Parsing
 * The TFT parses and processes extra information provided by the slicer as comments in the G-code file.
 * If enabled, the current implementation parses and processes print time and print layer information
 * from the G-code file (nothing else).
 * If disabled, the "layer_disp_type" setting provided in "UI Settings" section becomes redundant.
 *
 * NOTE: Enable it in case the slicer (e.g. Cura) supports extra information.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define FILE_COMMENT_PARSING 1  // Default: 1

//================================================================================
//================================= UI Settings ==================================
//================================================================================

/**
 * Rotated UI
 * Rotate UI by 180 degrees.
 *   Options: [disable: 0, enable: 1]
 */
#define ROTATED_UI 0  // Default: 0

/**
 * Touch Mode Language
 * Select the language to use on the LCD while in Touch Mode.
 *
 * NOTE: To add/flash a second language copy the required "language_xx.ini" file from
 *       "Language Packs" folder to the SD root folder.
 *       Then press the reset button to load/flash the copied language file.
 *
 *   Options: [Primary Language (english): 0, Secondary Language: 1]
 */
#define LANGUAGE 0  // Default: 0

/**
 * Status Screen
 * Select the default home screen while in Touch Mode.
 * If enabled, the Status Screen menu will become the default home screen.
 * If disabled, the Main menu will become the default home screen.
 *
 * NOTE: Both the Status Screen and Main menus display the current temperature, fan and speeds.
 *       Furthermore, the Status Screen menu provides the status area reporting the printer notifications.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define STATUS_SCREEN 1  // Default: 1

/**
 * Touch Mode Colors
 * Set colors used in Touch Mode.
 *
 * NOTE: Select an option from the provided list or set the color (RGB888 format) hex value directly
 *       (start with "0x"), such as: Red: 0xFF0000, Green: 0x00FF00, Blue: 0x0000FF.
 *
 *   Options: [ WHITE: 0,  BLACK: 1,  RED: 2,  GREEN: 3,      BLUE: 4,       CYAN: 5,  MAGENTA: 6,    YELLOW: 7,
 *             ORANGE: 8, PURPLE: 9, LIME: 10, BROWN: 11, DARKBLUE: 12, DARKGREEN: 13,    GRAY: 14, DARKGRAY: 15]
 */
#define TITLE_BACKGROUND_COLOR  1  // Title background color (Default: 1)
#define MENU_BACKGROUND_COLOR   1  // Menu background color (Default: 1)
#define MENU_FONT_COLOR         0  // Menu font color (Default: 0)
#define REMINDER_FONT_COLOR     2  // Reminder font color, such as: "No print attached", "Busy processing", etc. (Default: 2)
#define STATUS_FONT_COLOR       5  // Status (e.g. volume reminder, ABL probing point etc.) font color, such as: "Card inserted", "Card removed" (Default: 5)
#define STATUS_XYZ_BG_COLOR    15  // Background color for X Y Z position display in Status Screen menu (Default: 15)
#define LIST_BORDER_COLOR      15  // List View border color (Default: 15)
#define LIST_BUTTON_BG_COLOR   15  // List View button background color (Default: 15)

// Mesh Leveling Display Colors (Mesh Editor)
// Set the colors used for drawing the mesh with the minimum and maximum value in the grid.
#define MESH_MIN_COLOR 7  // Default: 7
#define MESH_MAX_COLOR 2  // Default: 2

/**
 * Terminal Text Color Scheme
 * Color scheme for displaying text in Terminal menu.
 *   Options: [Material Dark: 0, Material Light: 1, High Contrast: 2]
 *     Material Dark:  Dark background with light font color and orange command font color.
 *     Material Light: Light background with dark font color and orange command font color.
 *     High Contrast:  Black background with white font color and orange command font color.
 */
#define TERMINAL_COLOR_SCHEME 0  // Default: 0

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
#define ACK_NOTIFICATION 1  // Default: 1

/**
 * Files Sorting
 * Sort files and folders based on the selected option.
 *
 * NOTE: Only applicable for files in TFT SD Card and TFT USB Stick.
 *
 *   Options: [Date Newest First: 0, Date Oldest First: 1, Name Ascending: 2, Name Descending: 3]
 */
#define FILES_SORT_BY 0  // Default: 0

/**
 * Files List Mode
 * Display files in list mode instead of icon mode.
 *   Options: [disable: 0, enable: 1]
 */
#define FILES_LIST_MODE 1  // Default: 1

/**
 * Filename Extension
 * Display fullname for files listed in List Mode / Icon Mode menu.
 * If disabled, any filename extension starting with ".g" or ".G" (e.g. ".g", ".gco", ".gcode" etc.) will be hidden.
 *   Options: [disable: 0, enable: 1]
 */
#define FILENAME_EXTENSION 1  // Default: 1

/**
 * Fan Speed In Percentage
 * Show fan speed in percentage. If disabled fan speed will be displayed as PWM values.
 *   Options: [disable: 0, enable: 1]
 */
#define FAN_SPEED_PERCENTAGE 1  // Default: 1

/**
 * Persistent Temperature Info
 * Show persistent temperature info in all menus.
 *   Options: [disable: 0, enable: 1]
 */
#define PERSISTENT_INFO 0  // Default: 0

/**
 * Temperature ACK In Terminal
 * Show temperature ACK in Terminal menu.
 *   Options: [disable: 0, enable: 1]
 */
#define TERMINAL_ACK 0  // Default: 0

/**
 * Notification M117
 * If enabled, any notification received from Marlin through "//action:notification" is also
 * stored on the notification screen. The notification screen reporting the history of the
 * received notifications is displayed pressing on the notification bar.
 *
 * NOTE: Marlin notifications are also always displayed on the Status Screen menu.
 *       Furthermore, they are also displayed on the notification bar as toast messages
 *       in case the current menu is not the Status Screen menu.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define NOTIFICATION_M117 0  // Default: 0

/**
 * Progress Numeric Display Mode During Print
 * This sets the default display type for print progress numeric display. It can be changed during
 * print by pressing the hourglass icon.
 * At each click it will alter between the 3 variants.
 *
 * NOTE: Some slicers include time related info in the G-code files as comments. If the time info
 *       is not added by the slicer, then a post processing plugin is needed that adds that info in
 *       the G-code file (comment or M73 Rxx format).
 *       If no remaining time info is present in the G-code file (comment or M73 Rxx), the display
 *       defaults to option 0.
 *
 *   Options: [Percentage & Elapsed time: 0, Percentage & Remaining time: 1, Elapsed time & Remaining time: 2]
 */
#define PROG_DISP_TYPE 2  // Default: 2

/**
 * Current Layer Display Mode During Print
 * This sets the default display type for the printing layer. It can be changed during print by
 * pressing the nozzle icon. At each click it will alter between the 3 variants.
 *
 * NOTES:
 *   - It requires "FILE_COMMENT_PARSING" to be enabled.
 *   - This feature uses the layer number comments added by slicers at the starting of each layer.
 *   - Some slicers may not include the total number of layers in the G-code file. In this case only
 *     the current layer will be displayed. To display total number of layers, a comment should be
 *     added at the beginning of the G-code file in the format "; Layer count: xx".
 *     Separators can be " ", ":", "_" or "=".
 *   - If the total number of layers exceeds 999, this information will not be displayed because
 *     there is not enough space for both current and total layer number to be shown.
 *
 *   - For PrusaSlicer, to enable this feature, following comment lines must be added in
 *     Printer Settings -> Custom G-code section:
 *     - In After layer change G-code section:
 *       ";LAYER:[layer_num]"
 *     - In Start G-code section:
 *       ";LAYER_COUNT:[total_layer_count]"
 *
 *   Options: [Layer height: 0, Layer number: 1, Both - height & number: 2]
 */
#define LAYER_DISP_TYPE 0  // Default: 0

//================================================================================
//============================= Marlin Mode Settings =============================
//========== (only for TFT24 V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70 V3.0) ==========
//================================================================================

/**
 * Default Mode
 * Set Marlin/Touch Mode as the default mode at startup.
 *
 * NOTE: Mode switching is possible only for Marlin Mode and Touch Mode by a long press of
 *       1.5 seconds on the display or holding down the encoder button for 1.5 seconds.
 *
 *   Options: [Marlin Mode: 0, Touch Mode: 1, Blocked Marlin Mode: 2, Blocked Touch Mode: 3]
 */
#define DEFAULT_MODE 1  // Default: 1

/**
 * Serial Always ON
 * Keep UART (serial communication) alive in Marlin Mode.
 * Allows seamless OctoPrint UART connection to the TFT's UART/serial expansion port
 * no matter which mode the TFT is in.
 *   Options: [disable: 0, enable: 1]
 */
#define SERIAL_ALWAYS_ON 0  // Default: 0

/**
 * Marlin Mode Background & Font Colors
 * Set colors used in Marlin Mode.
 *
 * NOTE: Select an option from the provided list or set the color (RGB888 format) hex value directly
 *       (start with "0x"), such as: Red: 0xFF0000, Green: 0x00FF00, Blue: 0x0000FF.
 *
 *   Options: [ WHITE: 0,  BLACK: 1,  RED: 2,  GREEN: 3,      BLUE: 4,       CYAN: 5,  MAGENTA: 6,    YELLOW: 7,
 *             ORANGE: 8, PURPLE: 9, LIME: 10, BROWN: 11, DARKBLUE: 12, DARKGREEN: 13,    GRAY: 14, DARKGRAY: 15]
 */
#define MARLIN_BACKGROUND_COLOR 1  // Marlin Mode background color (Default: 1)
#define MARLIN_FONT_COLOR       0  // Marlin Mode font color (Default: 0)

/**
 * Fullscreen Marlin Mode
 * Run Marlin Mode in fullscreen.
 *
 * NOTE: Disable is recommended for TFT24.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define MARLIN_FULLSCREEN 0  // Default: 0

/**
 * Show Marlin Mode Title
 * Show banner text at the top of the TFT in Marlin Mode.
 *   Options: [disable: 0, enable: 1]
 */
#define MARLIN_SHOW_TITLE 1  // Default: 1

/**
 * Marlin Mode Title
 * Banner text displayed at the top of the TFT in Marlin Mode.
 *   Value range: [min: 3, max: 20 characters]
 */
#define MARLIN_TITLE "Marlin Mode"  // Default: "Marlin Mode"

/**
 * Marlin Mode Type
 * Select Marlin Mode type.
 *   Options: [128x64 Full Graphic LCD: 0, 20x4 Character LCD: 1]
 */
#define MARLIN_TYPE 0  // Default: 0

//================================================================================
//========================== Printer / Machine Settings ==========================
//================================================================================

/**
 * Hotend Count
 *   Value range: [min: 0, max: 6]
 */
#define HOTEND_COUNT 1  // Default: 1

/**
 * Heated Bed Support
 *   Options: [disable: 0, enable: 1]
 */
#define HEATED_BED 1  // Default: 1

/**
 * Heated Chamber Support
 * The TFT will auto-detect if chamber heating is enabled in Marlin firmware.
 *   Options: [disable: 0, enable: 1]
 */
#define HEATED_CHAMBER 0  // Default: 0

/**
 * Extruder Count
 *   Value range: [min: 0, max: 6]
 */
#define EXTRUDER_COUNT  1  // Default: 1
#define MIXING_EXTRUDER 0  // Default: 0. For mixing_extruder set to 1 (This option turns off autodetection
                           // of the number of extruders)

/**
 * Fan Count
 *   Value range: [min: 1, max: 6]
 */
#define FAN_COUNT 1  // Default: 1

/**
 * Controller Fan Support
 * Enable/disable controller fan speed control for Active and Idle cooling if Marlin
 * firmware supports controller fan (M710).
 *   Options: [disable: 0, enable: 1]
 */
#define CONTROLLER_FAN 0  // Default: 0

/**
 * Bed / Extruder / Chamber Maximum Temperatures
 *   Format: [max_temp: T0:<max temp> T1:<max temp> T2:<max temp> T3:<max temp> T4:<max temp> T5:<max temp> BED:<max temp> CHAMBER:<max temp>]
 *   Unit: [temperature in °C]
 *   Value range: hotend:  [min: 20, max: 1000]
 *                bed:     [min: 20, max: 400]
 *                chamber: [min: 20, max: 200]
 */
#define MAX_TEMP {275, 275, 275, 275, 275, 275, 150, 60}  // Default: {275, 275, 275, 275, 275, 275, 150, 60}

/**
 * Cold Extrusion Minimum Temperature
 * Minimum temperature needed to extrude/retract.
 * Any extrusion/retraction below this temperature will be prevented.
 *   Unit: [temperature in °C]
 *   Value range: [min: 20, max: 1000]
 */
#define MIN_TEMP 180  // Default: 180

/**
 * Fan Maximum PWM Speed
 * Set minimum and maximum fan speed allowed by the printer for Cooling Fans & Controller Fan.
 * Cooling fans have index from F0 to F5.
 * Controller fan has index CtA and CtI (Active and Idle). It requires "CONTROLLER_FAN" to be enabled.
 *   Format: [fan_max: F0:<max PWM> F1:<max PWM> F2:<max PWM> F3:<max PWM> F4:<max PWM> F5:<max PWM> CtA:<max PWM> CtI:<max PWM>]
 *   Unit: [PWM]
 *   Value range: [min: 25, max: 255]
 */
#define FAN_MAX {255, 255, 255, 255, 255, 255, 255, 255}  // Default: {255, 255, 255, 255, 255, 255, 255, 255}

/**
 * Machine Size / Build Area
 * The TFT will auto-detect the machine size (min and max) in Marlin firmware (requires
 * enabling "M115_GEOMETRY_REPORT" in Configuration_adv.h in Marlin firmware).
 *   Format: [size_min: X<minimum distance> Y<minimum distance> Z<minimum distance>]
 *           [size_max: X<maximum distance> Y<maximum distance> Z<maximum distance>]
 *   Unit: [distance in mm]
 *   Value range: [min: -2000, max: 2000]
 */
#define X_MIN_POS   0  // Default: 0
#define Y_MIN_POS   0  // Default: 0
#define Z_MIN_POS   0  // Default: 0
#define X_MAX_POS 235  // Default: 235
#define Y_MAX_POS 235  // Default: 235
#define Z_MAX_POS 250  // Default: 250

/**
 * X & Y Move Speeds/Feedrates
 * Move speeds used in Move menu to move X and Y axes.
 *   Format: [move_speed: S<feedrate> N<feedrate> F<feedrate>]
 *   Unit: [feedrate in mm/min]
 *   Value range: [min: 10, max: 12000]
 */
#define SPEED_XY_SLOW   1000  // Default: 1000
#define SPEED_XY_NORMAL 3000  // Default: 3000
#define SPEED_XY_FAST   5000  // Default: 5000

/**
 * Z Speeds/Feedrates
 * Move speeds used in Move menu to move Z axis.
 *   Format: [move_speed: S<feedrate> N<feedrate> F<feedrate>]
 *   Unit: [feedrate in mm/min]
 *   Value range: [min: 10, max: 12000]
 */
#define SPEED_Z_SLOW   500   // Default: 500
#define SPEED_Z_NORMAL 1000  // Default: 1000
#define SPEED_Z_FAST   2000  // Default: 2000

/**
 * Extruder Speeds/Feedrates
 * Speed settings used to extrude/retract.
 *   Format: [ext_speed: S<feedrate> N<feedrate> F<feedrate>]
 *   Unit: [feedrate in mm/min]
 *   Value range: [min: 10, max: 12000]
 */
#define EXTRUDE_SLOW_SPEED     60  // Default: 60
#define EXTRUDE_NORMAL_SPEED  600  // Default: 600
#define EXTRUDE_FAST_SPEED   1200  // Default: 1200

/**
 * Auto Save/Load Bed Leveling Data
 * The TFT will auto-detect if Auto Bed Level is available.
 * Enable this will send "M500" after "G29" to store leveling value and send "M420 S1"
 * to enable leveling state after startup.
 *   Options: [disable: 0, enable: 1]
 */
#define AUTO_LOAD_LEVELING 1  // Default: 1

/**
 * Onboard / Printer SD
 * Starting from Marlin Bugfix 2.0.x Distribution Date: 2020-04-27 & above, the TFT will auto detect
 * On-Board SD Card and auto-configure M27 AutoReport with M115 command.
 * Set the time interval to poll SD Printing status if Marlin reports M27 AutoReport as disabled.
 */

/**
 * Onboard / Printer SD Card Support
 * On Marlin firmware, the TFT will auto-detect Onboard SD Card.
 * Auto-detect is not available for other firmwares like Smoothieware.
 *   Options: [disable: 0, enable: 1, auto-detect: 2]
 */
#define ONBOARD_SD 2  // Default: 2

/**
 * M27 Printing Status Refresh Time
 * M27 printing status refresh time (this will be used if SD_AUTOREPORT is not detected by the TFT).
 *   Unit: [time in seconds]
 *   Value range: [min: 1, max: 100]
 */
#define M27_REFRESH_TIME 3  // Default: 3

/**
 * M27 Always Active
 * Keep polling M27 even if not printing (e.g. SD print not started from TFT).
 *   Options: [disable: 0, enable: 1]
 */
#define M27_ALWAYS_ACTIVE 1  // Default: 1

/**
 * Long File Names Support
 * On Marlin firmware, the TFT will auto-detect Long File Name support.
 * Auto-detect is not available for other firmwares like Smoothieware.
 *   Options: [disable: 0, enable: 1, auto-detect: 2]
 */
#define LONG_FILENAME 2  // Default: 2

/**
 * Pause/Nozzle Park Settings
 * These settings are used when a print is paused or in any feature which requires moving/parking the nozzle
 * before performing a task like in (Un)Load or Extruder Tuning menus.
 *
 * Pause Retract Length
 *   Format: [pause_retract: R<retract length> P<resume purge length>]
 *   Unit: [length in mm]
 *   Value range: [min: 0.0, max: 20.0]
 *
 * Pause XY Position
 * NOTES:
 *   - It MUST BE a value >= 0 for a Cartesian printer.
 *   - It MUST BE a value <= 0 for a Delta printer.
 *
 *   Format: [pause_pos: X<position> Y<position>]
 *   Unit: [position in mm]
 *   Value range: [min: -2000.0, max: 2000.0]
 *
 * Pause Z Raise
 * Raise Z axis by this value relative to the current layer height.
 *   Unit: [distance in mm]
 *   Value range: [min: 0.0, max: 2000.0]
 *
 * Pause Feed Rate
 * Feedrate to use when moving an axis when printing is paused.
 *   Format: [pause_feedrate: XY<feedrate> Z<feedrate> E<feedrate>]
 *   Unit: [feedrate in mm/min]
 *   Value range: [min: 10, max: 12000]
 */
#define NOZZLE_PAUSE_RETRACT_LENGTH               15.0f  // (mm) (Default: 15.0f)
#define NOZZLE_RESUME_PURGE_LENGTH                16.0f  // (mm) (Default: 16.0f)
#define NOZZLE_PAUSE_X_POSITION     (X_MIN_POS + 10.0f)  // (mm) (Default: 10.0f)
#define NOZZLE_PAUSE_Y_POSITION     (Y_MIN_POS + 10.0f)  // (mm) (Default: 10.0f)
#define NOZZLE_PAUSE_Z_RAISE                      10.0f  // (mm) (Default: 10.0f)
#define NOZZLE_PAUSE_XY_FEEDRATE                   6000  // (mm/min) X and Y axes feedrate (Default: 6000)
#define NOZZLE_PAUSE_Z_FEEDRATE                    6000  // (mm/min) Z axis feedrate (Default: 6000)
#define NOZZLE_PAUSE_E_FEEDRATE                     600  // (mm/min) retract & purge feedrate (Default: 600)

/**
 * Leveling Settings
 * These settings are used for leveling.
 *
 * Leveling Edge Distance (Manual Leveling)
 * Inset distance from bed edges. This distance is added to minimum X & Y bed coordinates and
 * subtracted from maximum X & Y bed coordinates to calculate manual leveling points.
 *   Unit: [distance in mm]
 *   Value range: [min: 0, max: 2000]
 *
 * Leveling Z Position (Manual Leveling, Mesh Leveling, Probe/Home Offset, Mesh Tuner)
 * For Manual Leveling and MBL, lower Z axis to this absolute position after reaching a leveling point.
 * For Probe/Home Offset and ABL in Mesh Tuner, raise Z axis by this relative position after reaching
 * a leveling point.
 *   Unit: [position in mm]
 *   Value range: [min: 0.0, max: 2000.0]
 *
 * Leveling Z Raise (Manual Leveling, Mesh Leveling)
 * Raise Z axis by this relative value before moving to another point during leveling/probing procedures.
 *   Unit: [distance in mm]
 *   Value range: [min: 0.0, max: 2000.0]
 *
 * Leveling Feed Rate (Manual Leveling, Mesh Leveling)
 * Feedrate to use when moving an axis during leveling/probing procedures.
 *   Format: [level_feedrate: XY<feedrate> Z<feedrate>]
 *   Unit: [feedrate in mm/min]
 *   Value range: [min: 10, max: 12000]
 */
#define LEVELING_EDGE_DISTANCE    20  // (mm) Inset distance from bed's edge for calculating leveling point location (Default: 20)
#define LEVELING_Z_POS          0.2f  // (mm) Z-axis position when nozzle stays for leveling (Default: 0.2f)
#define LEVELING_Z_RAISE       10.0f  // (mm) Z-axis position when nozzle move to next point (Default: 10.0f)
#define LEVELING_XY_FEEDRATE    6000  // (mm/min) X and Y axes move feedrate (Default: 6000)
#define LEVELING_Z_FEEDRATE     6000  // (mm/min) Z axis move feedrate (Default: 6000)

/**
 * Inverted Axes (Manual Leveling, Move, Probe Offset)
 * Used by Manual Leveling, Move and Probe Offset menus in order axis matches the actual axis movement.
 *
 * NOTE: The Y axis of different printer (move hotbed or move nozzle) move in different directions.
 *       So Y axis leveling inversion can't follow up inverted_axis[Y_AXIS].
 *       We separate a single variable "LY" (Leveling Y axis) to deal with the Y axis leveling movement direction.
 *
 *   Format: [X<option> Y<option> Z<option> LY<option>]
 *   Options: [disable: 0, enable: 1]
 */
#define INVERTED_X_AXIS  0  // Default: 0
#define INVERTED_Y_AXIS  0  // Default: 0
#define INVERTED_Z_AXIS  0  // Default: 0
#define INVERTED_LY_AXIS 0  // Default: 0

/**
 * Probing For Z Offset (Probe Offset)
 * Used by the Probe Offset menu for the Z offset tuning process.
 * If enabled, after homing a probing in the center of the bed is performed and then the nozzle
 * is moved to the XY probing point.
 * If disabled, after homing the nozzle is moved directly to the XY honing point. This is useful
 * in case Marlin firmware is configured to use the probe for Z axis homing (e.g.
 * USE_PROBE_FOR_Z_HOMING enabled in Marlin firmware) to avoid a second probing after homing.
 *
 * NOTES:
 *   - Enable it in case Marlin firmware is not configured to use the probe for Z axis homing
 *     (e.g. USE_PROBE_FOR_Z_HOMING disabled in Marlin firmware) or the XY probing point set
 *     for homing is not reachable by the nozzle (e.g. due to HW limitations/constraints or
 *     printer specific configuration).
 *   - Disable it (preferably) in case Marlin firmware is configured to use the probe for Z axis
 *     homing (e.g. USE_PROBE_FOR_Z_HOMING enabled in Marlin firmware).
 *
 *   Options: [disable: 0, enable: 1]
 */
#define PROBING_Z_OFFSET 1  // Default: 1

/**
 * Probing Z Raise (Probe Offset, Mesh Editor)
 * Used by the Probe Offset / Mesh Editor menu for the Z offset / Z height tuning process.
 * Raise / drop Z axis by this relative value after homing (G28) before starting to probe a point.
 *
 * NOTES:
 *   - It MUST BE a value >= 0 (e.g. 20) for a Cartesian printer to avoid crashing into the bed.
 *   - It MUST BE a value <= 0 (e.g. -50) for a Delta printer to avoid crashing into the top of the tower.
 *
 *   Unit: [distance in mm]
 *   Value range: [min: -2000.0, max: 2000.0]
 */
#define PROBING_Z_RAISE 20.0f  // Default: 20.0f

/**
 * Z Steppers Auto-Alignment (ABL)
 * It allows to align multiple Z stepper motors using a bed probe by probing one position per stepper.
 * Enable this setting to show an icon in ABL menu allowing to run G34 command (it requires
 * Z_STEPPER_AUTO_ALIGN enabled in Configuration_adv.h in Marlin firmware).
 *
 * NOTE: Only for Marlin printers with one stepper driver per Z stepper motor and no Z timing belt.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define Z_STEPPER_ALIGNEMENT 0  // Default: 0

/**
 * TouchMI Settings (ABL)
 * Enable this option for displaying TouchMI sensor settings in ABL menu (Init, Z Offset, Save, Test).
 *   Options: [disable: 0, enable: 1]
 */
#define TOUCHMI_SENSOR 0  // Default: 0

/**
 * Preheat Temperatures
 *   Format: [preheat_name_X:<name>]
 *           [preheat_temp_X:T<hotend temp> B<bed temp>]
 *   Unit: [temperature in °C]
 *   Value range: name:        [min: 3, max: 20 characters]
 *                hotend temp: [min: 20, max: 1000]
 *                bed temp:    [min: 20, max: 400]
 */
#define PREHEAT_LABELS {"PLA", "PETG", "ABS", "WOOD", "TPU", "NYLON"}  // Default: {"PLA", "PETG", "ABS", "WOOD", "TPU", "NYLON"}
#define PREHEAT_HOTEND {200,   240,    230,   170,    220,   250}      // Default: {200,   240,    230,   170,    220,   250}
#define PREHEAT_BED    { 60,    70,     90,    50,     50,    90}      // Default: { 60,    70,     90,    50,     50,    90}

//================================================================================
//============================ Power Supply Settings =============================
//==================== (only if connected to TFT controller) =====================
//================================================================================

/**
 * Active HIGH Power Supply Logic
 * Used in case it is supported by the TFT.
 * The power supply uses this HIGH signal logic to stay ON.
 * The power supply uses the opposite of this logic to stay OFF.
 *   Options: [disable: 0, enable: 1]
 */
#define PS_ACTIVE_HIGH 1  // Default: 1

/**
 * Power Supply Auto Shutdown Mode
 * Used in case it is supported by the TFT.
 * Enable power supply auto shutdown after a print is finished when hotend temperature drops below target value.
 *   Options: [disable: 0, enable: 1, auto-detect: 2]
 */
#define PS_AUTO_SHUTDOWN 0  // Default: 0

/**
 * Power Supply Auto Shutdown Temperature
 * Maximum hotend temperature for automatic shutdown after printing, if automatic shutdown is enabled.
 * The printer will shutdown automatically if the hotend temperature is below this value.
 * If the hotend temperature is higher than this value the fans will be turned on to cooldown and it
 * will wait for the hotend temperature to drop below this value before shutting down automatically.
 *   Unit: [temperature in °C]
 *   Value range: [min: 20, max: 1000]
 */
#define PS_AUTO_SHUTDOWN_TEMP 50  // Default: 50

//================================================================================
//=========================== Filament Runout Settings ===========================
//==================== (only if connected to TFT controller) =====================
//================================================================================

/**
 * NOTES for users having a filament sensor connected to the mainboard:
 *   1) Define "FIL_RUNOUT 0" below to disable the sensor handling on the TFT.
 *   2) Configure the sensor in the firmware of your mainboard.
 *   3) Add M75 to "START_GCODE" and M77 to "END_GCODE" of the TFT (or your slicer).
 */

/**
 * Filament Runout Sensor
 * Select the type of filament runout sensor and its default enabled/disabled state.
 *   Options: [Normal Disabled: 0, Normal Enabled: 1, Smart Disabled: 2, Smart Enabled: 3]
 */
#define FIL_RUNOUT 0  // Default: 0

/**
 * Inverted Filament Runout Sensor Logic
 * The sensor uses an inverted logic.
 *   Options: [disable: 0, enable: 1]
 */
#define FIL_RUNOUT_INVERTED 1  // Default: 1

/**
 * NC (Normal Close) Filament Runout Sensor
 * The sensor is of type NC.
 *   Options: [Normal Open: 0, Normal Close: 1]
 */
#define FIL_RUNOUT_NC 1  // Default: 1

/**
 * Filament Runout Noise Threshold
 * Pause print when filament runout is detected at least for this time period.
 *   Unit: [time in milliseconds]
 *   Value range: [min: 10, max: 1800]
 */
#define FIL_RUNOUT_NOISE_THRESHOLD 100  // Default: 100

/**
 * Smart Filament Runout Detection
 * Used in conjunction with an SFS (Smart Filament Sensor) based on an encoder disc that
 * toggles runout pin as filament moves.
 *   Unit: [distance in mm]
 *   Value range: [min: 1, max: 50]
 */
#define FIL_RUNOUT_DISTANCE 7  // Default: 7

//================================================================================
//==================== Power Loss Recovery & BTT UPS Settings ====================
//================================================================================

/**
 * Most suitable for Delta printers since most printers will
 * crash into printed model when homing after powerloss.
 */

/**
 * Power Loss Recovery Mode
 * Enable power loss recovery.
 * Disable to reduce the loss of SD card or U disk.
 *   Options: [disable: 0, enable: 1]
 */
#define PL_RECOVERY 1  // Default: 1

/**
 * Power Loss Recovery Homing
 * Home before power loss recovery.
 *   Options: [disable: 0, enable: 1]
 */
#define PL_RECOVERY_HOME 0  // Default: 0

/**
 * Power Loss Recovery Z Raise
 * Raise Z axis on resume (on power loss with UPS).
 *   Unit: [distance in mm]
 *   Value range: [min: 0.0, max: 2000.0]
 */
#define PL_RECOVERY_Z_RAISE 10.0f  // Default: 10.0f

/**
 * BTT Mini UPS Support
 * Enable backup power/UPS to move Z axis on power loss.
 *   Options: [disable: 0, enable: 1]
 */
#define BTT_MINI_UPS 0  // Default: 0

//================================================================================
//======================== Other Device-Specific Settings ========================
//================================================================================

/**
 * Sounds / Buzzer
 * Set sound ON or OFF.
 *
 * NOTE: Error messages from printer will always play the error sound.
 *
 * Parameters:
 *   touch_sound:  Enable/disable this to control touch feedback sound.
 *   toast_sound:  Enable/disable this to control all toast notification sounds.
 *   alert_sound:  Enable/disable this to control all popup and alert sounds
 *                 like print finish alert, dialog sound etc.
 *   heater_sound: Enable/disable this to control acoustic feedback when temperature
 *                 has reached the desired value on heaters (nozzle, bed, chamber).
 *
 *   Options: [disable: 0, enable: 1]
 */
#define TOUCH_SOUND  1  // Default: 1
#define TOAST_SOUND  1  // Default: 1
#define ALERT_SOUND  1  // Default: 1
#define HEATER_SOUND 1  // Default: 1

/**
 * LCD Brightness Levels (only for TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
 * Brightness levels for LCD.
 *   Options: [OFF: 0, 5%: 1, 10%: 2, 20%: 3, 30%: 4, 40%: 5, 50%: 6, 60%: 7, 70%: 8, 80%: 9, 90%: 10, 100%: 11]
 */
#define LCD_BRIGHTNESS      11  // LCD brightness level (Default: 11)
#define LCD_IDLE_BRIGHTNESS  5  // LCD brightness level when device is idle (Default: 5)

/**
 * LCD Idle Time (only for TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
 * The LCD screen will dim to idle brightness, if the display is not touched for the
 * period of the LCD idle time.
 *   Options: [OFF: 0, 5sec: 1, 10sec: 2, 30sec: 3, 1min: 4, 2min: 5, 5min: 6, 10min: 7]
 */
#define LCD_IDLE_TIME 0  // Default: 0

// Custom value in seconds. This will be used if LCD_IDLE_TIME is set to 7 (CUSTOM Seconds)
#define IDLE_TIME_CUSTOM (10 * 60)  // Default: 10 * 60

/**
 * LCD Lock On Idle (only for TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
 * If enabled, when the LCD is idle (dimmed) then the first touch on the display will
 * simply restore the normal LCD brightness. The touch is then skipped, preventing to
 * trigger any undesired action due to the dimmed display.
 *
 * NOTE: The lock is always avoided if the LCD brightness is restored by the use of
 *       rotary encoder instead of touching the display.
 *
 *   Options: [disable: 0, enable: 1]
 */
#define LCD_LOCK_ON_IDLE 0  // Default: 0

/**
 * Knob LED Color (only for TFT28/TFT35_E3/TFT43/TFT50/TFT70 V3.0)
 * Knob LED color at startup.
 *   Options: [OFF: 0, WHITE: 1, RED: 2, ORANGE: 3, YELLOW: 4, GREEN: 5, BLUE: 6, INDIGO: 7, VIOLET: 8]
 */
#define KNOB_LED_COLOR 1  // Default: 1

// Keep the LED state in Marlin Mode
#define KEEP_KNOB_LED_COLOR_MARLIN_MODE  // Default: uncommented (enabled)

/**
 * Knob LED Idle State (only for TFT28/TFT35_E3/TFT43/TFT50/TFT70 V3.0)
 * If enabled, when the LCD is idle (dimmed) then the knob LED will be also switched off.
 *   Options: [disable: 0, enable: 1]
 */
#define KNOB_LED_IDLE 1  // Default: 1

/**
 * Knob LED Pixels (only for TFT28/TFT35_E3/TFT43/TFT50/TFT70 V3.0)
 * Set the number of LEDs in the strip connected to "Neopixel" port of TFT.
 * It shares the same signal line as "knob_led". 0 means the default number in TFT hardware.
 * Greater than 0 means the number of LEDs in the strip.
 *   Value range: [min: 0, max: 200]
 */
//#define NEOPIXEL_PIXELS 0  // Default: commented (use default value for TFT variant).
                             // Uncomment it to override default value for TFT variant

//================================================================================
//============================ Custom G-code Commands ============================
//================================================================================

/**
 * Custom G-code Commands
 * Up to 15 custom G-code commands that will be available in the Custom menu.
 *
 * Usage:
 *   - To enable a custom command, remove "//" at the beginning of custom commands label & G-code.
 *   - To disable a custom command, add "//" at the beginning of custom commands label & G-code.
 *
 * NOTE: If the values are left blank then default name and G-code will be used.
 *
 *   Value range: label: [min: 3, max: 75 characters]
 *                G-code: [min: 3, max: 75 characters]
 */
#define CUSTOM_LABEL_0 "Disable Steppers"
#define CUSTOM_GCODE_0 "M84\n"
#define CUSTOM_LABEL_1 "Init SD Card"
#define CUSTOM_GCODE_1 "M21\n"
#define CUSTOM_LABEL_2 "Release SD Card"
#define CUSTOM_GCODE_2 "M22\n"
#define CUSTOM_LABEL_3 "Enable Leveling State"
#define CUSTOM_GCODE_3 "M420 S1\n"
#define CUSTOM_LABEL_4 "Save to EEPROM"
#define CUSTOM_GCODE_4 "M500\n"
#define CUSTOM_LABEL_5 "Restore from EEPROM"
#define CUSTOM_GCODE_5 "M501\n"
#define CUSTOM_LABEL_6 "EEPROM Defaults"
#define CUSTOM_GCODE_6 "M502\n"
//#define CUSTOM_LABEL_7 "Custom7"
//#define CUSTOM_GCODE_7 "M105\n"
//#define CUSTOM_LABEL_8 "Custom8"
//#define CUSTOM_GCODE_8 "M105\n"
//#define CUSTOM_LABEL_9 "Custom9"
//#define CUSTOM_GCODE_9 "M105\n"
//#define CUSTOM_LABEL_10 "Custom10"
//#define CUSTOM_GCODE_10 "M105\n"
//#define CUSTOM_LABEL_11 "Custom11"
//#define CUSTOM_GCODE_11 "M105\n"
//#define CUSTOM_LABEL_12 "Custom12"
//#define CUSTOM_GCODE_12 "M105\n"
//#define CUSTOM_LABEL_13 "Custom13"
//#define CUSTOM_GCODE_13 "M105\n"
//#define CUSTOM_LABEL_14 "Custom14"
//#define CUSTOM_GCODE_14 "M105\n"

//================================================================================
//===================== Start, End & Cancel G-code Commands ======================
//================================================================================

/**
 * NOTES for users having a filament sensor connected to the mainboard:
 *   1) Enable the start/end G-code below.
 *   2) Add the following commands to the start/end G-code:
 *      - START_GCODE: M75\n
 *      - END_GCODE: M77\n
 */

/**
 * Start, End and Cancel G-code Status
 *   Options: [disable: 0, enable: 1]
 */
#define START_GCODE_ENABLED  0  // Default: 0
#define END_GCODE_ENABLED    0  // Default: 0
#define CANCEL_GCODE_ENABLED 0  // Default: 0

/**
 * Start, End and Cancel G-code
 * START_GCODE will run before starting a print if "START_GCODE_ENABLED" is enabled.
 * END_GCODE will run after a print is completed if "END_GCODE_ENABLED" is enabled.
 * CANCEL_GCODE will run when a print is canceled if "CANCEL_GCODE_ENABLED" is enabled.
 *   Value range: [min: 3, max: 75 characters]
 */
#define START_GCODE  "G28 XY R20\n"
#define END_GCODE    "M104 S0\nM140 S0\nM107\nM18\n"
#define CANCEL_GCODE "M104 S0\nM140 S0\nG28 XY R10\nM107\nM18\n"

//====================================================================================================
//============================ Settings Configurable At Compile Time Only ============================
//====================================================================================================

//================================================================================
//========================== Developer / Debug Settings ==========================
//================================================================================

/**
 * Screenshot For Documentation
 * A press on touch screen or on encoder button will take and save a full screen screenshot to SD card.
 * Only for documentation purposes. DON'T enable it for normal printing.
 *
 * For TFT with an encoder "LCD_ENCODER_SUPPORT":
 * - A press on touch screen to trigger a screenshot in Marlin mode
 * - A press on encoder button to trigger a screenshot in Touch mode
 *
 * For TFT with no encoder:
 * - A long press of 1.5 seconds on touch screen to trigger a screenshot
 *
 * WARNING for TFT35 V2, TFT35 V3, TFT35 B1 V3 and TFT35 E3 V3:
 *   This feature requires that pin 39 on the 40 pins TFT connector is switched from GND to 3.3V.
 *   Otherwise, the color read out is incorrect.
 */
//#define SCREEN_SHOT_TO_SD  // Default: commented (disabled)

/**
 * If one of the DEBUG_x below is defined the SERIAL_DEBUG_PORT defined in board specific Pin_xx.h file
 * will be used for debugging purposes. DON'T enable one of the DEBUG_x below for normal printing.
 *
 * WARNINGS:
 *   - SERIAL_DEBUG_PORT needs to be activated specifically. Please consider SERIAL_PORT setting in the config.ini.
 *   - If the TFT is only showing the boot logo and is not responding anymore, check the SERIAL_PORT setting.
 *   - If you update the config.ini you need to reset the TFT *twice* (first reset to update the config, second
 *     reset to enable the SERIAL_DEBUG_PORT due to the changed firmware config).
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

// PID autotune
#define PID_CMD_MARLIN      {"M303 U1 C8 E0", "M303 U1 C8 E1", "M303 U1 C8 E2", "M303 U1 C8 E3", "M303 U1 C8 E4", "M303 U1 C8 E5", "M303 U1 C8 E-1", ""}
#define PID_CMD_RRF         {"M303 T0",       "M303 T1",       "M303 T2",       "M303 T3",       "M303 T4",       "M303 T5",       "M303 H0",        ""}
#define PID_PROCESS_TIMEOUT (15 * 60000)  // Timeout in MilliSeconds (1 minute = 60000 MilliSeconds). Default: 15 * 60000

// Heat
#define HEAT_SIGN_ID    {"T0:",     "T1:",     "T2:",     "T3:",     "T4:",     "T5:",     "B:",   "C:"}
#define HEAT_DISPLAY_ID {"T0",      "T1",      "T2",      "T3",      "T4",      "T5",      "Bed",  "Chamber"}
#define HEAT_SHORT_ID   {"T0",      "T1",      "T2",      "T3",      "T4",      "T5",      "Bed",  "Ch."}
#define HEAT_CMD        {"M104 T0", "M104 T1", "M104 T2", "M104 T3", "M104 T4", "M104 T5", "M140", "M141"}
#define HEAT_WAIT_CMD   {"M109 T0", "M109 T1", "M109 T2", "M109 T3", "M109 T4", "M109 T5", "M190", "M191"}

// Tool Change / Extruder Id
#define TOOL_CHANGE {"T0", "T1", "T2", "T3", "T4", "T5"}
#define EXTRUDER_ID {"E0", "E1", "E2", "E3", "E4", "E5"}

/**
 * Cooling Fans & Controller Fan
 * Cooling fans have index from 0 to 5.
 * Controller fan has index 6 and 7 (Active and Idle).
 */
#define FAN_DISPLAY_ID {"F0 ", "F1 ", "F2 ", "F3 ", "F4 ", "F5 ", "CtA", "CtI"}
#define FAN_CMD        {"M106 P0 S%d\n", "M106 P1 S%d\n", "M106 P2 S%d\n", "M106 P3 S%d\n", "M106 P4 S%d\n", "M106 P5 S%d\n", \
                        "M710 S%d\n",    "M710 I%d\n" }

// Speed/flow rate names displayed in status screen
#define SPEED_ID {"Sp.", "Fr."}  // (speed, flow rate)

// Axes names displayed in Parameter Settings menu
#define AXIS_DISPLAY_ID    {"X", "Y", "Z", "E0", "E1"}                    // (X, Y, Z, E0, E1)
#define STEPPER_DISPLAY_ID {"X", "X2", "Y", "Y2", "Z", "Z2", "E0", "E1"}  // (X, X2, Y, Y2, Z, Z2, E0, E1)

// Manual Leveling
// Move to four corner points to Leveling manually (Point 1, Point 2, Point 3, Point 4).
#define LEVELING_EDGE_DISTANCE_DISPLAY_ID "X/Y"
#define LEVELING_EDGE_DISTANCE_MIN           0                    // Default: 0
#define LEVELING_EDGE_DISTANCE_MAX         100                    // Default: 100
#define LEVELING_EDGE_DISTANCE_DEFAULT    LEVELING_EDGE_DISTANCE  // Default: LEVELING_EDGE_DISTANCE

// Z Fade limits
#define Z_FADE_MIN_VALUE      0.0f  // Default: 0.0f
#define Z_FADE_MAX_VALUE     20.0f  // Default: 20.0f
#define Z_FADE_DEFAULT_VALUE 10.0f  // Default: 10.0f

// Probe Offset limits
#define PROBE_Z_OFFSET_MIN_VALUE     -20.0f  // Default: -20.0f
#define PROBE_Z_OFFSET_MAX_VALUE      20.0f  // Default: 20.0f
#define PROBE_Z_OFFSET_DEFAULT_VALUE   0.0f  // Default: 0.0f

// Home Offset limits
#define HOME_Z_OFFSET_MIN_VALUE     -20.0f  // Default: -20.0f
#define HOME_Z_OFFSET_MAX_VALUE      20.0f  // Default: 20.0f
#define HOME_Z_OFFSET_DEFAULT_VALUE   0.0f  // Default: 0.0f

// Babystep limits
#define BABYSTEP_MIN_VALUE     -5.0f  // Default: -5.0f
#define BABYSTEP_MAX_VALUE      5.0f  // Default: 5.0f
#define BABYSTEP_DEFAULT_VALUE  0.0f  // Default: 0.0f
#define BABYSTEP_MAX_STEP       1.0f  // Default: 1.0f

/**
 * Mesh Leveling Max Grid Points (Mesh Editor)
 * Set the maximum number of grid points per dimension.
 *   Value range: [min: 1, max: 15]
 */
#define MESH_GRID_MAX_POINTS_X 15  // Default: 15
#define MESH_GRID_MAX_POINTS_Y 15  // Default: 15

/**
 * Bed Leveling Type
 * The TFT will attempt to auto detect bed leveing type with marlin firmware.
 * Set to 1 to enable auto-detect (Marlin only) or choose one of the options
 * below if auto-detect does not work.
 *
 * WARNING: If you're not sure, leave on auto-detect or disabled.
 *          UBL has extra options other leveling systems might not have.
 *
 *   Options: [disable: 0, auto-detect: 1, ABL: 2, BBL: 3, UBL: 4, MBL: 5]
 */
#define BED_LEVELING_TYPE 1  // Default: 1

/**
 * MBL Settings
 * Apply the "level_z_pos" configurable parameter value as the
 * starting Z height for each point during MBL process.
 * If not enabled, you can set the desired starting Z height
 * in Marlin fw (MANUAL_PROBE_START_Z in Configuration.h).
 */
#define MBL_START_Z  // Default: uncommented (enabled)

/**
 * M601: Pause Print
 * PrusaSlicer can add M601 on certain height.
 * Acts here like manual pause.
 */
#define NOZZLE_PAUSE_M601  // Default: uncommented (enabled)

/**
 * M701, M702: Marlin Filament Load / Unload Gcodes Support
 * FILAMENT_LOAD_UNLOAD_GCODES option on Marlin configuration_adv.h need to be uncommented.
 * Adds a submenu to the movement menu for selecting load and unload actions.
 */
#define LOAD_UNLOAD_M701_M702  // Default: uncommented (enabled)

/**
 * Delta Probe Type
 * Probe type in case of a Delta printer.
 * Set to 0 in case of not Delta printer.
 *   Options: [disable: 0, fixed-probe: 1, removable-probe: 2]
 */
#define DELTA_PROBE_TYPE 0  // Default: 0

//================================================================================
//================================= UI Settings ==================================
//================================================================================

/**
 * Primary Language (for Touch-Mode only)
 * Select the language to use on the LCD while in Touch Mode.
 *
 * NOTE: To add/flash a second language copy the required "language_xx.ini" file from
 *       "Language Packs" folder to the SD root folder.
 *       Then press the reset button to load/flash the copied language file.
 *
 *   Options: [ENGLISH,    CHINESE,  RUSSIAN,     JAPANESE,   ARMENIAN,  GERMAN,        CZECH,
 *             SPANISH,    FRENCH,   PORTUGUESE,  ITALIAN,    POLISH,    SLOVAK,        DUTCH,
 *             HUNGARIAN,  TURKISH,  GREEK,       SLOVENIAN,  CATALAN,   TRAD_CHINESE,  UKRAINIAN,
 *             BRAZIL]
 */
#define SYSTEM_LANGUAGE ENGLISH  // Default: ENGLISH

/**
 * Rapid Serial Communication
 * More frequent Serial communicaiton while printing.
 * Send and parse G-codes more frequently while drawing on screen to
 * prevent printer idling and stuttering due to empty printer buffer.
 *
 * NOTE: This may slow down graphics while switching menus while printing.
*/
#define RAPID_SERIAL_COMM  // Default: uncommented (enabled)

/**
 * LCD Encoder Settings (HW Rotary Encoder)
 * In case LCD Encoder's sliding buttons (pin LCD_ENCA_PIN and LCD_ENCB_PIN) don't produce
 * any movement on menu, try to increase the delay (in MilliSeconds) (e.g. 64).
 */
#ifdef MKS_TFT
  #define LCD_ENC_DELAY           40  // in ms. Default: 8
  #define LCD_ENC_PULSES_PER_STEP  2  // Default: 4
  #define LCD_ENC_BUTTON_INTERVAL 20  // in ms. Default: 20
#else
  #define LCD_ENC_DELAY            8  // in ms. Default: 8
  #define LCD_ENC_PULSES_PER_STEP  4  // Default: 4
  #define LCD_ENC_BUTTON_INTERVAL 20  // in ms. Default: 20
#endif

/**
 * Buzzer Settings
 * The duration and frequency for the UI feedback sound.
 * Set these to 0 to disable audio feedback in the LCD menus.
 * Only valid for Touch Mode and if BUZZER_PIN is set or available.
 *
 * NOTE: Test audio output with the G-Code:
 *       M300 S<frequency Hz> P<duration MilliSeconds>
 */
#define BUZZER_FREQUENCY_DURATION_MS    20  // in ms. Default: 20
#define BUZZER_FREQUENCY_HZ          10000  // in Hz (20Hz to 60000Hz). Default: 10000

/**
 * Buzzer Stop Level
 * Buzzer pin state when buzzer is idle or no sound is playing.
 *   Options: [HIGH, LOW]
 */
#define BUZZER_STOP_LEVEL LOW  // Default: LOW

// Show bootscreen when starting up
#define SHOW_BTT_BOOTSCREEN  // Default: uncommented (enabled)

// Bootscreen logo time in ms
#define BTT_BOOTSCREEN_TIME 3000  // Default: 3000

/**
 * Smart Home
 * If enabled, long press "Back" button triggers Home screen
 * It doesn't interfere with the "Screenshot" and "Marlin/Touch Mode" other than if
 * enabled, long press "Back" will not trigger "Screenshot" or "Marlin/Touch Mode"
 */
#define SMART_HOME  // Default: uncommented (enabled)

/**
 * Alternative Move Menu Layout
 * Enable alternative Move Menu Buttons Layout matching the direction
 * of actual printer axis Update the icons from alternate icon folder.
 */
#define ALTERNATIVE_MOVE_MENU  // Default: uncommented (enabled)

/**
 * Friendly Z Offset Language
 * Replace decrease/increase and "-" & "+" icons with down/up and friendly icons.
 */
#define FRIENDLY_Z_OFFSET_LANGUAGE  // Default: uncommented (enabled)

/**
 * Quick EEPROM Menu
 * Enable EEPROM menu (save/load/reset buttons) in Settings -> Machine menu.
 *
 * NOTE: If disabled, EEPROM operations can also be accessed in
 *       Settings -> Machine -> Parameter Settings menu.
 */
#define QUICK_EEPROM_BUTTON  // Default: uncommented (enabled)

/**
 * Toast Notification Duration (in MilliSeconds)
 * Set the duration for displaying toast notification on top of the screen.
 */
#define TOAST_DURATION (3 * 1000)  // in ms. Default: 3 * 1000

/**
 * Keyboard On Left Side (Mesh Editor, LED Color Custom)
 * By default the keyboard is drawn on right side of the screen.
 * Enable KEYBOARD_ON_LEFT to draw the keyboard on left side of the screen.
 */
//#define KEYBOARD_ON_LEFT  // Default: commented (disabled)

/**
 * Terminal Keyboard / Numpad Settings
 */

/**
 * Terminal Keyboard / Numpad Theme
 * Uncomment to enable Material theme for keyboard and Numpad.
 */
#define KEYBOARD_MATERIAL_THEME  // Default: uncommented (enabled)

/**
 * Terminal Keyboard / Numpad Color Layout
 * Color layout for the Terminal Keyboard / Numpad.
 *   Options: [Default: 0, Invert: 1, High Contrast: 2]
 *     Default:       The keyboard follows system background and border colors.
 *                    (Material Dark if KEYBOARD_MATERIAL_THEME is enabled).
 *     Invert:        The keyboard is drawn with background swapped with border colors.
 *                    (Material Light if KEYBOARD_MATERIAL_THEME is enabled).
 *     High Contrast: Use white and black colors for high contrast.
 */
#define KEYBOARD_COLOR_LAYOUT 0  // Default: 0

/**
 * QWERTY/QWERTZ Keyboard Layout (Terminal menu)
 * Keyboard layout for Terminal Keyboard (Only for TFT70 V3.0).
 *   Options: [qwerty: 0, qwertz: 1, azerty: 2]
 *     qwerty: The typically keyboard Layout for english.
 *     qwertz: The typically keyboard Layout for german.
 *     azerty: The typically keyboard Layout for french.
 */
#define TERMINAL_KEYBOARD_LAYOUT 0  // Default: 0

/**
 * Progress Bar Color (Printing menu)
 * The color of the progress bar during print.
 *   Options: [Orange: 0, Yellow: 1, Red: 2, Green: 3, Blue: 4, Cyan: 5, Magenta: 6, Purple: 7, Lime: 8, Gray: 9]
 */
#define PROGRESS_BAR_COLOR 0  // Default: 0

/**
 * Progress Bar Layout (Printing menu)
 * Uncomment to enable a progress bar with 10% markers.
 * Comment to enable a standard progress bar.
 */
//#define MARKED_PROGRESS_BAR  // Default: commented (disabled)

/**
 * Live Text Common Color Layout (Status Screen menu)
 * Some topics require to use a common color for live text in Status Screen menu.
 * Uncomment to use the color of live text 1 (name) also for live text 2 (value)
 * (e.g. for THEME_Rep Rap Firmware Dark theme).
 * Comment to use standard colors.
 */
//#define LIVE_TEXT_COMMON_COLOR  // Default: commented (disabled)

/**
 * Live Text Background Color Rendering Technique (Printing menu and Status Screen menu)
 * When enabled, it allows to eliminate the flickering on alternating icons avoiding to
 * draw the icon background under the live text area.
 * Furthermore, it allows to use the icon background colors or a sampled icon background
 * uniform color for each live text.
 * When disabled (set to 0), alternating icons are always fully drawn causing some
 * flickering when live text is drawn on top of them.
 * Furthermore, a standard rendering based on the sampling and use, in a pixel by pixel
 * basis, of the underlying icon background colors is used.
 *
 * NOTES:
 *   - Enable it only in case the icons maintain always the same background colors under
 *     the live text areas (e.g. applicable to Unified, Round Miracle etc... menu themes).
 *   - If enabled, it speeds up the rendering of the live text and the responsiveness of
 *     the TFT, so it can improve the print quality.
 *     Suitable in particular for the TFTs with a not fast HW (e.g. 24, 48 MHz).
 *   - If enabled, it allows to eliminate the flickering on alternating icons.
 */

/**
 * Live Text Background Color Rendering Technique (Printing menu)
 *   Value range: [min: 0, max: 2]
 *     0: disabled
 *     1: apply icon background colors to live text
 *     2: apply sampled icon background uniform color to live text
 */
#define LIVE_TEXT_BG_COLOR_PRINTING 0  // Default: 0 (disabled)

/**
 * Live Text Background Color Rendering Technique (Status Screen menu)
 *   Value range: [min: 0, max: 6]
 *     0: disabled
 *     1: apply icon background colors to live text 1 (name)
 *     2: apply sampled icon background uniform color to live text 1 (name)
 *     3: apply icon background colors to live text 2 (value)
 *     4: apply sampled icon background uniform color to live text 2 (value)
 *     5: apply icon background colors to both live text 1 and live text 2
 *     6: apply sampled icon background uniform color to both live text 1 and live text 2
 */
#define LIVE_TEXT_BG_COLOR_STATUS 0  // Default: 0 (disabled)

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
 *                    PNG thumbnail. It is slower as classic but most flexible. It does not
 *                    require dedicated post-processing of gcode files for most slicers.
 *                    "RGB565 bitmap" and "Classic" are used as fallback.
 */
#define THUMBNAIL_PARSER 0  // Default: 0

#endif
