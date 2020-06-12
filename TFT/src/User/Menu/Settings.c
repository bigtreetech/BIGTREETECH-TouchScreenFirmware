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
const u8 defulat_custom_enabled[] = CUSTOM_GCODE_ENABLED;

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

  infoSettings.powerloss_home         = HOME_BEFORE_PLR;
  infoSettings.powerloss_invert       = PS_ON_ACTIVE_HIGH;
  infoSettings.powerloss_z_raise      = POWER_LOSS_ZRAISE;
  infoSettings.btt_ups                = BTT_MINI_UPS;

//machine specific settings

  infoSettings.tool_count             = TOOL_NUM;
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
  infoSettings.pause_pos[Y_AXIS]      = NOZZLE_PAUSE_X_POSITION;  // Y
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
}

void setupMachine(void){
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
}

// Version infomation
void menuInfo(void)
{
  char buf[128];
  const GUI_POINT clocks[] = {{0 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
                             {1 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
                             {2 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
                             {0 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},
                             {1 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},
                             {2 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},};
  const char* hardware = "Board   : BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = "Firmware: "HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;

  u16 HW_X = (LCD_WIDTH - GUI_StrPixelWidth((u8 *)hardware))/2;
  u16 FW_X = (LCD_WIDTH - GUI_StrPixelWidth((u8 *)firmware))/2;
  u16 centerY = LCD_HEIGHT/2;
  u16 startX = MIN(HW_X, FW_X);

  GUI_Clear(infoSettings.bg_color);

  my_sprintf(buf, "SYS:%dMhz", mcuClocks.rccClocks.SYSCLK_Frequency / 1000000);
  GUI_DispString(clocks[0].x, clocks[0].y, (uint8_t *)buf);

  my_sprintf(buf, "APB1:%dMhz", mcuClocks.rccClocks.PCLK1_Frequency / 1000000);
  GUI_DispString(clocks[1].x, clocks[1].y, (uint8_t *)buf);

  my_sprintf(buf, "P1Tim:%dMhz", mcuClocks.PCLK1_Timer_Frequency / 1000000);
  GUI_DispString(clocks[2].x, clocks[2].y, (uint8_t *)buf);

  my_sprintf(buf, "AHB:%dMhz", mcuClocks.rccClocks.HCLK_Frequency / 1000000);
  GUI_DispString(clocks[3].x, clocks[3].y, (uint8_t *)buf);

  my_sprintf(buf, "APB2:%dMhz", mcuClocks.rccClocks.PCLK2_Frequency / 1000000);
  GUI_DispString(clocks[4].x, clocks[4].y, (uint8_t *)buf);

  my_sprintf(buf, "P2Tim:%dMhz", mcuClocks.PCLK2_Timer_Frequency / 1000000);
  GUI_DispString(clocks[5].x, clocks[5].y, (uint8_t *)buf);

  GUI_DispString(startX, centerY - BYTE_HEIGHT, (u8 *)hardware);
  GUI_DispString(startX, centerY, (u8 *)firmware);
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  while(!isPress()) loopProcess();
  while(isPress())  loopProcess();

  infoMenu.cur--;
}

// Set uart pins to input, free uart
void menuDisconnect(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_ReSourceDeInit();
  while(!isPress());
  while(isPress());
  Serial_ReSourceInit();

  infoMenu.cur--;
}


const char* item_baudrate_str[ITEM_BAUDRATE_NUM] = {"2400", "9600", "19200", "38400", "57600", "115200", "250000", "500000", "1000000"};
const u32   item_baudrate[ITEM_BAUDRATE_NUM] = {2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000};

void menuBaudrate(void)
{
  LABEL title = {LABEL_BAUDRATE};
  LISTITEM totalItems[ITEM_BAUDRATE_NUM];
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t cur_item;

  // fill baudrate items
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (infoSettings.baudrate == item_baudrate[i]) {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = (uint8_t *)item_baudrate_str[i];
  }

  listWidgetCreat(title, totalItems, COUNT(totalItems), cur_item / LISTITEM_PER_PAGE);

  while (infoMenu.menu[infoMenu.cur] == menuBaudrate)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_5:
      listWidgetPreviousPage();
      break;

    case KEY_ICON_6:
      listWidgetNextPage();
      break;

    case KEY_ICON_7:
      infoMenu.cur--;
      break;

    default:
      if(key_num < LISTITEM_PER_PAGE){
        uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
        if (tmp_i != cur_item) { // has changed
          totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
          listWidgetRefreshItem(cur_item); // refresh unchecked status
          cur_item = tmp_i;
          totalItems[cur_item].icon = ICONCHAR_CHECKED;
          listWidgetRefreshItem(cur_item); // refresh checked status

          infoSettings.baudrate = item_baudrate[cur_item];
          Serial_ReSourceDeInit(); // Serial_Init() will malloc a dynamic memory, so Serial_DeInit() first to free, then malloc again.
          Serial_ReSourceInit();
          reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);
        }
      }
      break;
    }

    loopProcess();
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}


const MENUITEMS settingsItems = {
// title
LABEL_SETTINGS,
// icon                       label
 {{ICON_SCREEN_SETTINGS,      LABEL_SCREEN_SETTINGS},
  {ICON_MACHINE_SETTINGS,     LABEL_MACHINE_SETTINGS},
  {ICON_FEATURE_SETTINGS,     LABEL_FEATURE_SETTINGS},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BAUD_RATE,            LABEL_BAUDRATE},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuScreenSettings; break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMachineSettings; break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuFeatureSettings; break;
      case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuInfo; break;
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuDisconnect; break;
      case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuBaudrate; break;

      case KEY_ICON_7: infoMenu.cur--; break;

      default: break;
    }
    loopProcess();
  }
}
