//TG MODIFIED BY T.GIOIOSA
#include "FeatureSettings.h"
#include "includes.h"

LISTITEMS featureSettingsItems = {
  // title
  LABEL_FEATURE_SETTINGS,
  // icon                   ItemType      Item Title          item value text(only for custom value)
  {
    {ICONCHAR_BACKGROUND,   LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,   LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,   LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,   LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,   LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_PAGEUP,       LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_PAGEDOWN,     LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACK,         LIST_LABEL,   LABEL_BACKGROUND,   LABEL_BACKGROUND},
  }
};

//
//parameter values
//
#define TOGGLE_NUM   2      //TG ON/OFF button
const uint16_t toggleitem[TOGGLE_NUM] = {ICONCHAR_TOGGLE_OFF, ICONCHAR_TOGGLE_ON};

#define ONOFF_NUM   2      //TG 2/5/21 new for ON/OFF texts
const LABEL OnOffitem[ONOFF_NUM] = {LABEL_OFF, LABEL_ON};

#define PCT_RPM_NUM   2      //TG 2/5/21 new for RPM/PCT texts
const LABEL PctRpmitem[PCT_RPM_NUM] = {LABEL_RPM , LABEL_PCT};

#define CUTTER_PWR_UNIT_NUM  CUTTER_PWR_SIZE      //TG 2/5/21 new for MRPM, MPWM, MPCT texts
const LABEL cutterPwrUnititem[CUTTER_PWR_UNIT_NUM] = {LABEL_RPM, LABEL_PCT, LABEL_PWM};

#define SPINDLE_SPIN_NUM 2  //TG CW/CCW for spindle rotation
const LABEL itemSpindleSpin[SPINDLE_SPIN_NUM] = {
                                              //item value text(only for custom value)
                                              LABEL_CCW,
                                              LABEL_CW
                                            };

#define ITEM_TOGGLE_AUTO_NUM 3
const LABEL itemToggleAuto[ITEM_TOGGLE_AUTO_NUM] =
{
  LABEL_OFF,
  LABEL_ON,
  LABEL_AUTO
};

#define ITEM_TOGGLE_SMART_NUM 3
const LABEL itemToggleSmart[ITEM_TOGGLE_SMART_NUM] =
{
  LABEL_OFF,
  LABEL_ON,
  LABEL_SMART
};

#define ITEM_NOTIFICATION_TYPE_NUM 3
const char *const itemNotificationType[ITEM_NOTIFICATION_TYPE_NUM] =
{
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
  SKEY_TERMINAL_ACK = 0,
  SKEY_PERSISTENT_INFO,
  SKEY_FILE_LIST_MODE,
  SKEY_SPIN,              //TG 1/12/20 new
  SKEY_SPINDLE_RMAX,      //TG 2/5/21 new
  SKEY_SPINDLE_PMAX,      //TG 2/5/21 new
  SKEY_LCD_DISP_UNIT,     //TG 2/5/21 new
  SKEY_CUTTER_POWER_UNIT, //TG 2/14/21 new
  SKEY_SPINDLE_USE_PID,   //TG 9/27/21 new
  SKEY_LASER,             //TG 1/12/20 new
  SKEY_INVERT_X,
  SKEY_INVERT_Y,
  SKEY_INVERT_Z,
  SKEY_ACK_NOTIFICATION,
  SKEY_EMULATE_M600,
  SKEY_SERIAL_ALWAYS_ON,
  SKEY_SPEED,
  SKEY_AUTO_LOAD_LEVELING,
  SKEY_FAN_SPEED_PERCENT,
  SKEY_XY_OFFSET_PROBING,
  SKEY_Z_STEPPERS_ALIGNMENT,

  #ifdef PS_ON_PIN
    SKEY_PS_ON,
  #endif

  #ifdef FIL_RUNOUT_PIN
    SKEY_FIL_RUNOUT,
  #endif

  SKEY_PL_RECOVERY_EN,
  SKEY_PL_RECOVERY_HOME,
  SKEY_BTT_MINI_UPS,

  #ifdef LED_COLOR_PIN
    SKEY_KNOB_LED_COLOR,

    #ifdef LCD_LED_PWM_CHANNEL
      SKEY_KNOB_LED_IDLE,
    #endif
  #endif

  SKEY_START_GCODE_ENABLED,
  SKEY_END_GCODE_ENABLED,
  SKEY_CANCEL_GCODE_ENABLED,
  #ifdef LCD_LED_PWM_CHANNEL
    SKEY_LCD_BRIGHTNESS,
    SKEY_LCD_BRIGTHNESS_DIM,
    SKEY_LCD_DIM_IDLE_TIMER,
  #endif
  #ifdef ST7920_SPI
    SKEY_ST7920_FULLSCREEN,
  #endif
  SKEY_RESET_SETTINGS,        // Keep reset always at the bottom of the settings menu list.
  SKEY_COUNT                  // keep this always at the end
} SKEY_LIST;

