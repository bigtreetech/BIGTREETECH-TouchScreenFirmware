#include "Settings.h"
#include "includes.h"

SETTINGS infoSettings;
MACHINE_SETTINGS infoMachineSettings;

const uint8_t default_serial_port[]    = {SP_1, SP_2, SP_3, SP_4};
const uint16_t default_max_temp[]      = MAX_TEMP;
const uint16_t default_max_fan[]       = FAN_MAX;
const uint16_t default_size_min[]      = {X_MIN_POS, Y_MIN_POS, Z_MIN_POS};
const uint16_t default_size_max[]      = {X_MAX_POS, Y_MAX_POS, Z_MAX_POS};
const uint16_t default_xy_speed[]      = {SPEED_XY_SLOW, SPEED_XY_NORMAL, SPEED_XY_FAST};
const uint16_t default_z_speed[]       = {SPEED_Z_SLOW, SPEED_Z_NORMAL, SPEED_Z_FAST};
const uint16_t default_ext_speed[]     = {EXTRUDE_SLOW_SPEED, EXTRUDE_NORMAL_SPEED, EXTRUDE_FAST_SPEED};
const uint16_t default_pause_speed[]   = {NOZZLE_PAUSE_XY_FEEDRATE, NOZZLE_PAUSE_Z_FEEDRATE, NOZZLE_PAUSE_E_FEEDRATE};
const uint16_t default_level_speed[]   = {LEVELING_XY_FEEDRATE, LEVELING_Z_FEEDRATE};
const uint16_t default_preheat_ext[]   = PREHEAT_HOTEND;
const uint16_t default_preheat_bed[]   = PREHEAT_BED;
const uint8_t default_led_color[]      = {LED_R, LED_G, LED_B, LED_W, LED_P, LED_I};
const uint8_t default_custom_enabled[] = CUSTOM_GCODE_ENABLED;

