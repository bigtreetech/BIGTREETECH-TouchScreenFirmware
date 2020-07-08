#include "Settings.h"
#include "includes.h"

SETTINGS infoSettings;
MACHINESETTINGS infoMachineSettings;

// Reset settings data
void infoSettingsReset(void)
{
  infoSettings.baudrate             = BAUDRATE;
  infoSettings.language             = DEFAULT_LANGUAGE;
  infoSettings.mode                 = DEFAULT_LCD_MODE;
  infoSettings.runout               = 0;
  infoSettings.rotate_ui            = 0;
  infoSettings.bg_color             = ST7920_BKCOLOR;
  infoSettings.font_color           = ST7920_FNCOLOR;
  infoSettings.silent               = 0;
  infoSettings.auto_off             = 0;
  infoSettings.terminalACK          = 0;
  infoSettings.invert_axis[X_AXIS]  = 0;
  infoSettings.invert_axis[Y_AXIS]  = 0;
  infoSettings.invert_axis[Z_AXIS]  = 0;
  infoSettings.move_speed           = 0;
  infoSettings.knob_led_color       = (STARTUP_KNOB_LED_COLOR - 1);
  infoSettings.send_start_gcode     = 0;
  infoSettings.send_end_gcode       = 0;
  infoSettings.send_cancel_gcode    = 0;
  infoSettings.persistent_info      = 1;
  infoSettings.file_listmode        = 1;
  #ifdef LCD_LED_PWM_CHANNEL
  infoSettings.lcd_brightness       = (DEFAULT_LCD_BRIGHTNESS - 1);
  infoSettings.lcd_idle_brightness  = (DEFAULT_LCD_IDLE_BRIGHTNESS - 1);
  infoSettings.lcd_idle_timer       = (DEFAULT_LCD_IDLE_TIMER - 1);
  #endif
  infoSettings.marlin_mode_fullscreen = DEFAULT_ST7920_FULLSCREEN_MODE;
}

void initMachineSetting(void){

  infoMachineSettings.EEPROM                  = 0;
  infoMachineSettings.autoReportTemp          = 0;
  infoMachineSettings.autoLevel               = 0;
  infoMachineSettings.zProbe                  = 0;
  infoMachineSettings.levelingData            = 0;
  infoMachineSettings.softwarePower           = 0;
  infoMachineSettings.toggleLights            = 0;
  infoMachineSettings.caseLightsBrightness    = 0;
  infoMachineSettings.emergencyParser         = 0;
  infoMachineSettings.promptSupport           = 0;
  infoMachineSettings.autoReportSDStatus      = 0;
}

void setupMachine(void){
  #ifdef AUTO_SAVE_LOAD_LEVELING_VALUE
    if (infoMachineSettings.autoLevel == 1 && infoMachineSettings.EEPROM == 1){
      storeCmd("M420 S1\n");
    }
  #endif
  if (infoMachineSettings.emergencyParser != 1 && wasRestored == true){
    popupReminder(textSelect(LABEL_WARNING), textSelect(LABEL_EMERGENCYPARSER));
  }
}

// Version infomation
void menuInfo(void)
{
  const char* hardware = "Board   : BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = "Firmware: "HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;

  u16 HW_X = (LCD_WIDTH - GUI_StrPixelWidth((u8 *)hardware))/2;
  u16 FW_X = (LCD_WIDTH - GUI_StrPixelWidth((u8 *)firmware))/2;
  u16 centerY = LCD_HEIGHT/2;
  u16 startX = MIN(HW_X, FW_X);

  GUI_Clear(BACKGROUND_COLOR);

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
  GUI_Clear(BACKGROUND_COLOR);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_ReSourceDeInit();
  while(!isPress());
  while(isPress());
  Serial_ReSourceInit();

  infoMenu.cur--;
}

MENUITEMS settingsItems = {
// title
LABEL_SETTINGS,
// icon                       label
 {{ICON_SCREEN_SETTINGS,      LABEL_SCREEN_SETTINGS},
  {ICON_MACHINE_SETTINGS,     LABEL_MACHINE_SETTINGS},
  {ICON_FEATURE_SETTINGS,     LABEL_FEATURE_SETTINGS},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BAUD_RATE,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_BAUDRATE_NUM 9
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
// icon                       label
  {ICON_BAUD_RATE,             {.address = "2400"}},
  {ICON_BAUD_RATE,             {.address = "9600"}},
  {ICON_BAUD_RATE,             {.address = "19200"}},
  {ICON_BAUD_RATE,             {.address = "38400"}},
  {ICON_BAUD_RATE,             {.address = "57600"}},
  {ICON_BAUD_RATE,             {.address = "115200"}},
  {ICON_BAUD_RATE,             {.address = "250000"}},
  {ICON_BAUD_RATE,             {.address = "500000"}},
  {ICON_BAUD_RATE,             {.address = "1000000"}},
};
const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000};
static u8  item_baudrate_i = 0;

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }

  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuScreenSettings;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuMachineSettings;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuFeatureSettings;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;

      case KEY_ICON_5:
        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;
        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.baudrate = item_baudrate[item_baudrate_i];
        Serial_ReSourceDeInit(); // Serial_Init() will malloc a dynamic memory, so Serial_DeInit() first to free, then malloc again.
        Serial_ReSourceInit();
        reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
