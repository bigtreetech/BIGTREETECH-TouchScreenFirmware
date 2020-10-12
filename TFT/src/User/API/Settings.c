#include "Settings.h"
#include "includes.h"

SETTINGS infoSettings;
MACHINESETTINGS infoMachineSettings;

const u16 default_max_temp[]      = HEAT_MAX_TEMP;
const u16 default_max_fanPWM[]    = FAN_MAX_PWM;
const u16 default_size_min[]      = {X_MIN_POS,Y_MIN_POS,Z_MIN_POS};
const u16 default_size_max[]      = {X_MAX_POS,Y_MAX_POS,Z_MAX_POS};
const u16 default_move_speed[]    = {SPEED_MOVE_SLOW, DEFAULT_SPEED_MOVE, SPEED_MOVE_FAST};
const u16 default_ext_speed[]     = {EXTRUDE_SLOW_SPEED, EXTRUDE_NORMAL_SPEED, EXTRUDE_FAST_SPEED};
const u16 default_level_speed[]   = {LEVELING_POINT_XY_FEEDRATE,LEVELING_POINT_XY_FEEDRATE,LEVELING_POINT_Z_FEEDRATE};
const u16 default_pause_speed[]   = {NOZZLE_PAUSE_XY_FEEDRATE, NOZZLE_PAUSE_XY_FEEDRATE, NOZZLE_PAUSE_Z_FEEDRATE, NOZZLE_PAUSE_E_FEEDRATE};
const u16 default_preheat_ext[]   = PREHEAT_HOTEND;
const u16 default_preheat_bed[]   = PREHEAT_BED;
const u8 default_custom_enabled[] = CUSTOM_GCODE_ENABLED;

