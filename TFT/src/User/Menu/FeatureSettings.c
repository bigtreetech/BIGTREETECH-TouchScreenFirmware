#include "FeatureSettings.h"
#include "includes.h"


LISTITEMS featureSettingsItems = {
// title
LABEL_FEATURE_SETTINGS,
// icon                 ItemType      Item Title        item value text(only for custom value)
{
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_PAGEUP,     LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_PAGEDOWN,   LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},
  {ICONCHAR_BACK,       LIST_LABEL,  LABEL_BACKGROUND, LABEL_BACKGROUND},}
};

//
//setup item states
//
#define TOGGLE_NUM   2
const uint16_t toggleitem[TOGGLE_NUM] = {ICONCHAR_TOGGLE_OFF, ICONCHAR_TOGGLE_ON};

#ifdef FIL_RUNOUT_PIN
  #define ITEM_RUNOUT_NUM 3
  const LABEL itemRunout[ITEM_RUNOUT_NUM] = {
                                              //item value text(only for custom value)
                                              LABEL_OFF,
                                              LABEL_ON,
                                              LABEL_SMART
                                            };

#endif  // FIL_RUNOUT_PIN

#define ITEM_SPEED_NUM 3
const LABEL itemMoveSpeed[ITEM_SPEED_NUM] = {
                                              //item value text(only for custom value)
                                              LABEL_SLOW_SPEED,
                                              LABEL_NORMAL_SPEED,
                                              LABEL_FAST_SPEED
                                            };

#define NOTIFICATION_TYPE_NUM 3
const char *const notificationType[NOTIFICATION_TYPE_NUM] = {
                                                              //item value text(only for custom value)
                                                              "OFF",
                                                              "POPUP",
                                                              "TOAST"
                                                            };

//
//add key number index of the items
//
typedef enum
{
  SKEY_ACK = 0,
  SKEY_INVERT_X,
  SKEY_INVERT_Y,
  SKEY_INVERT_Z,
  #ifdef PS_ON_PIN
    SKEY_POWER,
  #endif
  #ifdef FIL_RUNOUT_PIN
    SKEY_RUNOUT,
  #endif
  SKEY_SPEED,
  SKEY_STARTGCODE,
  SKEY_ENDGCODE,
  SKEY_CANCELGCODE,
  SKEY_PERSISTENTINFO,
  SKEY_FILELIST,
  SKEY_ACK_NOTIFICATION,
  #ifdef LED_COLOR_PIN
    SKEY_KNOB,
  #ifdef LCD_LED_PWM_CHANNEL
    SKEY_KNOB_LED_IDLE,
  #endif
  #endif
  #ifdef LCD_LED_PWM_CHANNEL
    SKEY_LCD_BRIGHTNESS,
    SKEY_LCD_BRIGTHNESS_DIM,
    SKEY_LCD_DIM_IDLE_TIMER,
  #endif
  #ifdef ST7920_SPI
    SKEY_ST7920_FULLSCREEN,
  #endif
  SKEY_PLR_EN,
  SKEY_RESET_SETTINGS, // Keep reset always at the bottom of the settings menu list.
  SKEY_COUNT //keep this always at the end
}SKEY_LIST;

#define FE_PAGE_COUNT  (SKEY_COUNT+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE
int fe_cur_page = 0;