#define FE_PAGE_COUNT  (SKEY_COUNT+LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE
int fe_cur_page = 0;

//
//set item types
//
LISTITEM settingPage[SKEY_COUNT] = {
  // .icon                 .itemType          .titlelabel                      .valueLabel
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_TERMINAL_ACK,           LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PERSISTENT_INFO,        LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FILE_LIST_MODE,         LABEL_BACKGROUND},
  {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_SPINDLE_ROTATION,         LABEL_CW},          //TG 1/12/20 new
  {ICONCHAR_SETTING1,    LIST_CUSTOMVALUE,   LABEL_SPINDLE_RMAX ,            LABEL_DYNAMIC},     //TG 1/12/20 new
  {ICONCHAR_SETTING1,    LIST_CUSTOMVALUE,   LABEL_SPINDLE_PMAX ,            LABEL_DYNAMIC},     //TG 1/12/20 new
  {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_LCD_POWER_UNIT,           LABEL_RPM},         //TG 1/12/20 new
  {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_CUTTER_POWER_UNIT,        LABEL_RPM},         //TG 1/14/20 new
  {ICONCHAR_BLANK,       LIST_TOGGLE,        LABEL_SPINDLE_USE_PID,          LABEL_OFF},         //TG 9/27/21 new
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_LASER_MODE,               LABEL_BACKGROUND},  //TG 1/12/20 new
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_INVERT_XAXIS,             LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_INVERT_YAXIS,             LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_INVERT_ZAXIS,             LABEL_BACKGROUND},
  {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_ACK_NOTIFICATION,       LABEL_DYNAMIC},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EMULATE_M600,           LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_SERIAL_ALWAYS_ON,       LABEL_BACKGROUND},
  {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_MOVE_SPEED,             LABEL_NORMAL},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_AUTO_LOAD_LEVELING,     LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FAN_SPEED_PERCENT,      LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_XY_OFFSET_PROBING,      LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_Z_STEPPERS_ALIGNMENT,   LABEL_BACKGROUND},

  #ifdef PS_ON_PIN
    {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_PS_ON,                  LABEL_OFF},
  #endif

  #ifdef FIL_RUNOUT_PIN
    {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_FIL_RUNOUT,             LABEL_OFF},
  #endif

  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY_EN,         LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY_HOME,       LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_BTT_MINI_UPS,           LABEL_BACKGROUND},

  #ifdef LED_COLOR_PIN
    {ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_KNOB_LED_COLOR,         LABEL_OFF},

    #ifdef LCD_LED_PWM_CHANNEL
      {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_KNOB_LED_IDLE,          LABEL_BACKGROUND},
    #endif
  #endif

  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_START_GCODE_ENABLED,    LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_END_GCODE_ENABLED,      LABEL_BACKGROUND},
  {ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_CANCEL_GCODE_ENABLED,   LABEL_BACKGROUND},
  #ifdef LCD_LED_PWM_CHANNEL
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_BRIGHTNESS,           LABEL_DYNAMIC},
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_IDLE_BRIGHTNESS,      LABEL_DYNAMIC},
    {ICONCHAR_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_IDLE_DELAY,           LABEL_DYNAMIC},
  #endif
  #ifdef ST7920_SPI
    {ICONCHAR_BLANK,      LIST_TOGGLE,        LABEL_MARLIN_FULLSCREEN,        LABEL_OFF},  //TG fix for V27
  #endif
  // Keep reset settings always at the bottom of the settings menu list.
  {ICONCHAR_BLANK,       LIST_MOREBUTTON,    LABEL_SETTINGS_RESET,         LABEL_BACKGROUND}
};

void resetSettings(void)
{
  infoSettingsReset();
  storePara();
  popupReminder(DIALOG_TYPE_SUCCESS, LABEL_INFO, LABEL_SETTINGS_RESET_DONE);
}