// Init settings data with default values
void initSettings(void)
{
// General Settings
  infoSettings.general_settings       = ((0 << INDEX_LISTENING_MODE) | (EMULATED_M600 << INDEX_EMULATED_M600) |
                                         (EMULATED_M109_M190 << INDEX_EMULATED_M109_M190) |
                                         (EVENT_LED << INDEX_EVENT_LED) |
                                         (FILE_COMMENT_PARSING << INDEX_FILE_COMMENT_PARSING));

// UI Settings
  infoSettings.rotated_ui             = ROTATED_UI;
  infoSettings.language               = LANGUAGE;
  infoSettings.status_screen          = STATUS_SCREEN;
  infoSettings.title_bg_color         = lcd_colors[TITLE_BACKGROUND_COLOR];
  infoSettings.bg_color               = lcd_colors[MENU_BACKGROUND_COLOR];
  infoSettings.font_color             = lcd_colors[MENU_FONT_COLOR];
  infoSettings.reminder_color         = lcd_colors[REMINDER_FONT_COLOR];
  infoSettings.status_color           = lcd_colors[STATUS_FONT_COLOR];
  infoSettings.status_xyz_bg_color    = lcd_colors[STATUS_XYZ_BG_COLOR];
  infoSettings.list_border_color      = lcd_colors[LIST_BORDER_COLOR];
  infoSettings.list_button_bg_color   = lcd_colors[LIST_BUTTON_BG_COLOR];
  infoSettings.mesh_min_color         = lcd_colors[MESH_MIN_COLOR];
  infoSettings.mesh_max_color         = lcd_colors[MESH_MAX_COLOR];
  infoSettings.terminal_color_scheme  = TERMINAL_COLOR_SCHEME;

  infoSettings.ack_notification       = ACK_NOTIFICATION;
  infoSettings.files_sort_by          = FILES_SORT_BY;
  infoSettings.files_list_mode        = FILES_LIST_MODE;
  infoSettings.filename_extension     = FILENAME_EXTENSION;
  infoSettings.fan_percentage         = FAN_SPEED_PERCENTAGE;
  infoSettings.persistent_info        = PERSISTENT_INFO;
  infoSettings.terminal_ack           = TERMINAL_ACK;
  infoSettings.notification_m117      = NOTIFICATION_M117;
  infoSettings.prog_source            = PROG_SOURCE;
  infoSettings.prog_disp_type         = PROG_DISP_TYPE;
  infoSettings.layer_disp_type        = LAYER_DISP_TYPE;

// Marlin Mode Settings (only for TFT24 V1.1 & TFT28/TFT35/TFT43/TFT50/TFT70 V3.0)
  infoSettings.mode                   = DEFAULT_MODE;
  infoSettings.serial_always_on       = SERIAL_ALWAYS_ON;
  infoSettings.marlin_bg_color        = lcd_colors[MARLIN_BACKGROUND_COLOR];
  infoSettings.marlin_font_color      = lcd_colors[MARLIN_FONT_COLOR];
  infoSettings.marlin_fullscreen      = MARLIN_FULLSCREEN;
  infoSettings.marlin_show_title      = MARLIN_SHOW_TITLE;
  infoSettings.marlin_type            = MARLIN_TYPE;

// Printer / Machine Settings
  infoSettings.hotend_count           = HOTEND_COUNT;
  infoSettings.bed_en                 = HEATED_BED;
  infoSettings.chamber_en             = HEATED_CHAMBER;
  infoSettings.ext_count              = EXTRUDER_COUNT;
  infoSettings.fan_count              = FAN_COUNT;
  infoSettings.ctrl_fan_en            = CONTROLLER_FAN;
  infoSettings.min_ext_temp           = MIN_TEMP;
  infoSettings.auto_load_leveling     = AUTO_LOAD_LEVELING;
  infoSettings.onboard_sd             = ONBOARD_SD;  // ENABLED / DISABLED / AUTO
  infoSettings.m27_refresh_time       = M27_REFRESH_TIME;
  infoSettings.m27_active             = M27_ALWAYS_ACTIVE;
  infoSettings.long_filename          = LONG_FILENAME;  // ENABLED / DISABLED / AUTO

  infoSettings.pause_retract_len      = NOZZLE_PAUSE_RETRACT_LENGTH;
  infoSettings.resume_purge_len       = NOZZLE_RESUME_PURGE_LENGTH;
  infoSettings.pause_pos[X_AXIS]      = NOZZLE_PAUSE_X_POSITION;  // X
  infoSettings.pause_pos[Y_AXIS]      = NOZZLE_PAUSE_Y_POSITION;  // Y
  infoSettings.pause_z_raise          = NOZZLE_PAUSE_Z_RAISE;

  infoSettings.level_edge             = LEVELING_EDGE_DISTANCE;
  infoSettings.level_z_pos            = LEVELING_Z_POS;
  infoSettings.level_z_raise          = LEVELING_Z_RAISE;

  infoSettings.move_speed             = 1;  // index on infoSettings.axis_speed, infoSettings.ext_speed

  infoSettings.inverted_axis          = ((INVERTED_X_AXIS << X_AXIS) | (INVERTED_Y_AXIS << Y_AXIS) |
                                         (INVERTED_Z_AXIS << Z_AXIS) | (INVERTED_LY_AXIS << E_AXIS));

  infoSettings.probing_z_offset       = PROBING_Z_OFFSET;
  infoSettings.probing_z_raise        = PROBING_Z_RAISE;
  infoSettings.z_steppers_alignment   = Z_STEPPER_ALIGNEMENT;
  infoSettings.touchmi_sensor         = TOUCHMI_SENSOR;

// Power Supply Settings (only if connected to TFT controller)
  infoSettings.ps_active_high         = PS_ACTIVE_HIGH;
  infoSettings.auto_shutdown          = PS_AUTO_SHUTDOWN;
  infoSettings.auto_shutdown_temp     = PS_AUTO_SHUTDOWN_TEMP;

// Filament Runout Settings (only if connected to TFT controller)
  infoSettings.runout                 = ((FIL_RUNOUT << RUNOUT_ENABLED) | (FIL_RUNOUT_INVERTED << RUNOUT_INVERTED) |
                                         (FIL_RUNOUT_NC << RUNOUT_NO_NC));
  infoSettings.runout_noise           = FIL_RUNOUT_NOISE_THRESHOLD;
  infoSettings.runout_distance        = FIL_RUNOUT_DISTANCE;

// Power Loss Recovery & BTT UPS Settings
  infoSettings.plr                    = PL_RECOVERY;
  infoSettings.plr_home               = PL_RECOVERY_HOME;
  infoSettings.plr_z_raise            = PL_RECOVERY_Z_RAISE;
  infoSettings.btt_ups                = BTT_MINI_UPS;

// Other Device-Specific Settings
  infoSettings.sounds                 = ((TOUCH_SOUND << SOUND_TYPE_TOUCH) | (TOAST_SOUND << SOUND_TYPE_TOAST) |
                                         (ALERT_SOUND << SOUND_TYPE_ALERT) | (HEATER_SOUND << SOUND_TYPE_HEATER));
  infoSettings.lcd_brightness         = LCD_BRIGHTNESS;
  infoSettings.lcd_idle_brightness    = LCD_IDLE_BRIGHTNESS;
  infoSettings.lcd_idle_time          = LCD_IDLE_TIME;
  infoSettings.lcd_lock_on_idle       = LCD_LOCK_ON_IDLE;
  infoSettings.led_always_on          = LED_ALWAYS_ON;
  infoSettings.knob_led_color         = KNOB_LED_COLOR;
  infoSettings.knob_led_idle          = KNOB_LED_IDLE;
  #ifdef NEOPIXEL_PIXELS
    infoSettings.neopixel_pixels      = NEOPIXEL_PIXELS;
  #else
    infoSettings.neopixel_pixels      = 0;
  #endif

// Start, End & Cancel G-code Commands
  infoSettings.send_gcodes            = ((START_GCODE_ENABLED << SEND_GCODES_START_PRINT) | (END_GCODE_ENABLED << SEND_GCODES_END_PRINT) |
                                         (CANCEL_GCODE_ENABLED << SEND_GCODES_CANCEL_PRINT));

// All serial ports initialization
  for (int i = 0; i < MAX_SERIAL_PORT_COUNT; i++)
  {
    infoSettings.serial_port[i]       = default_serial_port[i];
  }

  for (int i = 0; i < MAX_HEATER_COUNT; i++)
  {
    infoSettings.max_temp[i]          = default_max_temp[i];
  }

  for (int i = 0; i < MAX_FAN_COUNT; i++)
  {
    infoSettings.fan_max[i]           = default_max_fan[i];
  }

  for (int i = 0; i < AXIS_NUM; i++)  // x, y, z
  {
    infoSettings.machine_size_min[i]  = default_size_min[i];
    infoSettings.machine_size_max[i]  = default_size_max[i];
  }

  for (int i = 0; i < SPEED_COUNT; i++)
  {
    infoSettings.xy_speed[i]          = default_xy_speed[i];
    infoSettings.z_speed[i]           = default_z_speed[i];
    infoSettings.ext_speed[i]         = default_ext_speed[i];
  }

  for (int i = 0; i < FEEDRATE_COUNT; i++)
  {
    infoSettings.pause_feedrate[i]    = default_pause_speed[i];  // XY, Z, E
  }

  for (int i = 0; i < FEEDRATE_COUNT - 1 ; i++)  // xy, z
  {
    infoSettings.level_feedrate[i]    = default_level_speed[i];
  }

  for (int i = 0; i < LED_COLOR_COMPONENT_COUNT - 1 ; i++)
  {
    infoSettings.led_color[i]         = default_led_color[i];
  }

  resetConfig();

  // Calculate checksum excluding the CRC variable in infoSettings
  infoSettings.CRC_checksum = calculateCRC16((uint8_t*)&infoSettings + sizeof(infoSettings.CRC_checksum),
                                                sizeof(infoSettings) - sizeof(infoSettings.CRC_checksum));
}