//
//set item types
//
LISTITEM settingPage[SKEY_COUNT] = {
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_TERMINAL_ACK,             LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_INVERT_XAXIS,             LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_INVERT_YAXIS,             LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_INVERT_ZAXIS,             LABEL_BACKGROUND},
  #ifdef PS_ON_PIN
    {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,      LABEL_AUTO_SHUT_DOWN,           LABEL_BACKGROUND},
  #endif
  #ifdef FIL_RUNOUT_PIN
    {ICONCHAR_BLANK,     LIST_CUSTOMVALUE,   LABEL_FILAMENT_SENSOR,          LABEL_OFF},
  #endif
  {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_MOVE_SPEED,               LABEL_NORMAL_SPEED},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_SEND_START_GCODE,         LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_SEND_END_GCODE,           LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_SEND_CANCEL_GCODE,        LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_PERSISTENT_STATUS_INFO,   LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_FILE_LISTMODE,            LABEL_BACKGROUND},
  {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_ACK_NOTIFICATION,         LABEL_DYNAMIC},
  #ifdef LED_COLOR_PIN
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_KNOB_LED,                 LABEL_OFF},
  #ifdef LCD_LED_PWM_CHANNEL
    {ICONCHAR_BLANK,      LIST_TOGGLE,        LABEL_KNOB_LED_IDLE,            LABEL_BACKGROUND},
  #endif
  #endif
  #ifdef LCD_LED_PWM_CHANNEL
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_BRIGHTNESS,           LABEL_DYNAMIC},
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_BRIGHTNESS_DIM,       LABEL_DYNAMIC},
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_DIM_IDLE_TIMER,       LABEL_DYNAMIC},
  #endif
  #ifdef ST7920_SPI
    {ICONCHAR_BLANK,      LIST_TOGGLE,        LABEL_ST7920_FULLSCREEN,        LABEL_OFF},
  #endif
  {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE,        LABEL_PLR_EN,                   LABEL_BACKGROUND},
  // Keep reset settings always at the bottom of the settings menu list.
  {ICONCHAR_BLANK,      LIST_MOREBUTTON,    LABEL_SETTING_RESET,            LABEL_BACKGROUND}
};

void resetSettings(void)
{
  infoSettingsReset();
  storePara();
  popupReminder(DIALOG_TYPE_SUCCESS, textSelect(LABEL_INFO), textSelect(LABEL_RESET_SETTINGS_DONE));
}