// Reset settings data
void infoSettingsReset(void)
{
// General Settings
  infoSettings.unified_menu           = UNIFIED_MENU;
  infoSettings.baudrate               = BAUDRATE;
  infoSettings.language               = LANG_DEFAULT;

  infoSettings.title_bg_color         = lcd_colors[TITLE_BACKGROUND_COLOR];
  infoSettings.bg_color               = lcd_colors[BACKGROUND_COLOR];
  infoSettings.font_color             = lcd_colors[FONT_COLOR];
  infoSettings.reminder_color         = lcd_colors[REMINDER_FONT_COLOR];
  infoSettings.sd_reminder_color      = lcd_colors[VOLUME_REMINDER_FONT_COLOR];
  infoSettings.status_xyz_bg_color    = lcd_colors[STATUS_XYZ_BG_COLOR];
  infoSettings.list_border_color      = lcd_colors[LISTVIEW_BORDER_COLOR];
  infoSettings.list_button_color      = lcd_colors[LISTVIEW_ICON_COLOR];

  infoSettings.terminalACK            = DISABLED;
  infoSettings.persistent_info        = ENABLED;
  infoSettings.file_listmode          = ENABLED;
  infoSettings.ack_notification       = ACK_NOTIFICATION_STYLE;

// Marlin Mode Settings
  infoSettings.mode                   = DEFAULT_LCD_MODE;
  infoSettings.serial_alwaysOn        = SERIAL_ALWAYS_ON;
  infoSettings.marlin_mode_bg_color   = lcd_colors[MARLIN_BKCOLOR];
  infoSettings.marlin_mode_font_color = lcd_colors[MARLIN_FNCOLOR];
  infoSettings.marlin_mode_showtitle  = MARLIN_SHOW_BANNER;
  infoSettings.marlin_mode_fullscreen = DEFAULT_ST7920_FULLSCREEN_MODE;
  infoSettings.marlin_type            = LCD12864;

// Printer / Machine Settings
  infoSettings.hotend_count           = HOTEND_NUM;
  infoSettings.bed_en                 = ENABLE;
  infoSettings.chamber_en             = DISABLE;
  infoSettings.ext_count              = EXTRUDER_NUM;
  infoSettings.fan_count              = FAN_NUM;
  infoSettings.fan_ctrl_count         = FAN_CTRL_NUM;
  infoSettings.min_ext_temp           = PREVENT_COLD_EXTRUSION_MINTEMP;
  infoSettings.auto_load_leveling     = AUTO_SAVE_LOAD_BL_VALUE;
  infoSettings.onboardSD              = AUTO;                        //ENABLED / DISABLED / AUTO
  infoSettings.m27_refresh_time       = M27_REFRESH;
  infoSettings.m27_active             = M27_WATCH_OTHER_SOURCES;
  infoSettings.longFileName           = AUTO;                        //ENABLED / DISABLED / AUTO
  infoSettings.fan_percentage         = ENABLED;

  infoSettings.pause_retract_len      = NOZZLE_PAUSE_RETRACT_LENGTH;
  infoSettings.resume_purge_len       = NOZZLE_RESUME_PURGE_LENGTH;
  infoSettings.pause_pos[X_AXIS]      = NOZZLE_PAUSE_X_POSITION;     // X
  infoSettings.pause_pos[Y_AXIS]      = NOZZLE_PAUSE_Y_POSITION;     // Y
  infoSettings.pause_z_raise          = NOZZLE_PAUSE_Z_RAISE;

  infoSettings.level_edge             = LEVELING_EDGE_DISTANCE;
  infoSettings.level_z_pos            = LEVELING_POINT_Z;
  infoSettings.level_z_raise          = LEVELING_POINT_MOVE_Z;

  infoSettings.move_speed             = ENABLED;                     // index on infoSettings.axis_speed, infoSettings.ext_speed

// Power Supply Settings
  infoSettings.auto_off               = DISABLED;
  infoSettings.ps_active_high         = PS_ON_ACTIVE_HIGH;
  infoSettings.auto_off_temp          = AUTO_SHUT_DOWN_MAXTEMP;

// Filament Runout Settings
  infoSettings.runout                 = DISABLED;
  infoSettings.runout_invert          = FIL_RUNOUT_INVERTING;
  infoSettings.runout_noise_ms        = FIL_NOISE_THRESHOLD;
  infoSettings.runout_distance        = FILAMENT_RUNOUT_DISTANCE_MM;

// Power Loss Recovery & BTT UPS Settings
  infoSettings.powerloss_en           = ENABLED;
  infoSettings.powerloss_home         = HOME_BEFORE_PLR;
  infoSettings.powerloss_invert       = PS_ON_ACTIVE_HIGH;
  infoSettings.powerloss_z_raise      = POWER_LOSS_ZRAISE;
  infoSettings.btt_ups                = BTT_MINI_UPS;

// Other device-specific settings
  infoSettings.touchSound             = ENABLED;
  infoSettings.toastSound             = ENABLED;
  infoSettings.alertSound             = ENABLED;
  infoSettings.knob_led_color         = STARTUP_KNOB_LED_COLOR;
  infoSettings.knob_led_idle          = ENABLED;
  infoSettings.lcd_brightness         = DEFAULT_LCD_BRIGHTNESS;
  infoSettings.lcd_idle_brightness    = DEFAULT_LCD_IDLE_BRIGHTNESS;
  infoSettings.lcd_idle_timer         = DEFAULT_LCD_IDLE_TIMER;

// Start, End & Cancel G-codes
  infoSettings.send_start_gcode       = DISABLED;
  infoSettings.send_end_gcode         = DISABLED;
  infoSettings.send_cancel_gcode      = ENABLED;

// All the remaining array initializations
  for(int i = 0; i < MAX_HEATER_COUNT; i++)
  {
    infoSettings.max_temp[i]          = default_max_temp[i];
  }

  for(int i = 0; i < MAX_FAN_COUNT ;i++)
  {
    infoSettings.fan_max[i]           = default_max_fanPWM[i];
  }

  for(int i = 0; i < AXIS_NUM ;i++) //x, y, z
  {
    infoSettings.invert_axis[i]       = DISABLED;
    infoSettings.machine_size_min[i]  = default_size_min[i];
    infoSettings.machine_size_max[i]  = default_size_max[i];
    infoSettings.level_feedrate[i]    = default_level_speed[i];
  }

  for(int i = 0; i < SPEED_COUNT ;i++)
  {
    infoSettings.axis_speed[i]        = default_move_speed[i];
    infoSettings.ext_speed[i]         = default_ext_speed[i];
  }

  for(int i = 0; i < TOTAL_AXIS ;i++)
  {
    infoSettings.pause_feedrate[i]    = default_pause_speed[i];      // X, Y, Z, E
  }

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    infoSettings.preheat_temp[i]      = default_preheat_ext[i];
    infoSettings.preheat_bed[i]       = default_preheat_bed[i];
  }
  resetConfig();
}