// Save settings to Flash only if CRC does not match
void saveSettings(void)
{
  // Calculate checksum excluding the CRC variable in infoSettings
  uint32_t curCRC = calculateCRC16((uint8_t*)&infoSettings + sizeof(infoSettings.CRC_checksum),
                                      sizeof(infoSettings) - sizeof(infoSettings.CRC_checksum));

  if (curCRC != infoSettings.CRC_checksum)  // save to Flash only if CRC does not match
  {
    infoSettings.CRC_checksum = curCRC;
    storePara();
  }
}

void initMachineSettings(void)
{
  // some settings are assumes as active unless reported disabled by marlin
  infoMachineSettings.firmwareType            = FW_NOT_DETECTED;  // set fimware type to not_detected to avoid repeated ABL gcode on mode change
  infoMachineSettings.EEPROM                  = ENABLED;
  infoMachineSettings.autoReportTemp          = DISABLED;
  infoMachineSettings.autoReportPos           = DISABLED;
  infoMachineSettings.leveling                = BL_DISABLED;
  infoMachineSettings.zProbe                  = ENABLED;
  infoMachineSettings.levelingData            = ENABLED;
  infoMachineSettings.softwarePower           = DISABLED;
  infoMachineSettings.toggleLights            = DISABLED;
  infoMachineSettings.caseLightsBrightness    = DISABLED;
  infoMachineSettings.emergencyParser         = DISABLED;
  infoMachineSettings.promptSupport           = DISABLED;
  infoMachineSettings.onboardSD               = DISABLED;
  infoMachineSettings.multiVolume             = DISABLED;
  infoMachineSettings.autoReportSDStatus      = DISABLED;
  infoMachineSettings.longFilename            = DISABLED;
  infoMachineSettings.babyStepping            = DISABLED;
  infoMachineSettings.buildPercent            = DISABLED;
  infoMachineSettings.softwareEndstops        = ENABLED;

  // reset the state to restart the temperature polling process
  // needed by parseAck() function to establish the connection
  heatSetUpdateWaiting(false);
}