//
//perform action on button press
//
void updateFeatureSettings(uint8_t key_val)
{
  uint8_t item_index = fe_cur_page*LISTITEM_PER_PAGE+ key_val;
  switch (item_index)
  {
    case SKEY_ACK:
      infoSettings.terminalACK = (infoSettings.terminalACK + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.terminalACK];
      break;

    case SKEY_INVERT_X:
      infoSettings.invert_axis[X_AXIS] = (infoSettings.invert_axis[X_AXIS] + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[X_AXIS]];
     break;

    case SKEY_INVERT_Y:
      infoSettings.invert_axis[Y_AXIS] = (infoSettings.invert_axis[Y_AXIS] + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[Y_AXIS]];
      break;

    case SKEY_INVERT_Z:
      infoSettings.invert_axis[Z_AXIS] = (infoSettings.invert_axis[Z_AXIS] + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[Z_AXIS]];
      break;

    #ifdef PS_ON_PIN
      case SKEY_POWER:
      infoSettings.auto_off = (infoSettings.auto_off + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.auto_off];
      break;
    #endif

    #ifdef FIL_RUNOUT_PIN
      case SKEY_RUNOUT:
        infoSettings.runout = (infoSettings.runout + 1) % ITEM_RUNOUT_NUM;
        settingPage[item_index].valueLabel = itemRunout[infoSettings.runout];
        break;
    #endif

    case SKEY_SPEED:
      infoSettings.move_speed = (infoSettings.move_speed + 1) % ITEM_SPEED_NUM;
      settingPage[item_index].valueLabel = itemMoveSpeed[infoSettings.move_speed];
      break;

    case SKEY_STARTGCODE:
      infoSettings.send_start_gcode = (infoSettings.send_start_gcode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.send_start_gcode];
      break;

    case SKEY_ENDGCODE:
      infoSettings.send_end_gcode = (infoSettings.send_end_gcode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.send_end_gcode];
      break;

    case SKEY_CANCELGCODE:
      infoSettings.send_cancel_gcode = (infoSettings.send_cancel_gcode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.send_cancel_gcode];
      break;

    case SKEY_PERSISTENTINFO:
      infoSettings.persistent_info = (infoSettings.persistent_info + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.persistent_info];
      break;

    case SKEY_FILELIST:
      infoSettings.file_listmode = (infoSettings.file_listmode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.file_listmode];
      break;

    case SKEY_ACK_NOTIFICATION:
      infoSettings.ack_notification = (infoSettings.ack_notification + 1) % NOTIFICATION_TYPE_NUM;
      setDynamicTextValue(key_val, (char*)notificationType[infoSettings.ack_notification]);
      break;

    #ifdef LED_COLOR_PIN
      case SKEY_KNOB:
        infoSettings.knob_led_color = (infoSettings.knob_led_color + 1 ) % LED_COLOR_NUM;
        settingPage[item_index].valueLabel = itemLedcolor[infoSettings.knob_led_color];
        WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
        break;

    #ifdef LCD_LED_PWM_CHANNEL
      case SKEY_KNOB_LED_IDLE:
        infoSettings.knob_led_idle = (infoSettings.knob_led_idle + 1) % TOGGLE_NUM;
        settingPage[item_index].icon = toggleitem[infoSettings.knob_led_idle];
        break;
    #endif //LCD_LED_PWM_CHANNEL
    #endif

    case SKEY_RESET_SETTINGS:
      showDialog(DIALOG_TYPE_ALERT, textSelect(LABEL_SETTING_RESET), textSelect(LABEL_RESET_SETTINGS_INFO),
                  textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL) ,resetSettings,NULL,NULL);
      break;

    #ifdef LCD_LED_PWM_CHANNEL
      case SKEY_LCD_BRIGHTNESS:
      {
        infoSettings.lcd_brightness = (infoSettings.lcd_brightness + 1) % ITEM_BRIGHTNESS_NUM;
        if(infoSettings.lcd_brightness == 0)
          infoSettings.lcd_brightness = 1; //In Normal it should not be off. Set back to 5%

        char tempstr[8];
        sprintf(tempstr,(char *)textSelect(LABEL_PERCENT_VALUE),LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        setDynamicTextValue(key_val,tempstr);
        Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        break;
      }

      case SKEY_LCD_BRIGTHNESS_DIM:
      {
        infoSettings.lcd_idle_brightness = (infoSettings.lcd_idle_brightness + 1) % ITEM_BRIGHTNESS_NUM;
        char tempstr[8];
        sprintf(tempstr,(char *)textSelect(LABEL_PERCENT_VALUE),LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
        setDynamicTextValue(key_val,tempstr);
        break;
      }

      case SKEY_LCD_DIM_IDLE_TIMER:
        infoSettings.lcd_idle_timer = (infoSettings.lcd_idle_timer + 1) % ITEM_SECONDS_NUM;
        settingPage[item_index].valueLabel = itemDimTime[infoSettings.lcd_idle_timer];
        break;

    #endif //LCD_LED_PWM_CHANNEL

    #ifdef ST7920_SPI
    case SKEY_ST7920_FULLSCREEN:
      infoSettings.marlin_mode_fullscreen = (infoSettings.marlin_mode_fullscreen + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.marlin_mode_fullscreen];
      break;
    #endif

    case SKEY_PLR_EN:
      infoSettings.powerloss_en = (infoSettings.powerloss_en + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.powerloss_en];
      break;

    default:
      return;
  }
   featureSettingsItems.items[key_val] = settingPage[item_index];
} //updateFeatureSettings

//
//load values on page change and reload
//
void loadFeatureSettings(){
  for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++)
  {
    uint8_t item_index = fe_cur_page * LISTITEM_PER_PAGE + i;
    if (item_index < SKEY_COUNT)
    {
      switch (item_index)
      {
      case SKEY_ACK:
        settingPage[item_index].icon = toggleitem[infoSettings.terminalACK];
        break;

      case SKEY_INVERT_X:
        settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[X_AXIS]];
        break;

      case SKEY_INVERT_Y:
        settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[Y_AXIS]];
        break;

      case SKEY_INVERT_Z:
        settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[Z_AXIS]];
        break;

      #ifdef PS_ON_PIN
        case SKEY_POWER:
          settingPage[item_index].icon = toggleitem[infoSettings.auto_off];
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN
        case SKEY_RUNOUT:
          settingPage[item_index].valueLabel = itemRunout[infoSettings.runout];
          break;
      #endif

      case SKEY_SPEED:
        settingPage[item_index].valueLabel = itemMoveSpeed[infoSettings.move_speed];
        break;

      case SKEY_STARTGCODE:
        settingPage[item_index].icon = toggleitem[infoSettings.send_start_gcode];
        break;

      case SKEY_ENDGCODE:
        settingPage[item_index].icon = toggleitem[infoSettings.send_end_gcode];
        break;

      case SKEY_CANCELGCODE:
        settingPage[item_index].icon = toggleitem[infoSettings.send_cancel_gcode];
        break;

      case SKEY_PERSISTENTINFO:
        settingPage[item_index].icon = toggleitem[infoSettings.persistent_info];
        break;

      case SKEY_FILELIST:
        settingPage[item_index].icon = toggleitem[infoSettings.file_listmode];
        break;

      case SKEY_ACK_NOTIFICATION:
        setDynamicTextValue(i, (char*)notificationType[infoSettings.ack_notification]);
        break;

      case SKEY_RESET_SETTINGS:
        break;
      #ifdef LED_COLOR_PIN
        case SKEY_KNOB:
          settingPage[item_index].valueLabel = itemLedcolor[infoSettings.knob_led_color];
          featureSettingsItems.items[i] = settingPage[item_index];
          break;
        #ifdef LCD_LED_PWM_CHANNEL
        case SKEY_KNOB_LED_IDLE:
          settingPage[item_index].icon = toggleitem[infoSettings.knob_led_idle];
          break;
        #endif
      #endif
      #ifdef LCD_LED_PWM_CHANNEL
        case SKEY_LCD_BRIGHTNESS:
        {
          char tempstr[8];
          sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
          setDynamicTextValue(i, tempstr);
          break;
        }
        case SKEY_LCD_BRIGTHNESS_DIM:
        {
          char tempstr[8];
          sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
          setDynamicTextValue(i, tempstr);
          break;
        }
        case SKEY_LCD_DIM_IDLE_TIMER:
          settingPage[item_index].valueLabel = itemDimTime[infoSettings.lcd_idle_timer];
          break;
      #endif //PS_ON_PIN

      #ifdef ST7920_SPI
        case SKEY_ST7920_FULLSCREEN:
          settingPage[item_index].icon = toggleitem[infoSettings.marlin_mode_fullscreen];
          break;
      #endif

      case SKEY_PLR_EN:
        settingPage[item_index].icon = toggleitem[infoSettings.powerloss_en];
        break;

      default:
        break;
      }
      featureSettingsItems.items[i] = settingPage[item_index];
    }
    else
    {
      featureSettingsItems.items[i].icon = ICONCHAR_BACKGROUND;
    }
  }
  // set page up down button according to page count and current page
  if (SKEY_COUNT <= LISTITEM_PER_PAGE)
  {
    featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
    featureSettingsItems.items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if(fe_cur_page == 0){
      featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
      featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if(fe_cur_page == (FE_PAGE_COUNT-1)){
      featureSettingsItems.items[5].icon = ICONCHAR_PAGEUP;
      featureSettingsItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      featureSettingsItems.items[5].icon = ICONCHAR_PAGEUP;
      featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
  //menuDrawListItem(&featureSettingsItems.items[5],5);
  //menuDrawListItem(&featureSettingsItems.items[6],6);

} //loadFeatureSettings


void menuFeatureSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  loadFeatureSettings();
  menuDrawListPage(&featureSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuFeatureSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_5:
      if(FE_PAGE_COUNT > 1){
        if (fe_cur_page > 0){
          fe_cur_page--;
          loadFeatureSettings();
          menuRefreshListPage();
        }
      }
      break;

    case KEY_ICON_6:
      if(FE_PAGE_COUNT > 1){
        if (fe_cur_page < FE_PAGE_COUNT - 1){
          fe_cur_page++;
          loadFeatureSettings();
          menuRefreshListPage();
        }
      }
      break;

    case KEY_ICON_7:
      fe_cur_page = 0;
      infoMenu.cur--;
      break;
    default:
      if(key_num < LISTITEM_PER_PAGE){
      updateFeatureSettings(key_num);
      menuDrawListItem(&featureSettingsItems.items[key_num],key_num);
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