//
//perform action on button press
//
void updateFeatureSettings(uint8_t key_val)
{
  uint8_t item_index = fe_cur_page * LISTITEM_PER_PAGE + key_val;

  switch (item_index)
  {
    case SKEY_TERMINAL_ACK:
      infoSettings.terminalACK = (infoSettings.terminalACK + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.terminalACK];
      break;

    case SKEY_PERSISTENT_INFO:
      infoSettings.persistent_info = (infoSettings.persistent_info + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.persistent_info];
      break;

    case SKEY_FILE_LIST_MODE:
      infoSettings.file_listmode = (infoSettings.file_listmode + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.file_listmode];
      break;

    case SKEY_LCD_DISP_UNIT:   //TG 2/5/21 new
      infoSettings.cutter_disp_unit = (infoSettings.cutter_disp_unit + 1) % CUTTER_PWR_UNIT_NUM;
      settingPage[item_index].valueLabel = cutterPwrUnititem[infoSettings.cutter_disp_unit];
      break;
    
    case SKEY_CUTTER_POWER_UNIT:
      infoSettings.cutter_power_unit = (infoSettings.cutter_power_unit + 1) % CUTTER_PWR_UNIT_NUM;
      settingPage[item_index].valueLabel = cutterPwrUnititem[infoSettings.cutter_power_unit];

      break;
    case SKEY_SPINDLE_RMAX:      //TG 2/5/21 new
     {
      //char tempstr[8];
      uint32_t val = infoSettings.spindle_rpm_max[0];    //getParameter(0, key_val);
      val = numPadInt(NULL, val, val, false);
      val = NOBEYOND(0,val,default_max_spindleRPM[0]);
      //sprintf(tempstr,(char *)textSelect(LABEL_SPINDLE_RMAX ),val);   //TG 2/13/21 needed for Marlin mode?
      infoSettings.spindle_rpm_max[0] = val;
      setDynamicIntValue(key_val, val);
      menuDrawListPage(&featureSettingsItems);
      break;
     }

    case SKEY_SPINDLE_PMAX:      //TG 2/5/21 new
     {
      //char tempstr[8];
      uint16_t val = infoSettings.spindle_pwm_max[0];    //getParameter(0, key_val);
      val = numPadInt(NULL, val, val, false);
      val = NOBEYOND(0,val,default_max_spindlePWM[0]);
      //sprintf(tempstr,(char *)textSelect(LABEL_SPINDLE_PMAX ),val);   //TG 2/13/21 needed for Marlin mode?
      infoSettings.spindle_pwm_max[0] = val;
      setDynamicIntValue(key_val, val);
      menuDrawListPage(&featureSettingsItems);
      break;
     }

    case SKEY_SPINDLE_USE_PID:    //TG 1/12/20 new
      infoSettings.spindle_use_pid = (infoSettings.spindle_use_pid + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.spindle_use_pid];
      storeCmd("%s S%d \n", "M7979", infoSettings.spindle_use_pid);
     break;

    case SKEY_LASER:    //TG 1/12/20 new
      infoSettings.laser_mode = (infoSettings.laser_mode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon = toggleitem[infoSettings.laser_mode];
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

    case SKEY_ACK_NOTIFICATION:
      infoSettings.ack_notification = (infoSettings.ack_notification + 1) % ITEM_NOTIFICATION_TYPE_NUM;
      setDynamicTextValue(key_val, (char *)itemNotificationType[infoSettings.ack_notification]);
      break;

    case SKEY_EMULATE_M600:
      infoSettings.emulate_m600 = (infoSettings.emulate_m600 + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.emulate_m600];
      break;

    case SKEY_SERIAL_ALWAYS_ON:
      infoSettings.serial_alwaysOn = (infoSettings.serial_alwaysOn + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.serial_alwaysOn];
      break;

    case SKEY_SPEED:
      infoSettings.move_speed = (infoSettings.move_speed + 1) % ITEM_SPEED_NUM;
      settingPage[item_index].valueLabel = itemSpeed[infoSettings.move_speed].label;
      break;

    case SKEY_AUTO_LOAD_LEVELING:
      infoSettings.auto_load_leveling = (infoSettings.auto_load_leveling + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.auto_load_leveling];
      break;

    case SKEY_FAN_SPEED_PERCENT:
      infoSettings.fan_percentage = (infoSettings.fan_percentage + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.fan_percentage];
      break;

    case SKEY_XY_OFFSET_PROBING:
      infoSettings.xy_offset_probing = (infoSettings.xy_offset_probing + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.xy_offset_probing];
      break;

    case SKEY_Z_STEPPERS_ALIGNMENT:
      infoSettings.z_steppers_alignment = (infoSettings.z_steppers_alignment + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.z_steppers_alignment];
      break;

    #ifdef PS_ON_PIN
      case SKEY_PS_ON:
        infoSettings.auto_off = (infoSettings.auto_off + 1) % ITEM_TOGGLE_AUTO_NUM;
        settingPage[item_index].valueLabel = itemToggleAuto[infoSettings.auto_off];
        break;
    #endif

    #ifdef FIL_RUNOUT_PIN
      case SKEY_FIL_RUNOUT:
        infoSettings.runout = (infoSettings.runout + 1) % ITEM_TOGGLE_SMART_NUM;
        settingPage[item_index].valueLabel = itemToggleSmart[infoSettings.runout];
        break;
    #endif

    case SKEY_PL_RECOVERY_EN:
      infoSettings.powerloss_en = (infoSettings.powerloss_en + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.powerloss_en];
      break;

    case SKEY_PL_RECOVERY_HOME:
      infoSettings.powerloss_home = (infoSettings.powerloss_home + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.powerloss_home];
      break;

    case SKEY_BTT_MINI_UPS:
      infoSettings.btt_ups = (infoSettings.btt_ups + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.btt_ups];
      break;

    #ifdef LED_COLOR_PIN
      case SKEY_KNOB_LED_COLOR:
        infoSettings.knob_led_color = (infoSettings.knob_led_color + 1 ) % LED_COLOR_NUM;
        settingPage[item_index].valueLabel = itemLedcolor[infoSettings.knob_led_color];
        WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
        break;

      #ifdef LCD_LED_PWM_CHANNEL
        case SKEY_KNOB_LED_IDLE:
          infoSettings.knob_led_idle = (infoSettings.knob_led_idle + 1) % ITEM_TOGGLE_NUM;
          settingPage[item_index].icon = iconToggle[infoSettings.knob_led_idle];
          break;
      #endif  //LCD_LED_PWM_CHANNEL
    #endif

    case SKEY_START_GCODE_ENABLED:
      infoSettings.send_start_gcode = (infoSettings.send_start_gcode + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.send_start_gcode];
      break;

    case SKEY_END_GCODE_ENABLED:
      infoSettings.send_end_gcode = (infoSettings.send_end_gcode + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.send_end_gcode];
      break;

    case SKEY_CANCEL_GCODE_ENABLED:
      infoSettings.send_cancel_gcode = (infoSettings.send_cancel_gcode + 1) % ITEM_TOGGLE_NUM;
      settingPage[item_index].icon = iconToggle[infoSettings.send_cancel_gcode];
      break;
    
	#ifdef LCD_LED_PWM_CHANNEL
      case SKEY_LCD_BRIGHTNESS:
      {
        infoSettings.lcd_brightness = (infoSettings.lcd_brightness + 1) % ITEM_BRIGHTNESS_NUM;
        if(infoSettings.lcd_brightness == 0)
          infoSettings.lcd_brightness = 1; //In Normal it should not be off. Set back to 5%

        char tempstr[8];
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        setDynamicTextValue(key_val, tempstr);
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

    case SKEY_RESET_SETTINGS:
      setDialogText(LABEL_SETTINGS_RESET, LABEL_SETTINGS_RESET_INFO, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_ALERT, resetSettings, NULL, NULL);
      break;

    default:
      return;
  }

  featureSettingsItems.items[key_val] = settingPage[item_index];
}  //updateFeatureSettings

//
//load values on page change and reload
//
void loadFeatureSettings()
{
  for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++)
  {
    uint8_t item_index = fe_cur_page * LISTITEM_PER_PAGE + i;

    if (item_index < SKEY_COUNT)
    {
      switch (item_index)
      {
        case SKEY_TERMINAL_ACK:
          settingPage[item_index].icon = iconToggle[infoSettings.terminalACK];
          break;

        case SKEY_PERSISTENT_INFO:
          settingPage[item_index].icon = iconToggle[infoSettings.persistent_info];
          break;

        case SKEY_FILE_LIST_MODE:
          settingPage[item_index].icon = iconToggle[infoSettings.file_listmode];
          break;

        case SKEY_SPIN:            //TG 1/12/20 new
          //settingPage[item_index].icon = toggleitem[infoSettings.spin_dir];
          settingPage[item_index].valueLabel = itemSpindleSpin[infoSettings.spin_dir];
        break;

        case SKEY_SPINDLE_RMAX:      //TG 2/5/21 new
          setDynamicIntValue(i,infoSettings.spindle_rpm_max[0]);
          break;

        case SKEY_SPINDLE_PMAX:      //TG 2/5/21 new
          setDynamicIntValue(i,infoSettings.spindle_pwm_max[0]);
          break;

        case SKEY_LCD_DISP_UNIT:   //TG 2/5/21 new
          settingPage[item_index].valueLabel = cutterPwrUnititem[infoSettings.cutter_disp_unit];
          break;

        case SKEY_CUTTER_POWER_UNIT:
          settingPage[item_index].valueLabel = cutterPwrUnititem[infoSettings.cutter_power_unit];
          break;
          
        case SKEY_SPINDLE_USE_PID:    //TG 9/27/21 new
          settingPage[item_index].icon = toggleitem[infoSettings.spindle_use_pid];
          storeCmd("%s S%d \n", "M7979", infoSettings.spindle_use_pid);
          break;

        case SKEY_LASER:          //TG 1/12/20 new
          settingPage[item_index].icon = toggleitem[infoSettings.laser_mode];
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

        case SKEY_ACK_NOTIFICATION:
          setDynamicTextValue(i, (char *)itemNotificationType[infoSettings.ack_notification]);
          break;

        case SKEY_EMULATE_M600:
          settingPage[item_index].icon = iconToggle[infoSettings.emulate_m600];
          break;

        case SKEY_SERIAL_ALWAYS_ON:
          settingPage[item_index].icon = iconToggle[infoSettings.serial_alwaysOn];
          break;

        case SKEY_SPEED:
          settingPage[item_index].valueLabel = itemSpeed[infoSettings.move_speed].label;
          break;

        case SKEY_AUTO_LOAD_LEVELING:
          settingPage[item_index].icon = iconToggle[infoSettings.auto_load_leveling];
          break;

        case SKEY_FAN_SPEED_PERCENT:
          settingPage[item_index].icon = iconToggle[infoSettings.fan_percentage];
          break;

        case SKEY_XY_OFFSET_PROBING:
          settingPage[item_index].icon = iconToggle[infoSettings.xy_offset_probing];
          break;

        case SKEY_Z_STEPPERS_ALIGNMENT:
          settingPage[item_index].icon = iconToggle[infoSettings.z_steppers_alignment];
          break;

        #ifdef PS_ON_PIN
          case SKEY_PS_ON:
            settingPage[item_index].valueLabel = itemToggleAuto[infoSettings.auto_off];
            break;
        #endif

        #ifdef FIL_RUNOUT_PIN
          case SKEY_FIL_RUNOUT:
            settingPage[item_index].valueLabel = itemToggleSmart[infoSettings.runout];
            break;
        #endif

        case SKEY_PL_RECOVERY_EN:
          settingPage[item_index].icon = iconToggle[infoSettings.powerloss_en];
          break;

        case SKEY_PL_RECOVERY_HOME:
          settingPage[item_index].icon = iconToggle[infoSettings.powerloss_home];
          break;

        case SKEY_BTT_MINI_UPS:
          settingPage[item_index].icon = iconToggle[infoSettings.btt_ups];
          break;

        #ifdef LED_COLOR_PIN
          case SKEY_KNOB_LED_COLOR:
            settingPage[item_index].valueLabel = itemLedcolor[infoSettings.knob_led_color];
            featureSettingsItems.items[i] = settingPage[item_index];
            break;

          #ifdef LCD_LED_PWM_CHANNEL
            case SKEY_KNOB_LED_IDLE:
              settingPage[item_index].icon = iconToggle[infoSettings.knob_led_idle];
              break;
          #endif
        #endif

        case SKEY_START_GCODE_ENABLED:
          settingPage[item_index].icon = iconToggle[infoSettings.send_start_gcode];
          break;

        case SKEY_END_GCODE_ENABLED:
          settingPage[item_index].icon = iconToggle[infoSettings.send_end_gcode];
          break;

        case SKEY_CANCEL_GCODE_ENABLED:
          settingPage[item_index].icon = iconToggle[infoSettings.send_cancel_gcode];
          break;

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

        case SKEY_RESET_SETTINGS:
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
    if (fe_cur_page == 0)
    {
      featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
      featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if (fe_cur_page == (FE_PAGE_COUNT - 1))
    {
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

}  //loadFeatureSettings

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
        if (FE_PAGE_COUNT > 1 && fe_cur_page > 0)
        {
          fe_cur_page--;
          loadFeatureSettings();
          menuRefreshListPage();
        }
        break;

      case KEY_ICON_6:
        if (FE_PAGE_COUNT > 1 && fe_cur_page < FE_PAGE_COUNT - 1)
        {
          fe_cur_page++;
          loadFeatureSettings();
          menuRefreshListPage();
        }
        break;

      case KEY_ICON_7:
        fe_cur_page = 0;
        infoMenu.cur--;
        break;

      default:
        if (key_num < LISTITEM_PER_PAGE)
        {
          updateFeatureSettings(key_num);
          menuDrawListItem(&featureSettingsItems.items[key_num], key_num);
        }
        break;
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))  //TG compare, if no settings changed then returns zero
  {
    storePara();
  }
}