void setupMachine(FW_TYPE fwType)
{
  if (infoMachineSettings.firmwareType != FW_NOT_DETECTED)  // avoid repeated calls caused by manually sending M115 in terminal menu
    return;

  if (GET_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE) == 1)  // if TFT in listening mode, display a reminder message
    setReminderMsg(LABEL_LISTENING, SYS_STATUS_LISTENING);

  infoMachineSettings.firmwareType = fwType;

  #if BED_LEVELING_TYPE > 1  // if not disabled and not auto-detect
    #if BED_LEVELING_TYPE == 2
      infoMachineSettings.leveling = BL_ABL;
    #elif BED_LEVELING_TYPE == 3
      infoMachineSettings.leveling = BL_BBL;
    #elif BED_LEVELING_TYPE == 4
      infoMachineSettings.leveling = BL_UBL;
    #elif BED_LEVELING_TYPE == 5
      infoMachineSettings.leveling = BL_MBL;
    #endif
  #endif

  if (infoSettings.onboard_sd != AUTO)
    infoMachineSettings.onboardSD = infoSettings.onboard_sd;

  if (infoSettings.long_filename != AUTO)
    infoMachineSettings.longFilename = infoSettings.long_filename;

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)  // RRF does not report detailed M115 capabilities
  { // set only the values that differ from the ones initialized in initMachineSettings() function
    #if BED_LEVELING_TYPE == 1  // if auto-detect is enabled
      infoMachineSettings.leveling = BL_ABL;
    #endif

    mustStoreCmd("M552\n");  // query network state, populate IP if the screen boots up after RRF
  }
  else if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)  // Smoothieware does not report detailed M115 capabilities
  { // set only the values that differ from the ones initialized in initMachineSettings() function
    #if BED_LEVELING_TYPE == 1  // if auto-detect is enabled
      infoMachineSettings.leveling = BL_ABL;
    #endif
  }

  if (infoMachineSettings.leveling != BL_DISABLED && infoMachineSettings.EEPROM == 1 && infoSettings.auto_load_leveling == 1)
    mustStoreCmd("M420 S1\n");

  mustStoreCmd("M503 S0\n");
  mustStoreCmd("G90\n");  // set to absolute positioning

  if (infoSettings.hotend_count > 0)
  {
    // this is good, but we need to set the extruder number first, and it's defaulting to 1
    mustStoreCmd("M82\n");  // set extruder to absolute mode
  }

  if (infoSettings.led_always_on == 1)
    LED_SendColor(&ledColor);  // set (neopixel) LED light to current color (initialized in HW_Init function)
}