void initMachineSetting(void){
  // some settings are assumes as active unless reported disabled by marlin
  infoMachineSettings.isMarlinFirmware        = -1; // set fimware type to -1 to avoid repeated ABL gcode on mode change
  infoMachineSettings.EEPROM                  = ENABLED;
  infoMachineSettings.autoReportTemp          = DISABLED;
  infoMachineSettings.leveling                = BL_DISABLED;
  infoMachineSettings.zProbe                  = ENABLED;
  infoMachineSettings.levelingData            = ENABLED;
  infoMachineSettings.softwarePower           = DISABLED;
  infoMachineSettings.toggleLights            = DISABLED;
  infoMachineSettings.caseLightsBrightness    = DISABLED;
  infoMachineSettings.emergencyParser         = DISABLED;
  infoMachineSettings.promptSupport           = DISABLED;
  infoMachineSettings.onboard_sd_support      = DISABLED;
  infoMachineSettings.autoReportSDStatus      = DISABLED;
  infoMachineSettings.long_filename_support   = DISABLED;
  infoMachineSettings.babyStepping            = DISABLED;
}

void setupMachine(void)
{
  switch (ENABLE_BL_VALUE)
  {
    case 2:
      infoMachineSettings.leveling = BL_ABL;
      break;

    case 3:
      infoMachineSettings.leveling = BL_BBL;
      break;

    case 4:
      infoMachineSettings.leveling = BL_UBL;
      break;

    case 5:
      infoMachineSettings.leveling = BL_MBL;
      break;

    default:
      break;
  }

  if (infoMachineSettings.leveling != BL_DISABLED && infoMachineSettings.EEPROM == 1 && infoSettings.auto_load_leveling == 1)
  {
    storeCmd("M420 S1\n");
  }

  if (infoMachineSettings.isMarlinFirmware != 1) // Smoothieware does not report detailed M115 capabilities
  {
    infoMachineSettings.EEPROM                  = ENABLED;
    infoMachineSettings.autoReportTemp          = DISABLED;
    infoMachineSettings.leveling                = ENABLED;
    infoMachineSettings.zProbe                  = ENABLED;
    infoMachineSettings.levelingData            = ENABLED;
    infoMachineSettings.emergencyParser         = ENABLED;
    infoMachineSettings.autoReportSDStatus      = DISABLED;
  }
  if (infoSettings.onboardSD == ENABLED)
  {
    infoMachineSettings.onboard_sd_support = ENABLED;
  }
  else if (infoSettings.onboardSD == DISABLED)
  {
    infoMachineSettings.onboard_sd_support = DISABLED;
  }
  if (infoSettings.longFileName == ENABLED)
  {
    infoMachineSettings.long_filename_support = ENABLED;
  }
  else if (infoSettings.longFileName == DISABLED)
  {
    infoMachineSettings.long_filename_support = DISABLED;
  }
  mustStoreCmd("M503 S0\n");
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
  //cur_flash_sign[lang_sign] = flash_sign[lang_sign]; // ignore language signature not implemented yet

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
