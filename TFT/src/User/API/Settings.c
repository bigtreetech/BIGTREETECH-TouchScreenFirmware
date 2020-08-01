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
  infoSettings.baudrate             = BAUDRATE;
  infoSettings.language             = DEFAULT_LANGUAGE;
  infoSettings.mode                 = DEFAULT_LCD_MODE;
  infoSettings.unified_menu         = UNIFIED_MENU;

  infoSettings.bg_color             = lcd_colors[BACKGROUND_COLOR];
  infoSettings.font_color           = lcd_colors[FONT_COLOR];
  infoSettings.title_bg_color       = lcd_colors[TITLE_BACKGROUND_COLOR];
  infoSettings.reminder_color       = lcd_colors[REMINDER_FONT_COLOR];
  infoSettings.sd_reminder_color    = lcd_colors[VOLUME_REMINDER_FONT_COLOR];
  infoSettings.status_xyz_bg_color  = lcd_colors[STATUS_XYZ_BG_COLOR];
  infoSettings.list_border_color    = lcd_colors[LISTVIEW_BORDER_COLOR];
  infoSettings.list_button_color    = lcd_colors[LISTVIEW_ICON_COLOR];

  infoSettings.silent               = DISABLED;
  infoSettings.terminalACK          = DISABLED;
  infoSettings.move_speed           = ENABLED;
  infoSettings.knob_led_color       = STARTUP_KNOB_LED_COLOR;
  infoSettings.send_start_gcode     = DISABLED;
  infoSettings.send_end_gcode       = DISABLED;
  infoSettings.send_cancel_gcode    = ENABLED;
  infoSettings.persistent_info      = ENABLED;
  infoSettings.file_listmode        = ENABLED;

  infoSettings.lcd_brightness       = DEFAULT_LCD_BRIGHTNESS;
  infoSettings.lcd_idle_brightness  = DEFAULT_LCD_IDLE_BRIGHTNESS;
  infoSettings.lcd_idle_timer       = DEFAULT_LCD_IDLE_TIMER;

  infoSettings.serial_alwaysOn        = SERIAL_ALWAYS_ON;
  infoSettings.marlin_mode_bg_color   = lcd_colors[ST7920_BKCOLOR];
  infoSettings.marlin_mode_font_color = lcd_colors[ST7920_FNCOLOR];
  infoSettings.marlin_mode_showtitle  = ST7920_SHOW_BANNER;
  infoSettings.marlin_mode_fullscreen = DEFAULT_ST7920_FULLSCREEN_MODE;

  infoSettings.auto_off               = DISABLED;
  infoSettings.ps_active_high         = PS_ON_ACTIVE_HIGH;
  infoSettings.auto_off_temp          = AUTO_SHUT_DOWN_MAXTEMP;

  infoSettings.runout                 = DISABLED;
  infoSettings.runout_invert          = FIL_RUNOUT_INVERTING;
  infoSettings.runout_noise_ms        = FIL_NOISE_THRESHOLD;
  infoSettings.runout_distance        = FILAMENT_RUNOUT_DISTANCE_MM;

  infoSettings.powerloss_en           = ENABLED;
  infoSettings.powerloss_home         = HOME_BEFORE_PLR;
  infoSettings.powerloss_invert       = PS_ON_ACTIVE_HIGH;
  infoSettings.powerloss_z_raise      = POWER_LOSS_ZRAISE;
  infoSettings.btt_ups                = BTT_MINI_UPS;

//machine specific settings

  infoSettings.hotend_count           = HOTEND_NUM;
  infoSettings.bed_en                 = ENABLE;
  infoSettings.chamber_en             = DISABLE;
  infoSettings.ext_count              = EXTRUDER_NUM;
  infoSettings.fan_count              = FAN_NUM;
  infoSettings.auto_load_leveling     = AUTO_SAVE_LOAD_LEVELING_VALUE;
  infoSettings.onboardSD              = AUTO;     //ENABLED / DISABLED / AUTO
  infoSettings.m27_refresh_time       = M27_REFRESH;
  infoSettings.m27_active             = M27_WATCH_OTHER_SOURCES;
  infoSettings.longFileName           = AUTO;     //ENABLED / DISABLED / AUTO

  for(int i = 0; i < MAX_HEATER_COUNT; i++)
  {
    infoSettings.max_temp[i]          = default_max_temp[i];
  }

  infoSettings.min_ext_temp           = PREVENT_COLD_EXTRUSION_MINTEMP;

  for(int i = 0; i < MAX_FAN_COUNT ;i++)
  {
    infoSettings.fan_max[i]           = default_max_fanPWM[i];
  }

  infoSettings.fan_percentage         = ENABLED;

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

  infoSettings.pause_retract_len      = NOZZLE_PAUSE_RETRACT_LENGTH;
  infoSettings.resume_purge_len       = NOZZLE_RESUME_PURGE_LENGTH;


  infoSettings.pause_pos[X_AXIS]      = NOZZLE_PAUSE_X_POSITION;  // X
  infoSettings.pause_pos[Y_AXIS]      = NOZZLE_PAUSE_Y_POSITION;  // Y
  infoSettings.pause_z_raise          = NOZZLE_PAUSE_Z_RAISE;

  for(int i = 0; i < TOTAL_AXIS ;i++)
  {
    infoSettings.pause_feedrate[i]    = default_pause_speed[i]; // X, Y, Z, E
  }

  infoSettings.level_edge             = LEVELING_EDGE_DISTANCE;
  infoSettings.level_z_pos            = LEVELING_POINT_Z;
  infoSettings.level_z_raise          = LEVELING_POINT_MOVE_Z;

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    infoSettings.preheat_temp[i]      = default_preheat_ext[i];
    infoSettings.preheat_bed[i]       = default_preheat_bed[i];
  }
  resetConfig();
}

void initMachineSetting(void){
  // some settings are assumes as active unless reported disabled by marlin
  infoMachineSettings.EEPROM                  = ENABLED;
  infoMachineSettings.autoReportTemp          = DISABLED;
  infoMachineSettings.autoLevel               = ENABLED;
  infoMachineSettings.zProbe                  = ENABLED;
  infoMachineSettings.levelingData            = ENABLED;
  infoMachineSettings.softwarePower           = DISABLED;
  infoMachineSettings.toggleLights            = DISABLED;
  infoMachineSettings.caseLightsBrightness    = DISABLED;
  infoMachineSettings.emergencyParser         = DISABLED;
  infoMachineSettings.promptSupport           = DISABLED;
  infoMachineSettings.onboard_sd_support      = ENABLED;
  infoMachineSettings.autoReportSDStatus      = DISABLED;
  infoMachineSettings.enableubl               = DISABLED;
}

void setupMachine(void)
{
  #ifdef ENABLE_UBL_VALUE
    if (infoMachineSettings.autoLevel == 1 && ENABLE_UBL_VALUE == 1) {
      infoMachineSettings.enableubl = ENABLE;
    }
  #endif
  #ifdef AUTO_SAVE_LOAD_LEVELING_VALUE
    if (infoMachineSettings.autoLevel == 1 && infoMachineSettings.EEPROM == 1 && infoSettings.auto_load_leveling == 1){
      storeCmd("M420 S1\n");
    }
  #endif
  if(infoMachineSettings.isMarlinFirmware == 1)
  {
    printSetUpdateWaiting(infoSettings.m27_active);
  }
  else //Smoothieware does not report detailed M115 capabilities
  {
    infoMachineSettings.EEPROM                  = ENABLED;
    infoMachineSettings.autoReportTemp          = DISABLED;
    infoMachineSettings.autoLevel               = ENABLED;
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