float flashUsedPercentage(void)
{
  uint32_t total = W25Qxx_ReadCapacity();
  float percent = ((float)FLASH_USED * 100) / total;

  return percent;
}

// check font/icon/config signature in SPI flash for update
void checkflashSign(void)
{
  //cur_flash_sign[lang_sign] = flash_sign[lang_sign];  // ignore language signature not implemented yet

  bool statusfont = getFlashSignStatus(font_sign);
  bool statusconfig = getFlashSignStatus(config_sign);
  bool statuslang = getFlashSignStatus(lang_sign);
  bool statusicon = getFlashSignStatus(icon_sign);

  if (!statuslang)
    infoSettings.language = LANG_DEFAULT;

  if (!statusfont || !statusicon || !statusconfig)
  {
    int ypos = BYTE_HEIGHT + 5;
    GUI_Clear(BLACK);
    GUI_DispString(5, 5, (uint8_t *)"Found outdated data:");

    ypos += BYTE_HEIGHT;
    if (statusfont)
      GUI_DispString(10, ypos, (uint8_t *)"Fonts: OK");
    else
      GUI_DispString(10, ypos, (uint8_t *)"Fonts: Update required");

    ypos += BYTE_HEIGHT;
    if (statusconfig)
      GUI_DispString(10, ypos, (uint8_t *)"Config: OK");
    else
      GUI_DispString(10, ypos, (uint8_t *)"Config: Update required");

    ypos += BYTE_HEIGHT;
    if (statuslang)
      GUI_DispString(10, ypos, (uint8_t *)"Language: OK");
    else
      GUI_DispString(10, ypos, (uint8_t *)"Language: Update required(Optional)");

    ypos += BYTE_HEIGHT;
    if (statusicon)
      GUI_DispString(10, ypos, (uint8_t *)"Icons: OK");
    else
      GUI_DispString(10, ypos, (uint8_t *)"Icons: Update required");

    ypos += BYTE_HEIGHT;
    GUI_DispStringInRectEOL(10, ypos + 10, LCD_WIDTH, LCD_HEIGHT, (uint8_t *)"Insert the SD card with the required\n"
                                                                             "files and press the reset button\nto update.");
    while (1);
  }
}

bool getFlashSignStatus(int index)
{
  uint32_t flash_sign[sign_count] = {FONT_CHECK_SIGN, CONFIG_CHECK_SIGN, LANGUAGE_CHECK_SIGN, ICON_CHECK_SIGN};
  uint32_t cur_flash_sign[sign_count];
  uint32_t addr = FLASH_SIGN_ADDR;
  uint32_t len = sizeof(flash_sign);

  W25Qxx_ReadBuffer((uint8_t*)&cur_flash_sign, addr, len);
  return (flash_sign[index] == cur_flash_sign[index]);
}
