#include "ScreenSettings.h"
#include "includes.h"

enum
{
  KEY_INDEX_OFFSET = 3,
  #ifdef BUZZER_PIN
    KEY_INDEX_BUZZER,
  #endif
  #ifdef LCD_LED_PWM_CHANNEL
    KEY_INDEX_BRIGHTNESS,
  #endif
  #ifdef HAS_EMULATOR
    KEY_INDEX_EMULATOR,
  #endif
};

#define ITEM_MARLIN_TYPE_NUM 2
const char *const labelMarlinType[ITEM_MARLIN_TYPE_NUM] =
{
  // item value text(only for custom value)
  "128x64",
  "20x4"
};

#define ITEM_NOTIFICATION_TYPE_NUM 3
const char *const itemNotificationType[ITEM_NOTIFICATION_TYPE_NUM] =
{
  // item value text(only for custom value)
  "OFF",
  "POPUP",
  "TOAST"
};

const char *const itemSortBy[SORT_BY_COUNT] =
{
  // item value text(only for custom value)
  "Date ▼",
  "Date ▲",
  "Name ▲",
  "Name ▼",
};

#ifdef HAS_EMULATOR

void menuEmulatorFontColor(void)
{
  LABEL title = {LABEL_FONT_COLOR};
  LISTITEM totalItems[LCD_COLOR_COUNT];
  uint16_t curIndex = KEY_IDLE;
  uint8_t curItem = 0;

  // fill items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.marlin_font_color == lcd_colors[i])
    {
      totalItems[i].icon = CHARICON_CHECKED;
      curItem = i;
    }
    else
    {
      totalItems[i].icon = CHARICON_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel = lcd_color_names[i];
  }
  uint16_t curPage = curItem / LISTITEM_PER_PAGE;

  listViewCreate(title, totalItems, COUNT(totalItems), &curPage, true, NULL, NULL);

  while (MENU_IS(menuEmulatorFontColor))
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < COUNT(totalItems))
    {
      if (curIndex < (uint16_t)LCD_COLOR_COUNT && curIndex != curItem)  // has changed
      {
        totalItems[curItem].icon = CHARICON_UNCHECKED;
        listViewRefreshItem(curItem);  // refresh unchecked status
        curItem = curIndex;
        totalItems[curItem].icon = CHARICON_CHECKED;
        listViewRefreshItem(curItem);  // refresh checked status

        infoSettings.marlin_font_color = lcd_colors[curItem];
      }
    }

    loopProcess();
  }

  saveSettings();  // save settings
}

void menuEmulatorBGColor(void)
{
  LABEL title = {LABEL_BG_COLOR};
  LISTITEM totalItems[LCD_COLOR_COUNT];
  uint16_t curIndex = KEY_IDLE;
  uint8_t curItem = 0;

  // fill items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.marlin_bg_color == lcd_colors[i])
    {
      totalItems[i].icon = CHARICON_CHECKED;
      curItem = i;
    }
    else
    {
      totalItems[i].icon = CHARICON_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel = lcd_color_names[i];
  }
  uint16_t curPage = curItem / LISTITEM_PER_PAGE;

  listViewCreate(title, totalItems, COUNT(totalItems), &curPage, true, NULL, NULL);

  while (MENU_IS(menuEmulatorBGColor))
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < COUNT(totalItems))
    {
      if (curIndex < (uint16_t)LCD_COLOR_COUNT && curIndex != curItem)  // has changed
      {
        totalItems[curItem].icon = CHARICON_UNCHECKED;
        listViewRefreshItem(curItem);  // refresh unchecked status
        curItem = curIndex;
        totalItems[curItem].icon = CHARICON_CHECKED;
        listViewRefreshItem(curItem);  // refresh checked status

        infoSettings.marlin_bg_color = lcd_colors[curItem];
      }
    }

    loopProcess();
  }

  saveSettings();  // save settings
}

void menuMarlinModeSettings(void)
{
  LABEL title = {LABEL_MARLIN_MODE_SETTINGS};
  LISTITEM marlinModeitems[] = {
  // icon                       item type         item title               item value text(only for custom value)
    {CHARICON_FONT_COLOR,       LIST_CUSTOMVALUE, LABEL_FONT_COLOR,        LABEL_CUSTOM},
    {CHARICON_BACKGROUND_COLOR, LIST_CUSTOMVALUE, LABEL_BG_COLOR,          LABEL_CUSTOM},
    {CHARICON_TOGGLE_ON,        LIST_TOGGLE,      LABEL_MARLIN_FULLSCREEN, LABEL_NULL},
    {CHARICON_TOGGLE_ON,        LIST_TOGGLE,      LABEL_MARLIN_SHOW_TITLE, LABEL_NULL},
    {CHARICON_BLANK,            LIST_CUSTOMVALUE, LABEL_MARLIN_TYPE,       LABEL_DYNAMIC},
  };

  for (uint8_t i = 0; i < LCD_COLOR_COUNT; i++)
  {
    if (infoSettings.marlin_font_color == lcd_colors[i])
      marlinModeitems[0].valueLabel = lcd_color_names[i];

    if (infoSettings.marlin_bg_color == lcd_colors[i])
      marlinModeitems[1].valueLabel = lcd_color_names[i];
  }

  marlinModeitems[2].icon = iconToggle[infoSettings.marlin_fullscreen];
  marlinModeitems[3].icon = iconToggle[infoSettings.marlin_show_title];

  setDynamicTextValue(4, (char *)labelMarlinType[infoSettings.marlin_type]);

  uint16_t curIndex = KEY_IDLE;

  listViewCreate(title, marlinModeitems, COUNT(marlinModeitems), NULL, true, NULL, NULL);

  while (MENU_IS(menuMarlinModeSettings))
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case 0:
        OPEN_MENU(menuEmulatorFontColor);
        break;

      case 1:
        OPEN_MENU(menuEmulatorBGColor);
        break;

      case 2:
        TOGGLE_BIT(infoSettings.marlin_fullscreen, 0);
        marlinModeitems[2].icon = iconToggle[infoSettings.marlin_fullscreen];
        listViewRefreshItem(curIndex);
        break;

      case 3:
        TOGGLE_BIT(infoSettings.marlin_show_title, 0);
        marlinModeitems[3].icon = iconToggle[infoSettings.marlin_show_title];
        listViewRefreshItem(curIndex);
        break;

      case 4:
        infoSettings.marlin_type = (infoSettings.marlin_type + 1) % ITEM_MARLIN_TYPE_NUM;
        setDynamicTextValue(curIndex, (char *)labelMarlinType[infoSettings.marlin_type]);
        listViewRefreshItem(curIndex);
        break;

      default:
        break;
    }

    loopProcess();
  }

  saveSettings();  // save settings
}

#endif  // ST7920_EMULATOR

void menuLanguage(void)
{
  LABEL title = {LABEL_LANGUAGE};
  LISTITEM totalItems[LANGUAGE_NUM];
  uint16_t curIndex = KEY_IDLE;
  uint16_t curItem = infoSettings.language;
  SETTINGS now = infoSettings;

  char *firstLanguage = (char *)default_pack[LABEL_LANGUAGE];  // get first language name directly from memory
  char secondLanguage[MAX_LANG_LABEL_LENGTH];

  W25Qxx_ReadBuffer((uint8_t *)&secondLanguage, getLabelFlashAddr(LABEL_LANGUAGE), MAX_LANG_LABEL_LENGTH);  // read second language name from SPI flash

  // set language name labels
  setDynamicLabel(0, firstLanguage);
  setDynamicLabel(1, secondLanguage);

  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    totalItems[i].icon = CHARICON_UNCHECKED;
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.index = LABEL_DYNAMIC;
  }

  totalItems[curItem].icon = CHARICON_CHECKED;  // check current selected language

  listViewCreate(title, totalItems, COUNT(totalItems), NULL, true, NULL, NULL);

  while (MENU_IS(menuLanguage))
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < COUNT(totalItems))
    {
      if (curIndex < (uint16_t)LANGUAGE_NUM && curIndex != curItem)  // has changed
      {
        totalItems[curItem].icon = CHARICON_UNCHECKED;
        listViewRefreshItem(curItem);  // refresh unchecked status
        curItem = curIndex;  // update selected index
        totalItems[curItem].icon = CHARICON_CHECKED;
        listViewRefreshItem(curItem);  // refresh checked status

        infoSettings.language = curItem;
      }
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    statusScreen_setReady();  // restore msg buffer when language is changed
    storePara();
  }
}

void menuUISettings(void)
{
  LABEL title = {LABEL_UI_SETTINGS};
  LISTITEM uiItems[] = {
  // icon                item type         item title                  item value text(only for custom value)
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_ACK_NOTIFICATION,     LABEL_DYNAMIC},
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_FILES_SORT_BY,        LABEL_DYNAMIC},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_FILES_LIST_MODE,      LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_FILENAME_EXTENSION,   LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_FAN_SPEED_PERCENTAGE, LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_PERSISTENT_INFO,      LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_TERMINAL_ACK,         LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_LED_ALWAYS_ON,        LABEL_NULL},

    #ifdef KNOB_LED_COLOR_PIN
      {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_KNOB_LED_COLOR,       LABEL_OFF},

      #ifdef LCD_LED_PWM_CHANNEL
        {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_KNOB_LED_IDLE,        LABEL_NULL},
      #endif
    #endif
  };

  uint16_t curIndex = KEY_IDLE;

  setDynamicTextValue(0, (char *)itemNotificationType[infoSettings.ack_notification]);
  setDynamicTextValue(1, (char *)itemSortBy[infoSettings.files_sort_by]);
  uiItems[2].icon = iconToggle[infoSettings.files_list_mode];
  uiItems[3].icon = iconToggle[infoSettings.filename_extension];
  uiItems[4].icon = iconToggle[infoSettings.fan_percentage];
  uiItems[5].icon = iconToggle[infoSettings.persistent_info];
  uiItems[6].icon = iconToggle[infoSettings.terminal_ack];
  uiItems[7].icon = iconToggle[infoSettings.led_always_on];

  #ifdef KNOB_LED_COLOR_PIN
    uiItems[8].valueLabel = knob_led_color_names[infoSettings.knob_led_color];

    #ifdef LCD_LED_PWM_CHANNEL
      uiItems[9].icon = iconToggle[infoSettings.knob_led_idle];
    #endif
  #endif

  listViewCreate(title, uiItems, COUNT(uiItems), NULL, true, NULL, NULL);

  while (MENU_IS(menuUISettings))
  {
    curIndex = listViewGetSelectedIndex();
    switch (curIndex)
    {
      case 0:
        infoSettings.ack_notification = (infoSettings.ack_notification + 1) % ITEM_NOTIFICATION_TYPE_NUM;
        setDynamicTextValue(curIndex, (char *)itemNotificationType[infoSettings.ack_notification]);
        break;

      case 1:
        infoSettings.files_sort_by = (infoSettings.files_sort_by + 1) % SORT_BY_COUNT;
        setDynamicTextValue(curIndex, (char *)itemSortBy[infoSettings.files_sort_by]);
        break;

      case 2:
        TOGGLE_BIT(infoSettings.files_list_mode, 0);
        uiItems[curIndex].icon = iconToggle[infoSettings.files_list_mode];
        break;

      case 3:
        TOGGLE_BIT(infoSettings.filename_extension, 0);
        uiItems[curIndex].icon = iconToggle[infoSettings.filename_extension];
        break;

      case 4:
        TOGGLE_BIT(infoSettings.fan_percentage, 0);
        uiItems[curIndex].icon = iconToggle[infoSettings.fan_percentage];
        break;

      case 5:
        TOGGLE_BIT(infoSettings.persistent_info, 0);
        uiItems[curIndex].icon = iconToggle[infoSettings.persistent_info];
        break;

      case 6:
        TOGGLE_BIT(infoSettings.terminal_ack, 0);
        uiItems[curIndex].icon = iconToggle[infoSettings.terminal_ack];
        break;

      case 7:
        TOGGLE_BIT(infoSettings.led_always_on, 0);
        uiItems[curIndex].icon = iconToggle[infoSettings.led_always_on];
        break;

      #ifdef KNOB_LED_COLOR_PIN
        case 8:
          infoSettings.knob_led_color = (infoSettings.knob_led_color + 1 ) % KNOB_LED_COLOR_COUNT;
          uiItems[curIndex].valueLabel = knob_led_color_names[infoSettings.knob_led_color];
          Knob_LED_SetColor(knob_led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
          break;

        #ifdef LCD_LED_PWM_CHANNEL
          case 9:
            TOGGLE_BIT(infoSettings.knob_led_idle, 0);
            uiItems[curIndex].icon = iconToggle[infoSettings.knob_led_idle];
            break;
        #endif  // LCD_LED_PWM_CHANNEL
      #endif

      default:
        break;
    }

    if (curIndex < sizeof(uiItems))
      listViewRefreshItem(curIndex);

    loopProcess();
  }

  saveSettings();  // save settings
}

#ifdef BUZZER_PIN

void menuSoundSettings(void)
{
  LABEL title = {LABEL_SOUND};
  LISTITEM sounditems[] = {
  // icon                item type    item title          item value text(only for custom value)
    {CHARICON_TOGGLE_ON, LIST_TOGGLE, LABEL_TOUCH_SOUND,  LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE, LABEL_TOAST_SOUND,  LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE, LABEL_ALERT_SOUND,  LABEL_NULL},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE, LABEL_HEATER_SOUND, LABEL_NULL},
  };

  uint16_t curIndex = KEY_IDLE;

  for (uint8_t i = 0; i < SOUND_TYPE_COUNT; i++)
  {
    sounditems[i].icon = iconToggle[GET_BIT(infoSettings.sounds, i)];
  }

  listViewCreate(title, sounditems, COUNT(sounditems), NULL, true, NULL, NULL);

  while (MENU_IS(menuSoundSettings))
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < SOUND_TYPE_COUNT)
    {
      TOGGLE_BIT(infoSettings.sounds, curIndex);
      sounditems[curIndex].icon = iconToggle[GET_BIT(infoSettings.sounds, curIndex)];
      listViewRefreshItem(curIndex);
    }

    loopProcess();
  }

  saveSettings();  // save settings
}  // menuSoundSettings

#endif  // BUZZER_PIN

#ifdef LCD_LED_PWM_CHANNEL

void menuBrightnessSettings(void)
{
  LABEL title = {LABEL_LCD_BRIGHTNESS};
  LISTITEM brightnessitems[] = {
  // icon                item type         item title                 item value text(only for custom value)
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_LCD_BRIGHTNESS,      LABEL_DYNAMIC},
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_LCD_IDLE_BRIGHTNESS, LABEL_DYNAMIC},
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_LCD_IDLE_TIME,       LABEL_DYNAMIC},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_LCD_LOCK_ON_IDLE,    LABEL_NULL},
  };

  uint16_t curIndex = KEY_IDLE;
  char tempstr[8];

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_brightness]);
  setDynamicTextValue(0, tempstr);

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_idle_brightness]);
  setDynamicTextValue(1, tempstr);

  brightnessitems[2].valueLabel = lcd_idle_time_names[infoSettings.lcd_idle_time];
  brightnessitems[3].icon = iconToggle[infoSettings.lcd_lock_on_idle];

  listViewCreate(title, brightnessitems, COUNT(brightnessitems), NULL, true, NULL, NULL);

  while (MENU_IS(menuBrightnessSettings))
  {
    curIndex = listViewGetSelectedIndex();
    switch (curIndex)
    {
      case 0:
        infoSettings.lcd_brightness = (infoSettings.lcd_brightness + 1) % LCD_BRIGHTNESS_COUNT;

        if (infoSettings.lcd_brightness == 0)
          infoSettings.lcd_brightness = 1;  // In Normal it should not be off. Set back to 5%

        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_brightness]);
        setDynamicTextValue(curIndex, tempstr);
        LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_brightness]);
        break;

      case 1:
        infoSettings.lcd_idle_brightness = (infoSettings.lcd_idle_brightness + 1) % LCD_BRIGHTNESS_COUNT;
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_idle_brightness]);
        setDynamicTextValue(curIndex, tempstr);
        break;

      case 2:
        infoSettings.lcd_idle_time = (infoSettings.lcd_idle_time + 1) % LCD_IDLE_TIME_COUNT;
        brightnessitems[curIndex].valueLabel = lcd_idle_time_names[infoSettings.lcd_idle_time];
        break;

      case 3:
        infoSettings.lcd_lock_on_idle = (infoSettings.lcd_lock_on_idle + 1) % 2;
        brightnessitems[curIndex].icon = iconToggle[infoSettings.lcd_lock_on_idle];
        break;

      default:
        break;
    }

    if (curIndex < sizeof(brightnessitems))
      listViewRefreshItem(curIndex);

    loopProcess();
  }

  saveSettings();  // save settings
}

#endif  // LCD_LED_PWM_CHANNEL

void menuScreenSettings(void)
{
  MENUITEMS screenSettingsItems = {
    // title
    LABEL_SCREEN_SETTINGS,
    // icon                          label
    {
      {ICON_ROTATE_UI,               LABEL_ROTATED_UI},
      {ICON_TOUCHSCREEN_ADJUST,      LABEL_TOUCHSCREEN_ADJUST},
      {ICON_LANGUAGE,                LABEL_LANGUAGE},
      {ICON_FEATURE_SETTINGS,        LABEL_UI_SETTINGS},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  // load buzzer icon
  #ifdef BUZZER_PIN
    screenSettingsItems.items[KEY_INDEX_BUZZER].icon = ICON_SOUND;
    screenSettingsItems.items[KEY_INDEX_BUZZER].label.index = LABEL_SOUND;
  #endif

  #ifdef LCD_LED_PWM_CHANNEL
    screenSettingsItems.items[KEY_INDEX_BRIGHTNESS].icon = ICON_BRIGHTNESS;
    screenSettingsItems.items[KEY_INDEX_BRIGHTNESS].label.index = LABEL_LCD_BRIGHTNESS;
  #endif

  #ifdef ST7920_EMULATOR
    // LCD12864 background color
    screenSettingsItems.items[KEY_INDEX_EMULATOR].icon = ICON_MARLIN_MODE;
    screenSettingsItems.items[KEY_INDEX_EMULATOR].label.index = LABEL_MARLIN_MODE_SETTINGS;
  #endif

  uint16_t curIndex = KEY_IDLE;

  menuDrawPage(&screenSettingsItems);

  while (MENU_IS(menuScreenSettings))
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        TOGGLE_BIT(infoSettings.rotated_ui, 0);
        LCD_RefreshDirection(infoSettings.rotated_ui);
        TSC_Calibration();
        menuDrawPage(&screenSettingsItems);
        break;

      case KEY_ICON_1:
        TSC_Calibration();
        menuDrawPage(&screenSettingsItems);
        break;

      case KEY_ICON_2:
        if (getFlashSignStatus(lang_sign))
          OPEN_MENU(menuLanguage);
        else
          popupReminder(DIALOG_TYPE_ALERT, (uint8_t *)"Language not available",
                        (uint8_t *)"To change Language first flash a Language pack ini file.");
        break;

      case KEY_ICON_3:
        OPEN_MENU(menuUISettings);
        break;

      #ifdef BUZZER_PIN
        case KEY_INDEX_BUZZER:
          OPEN_MENU(menuSoundSettings);
          break;
      #endif

      #ifdef LCD_LED_PWM_CHANNEL
        case KEY_INDEX_BRIGHTNESS:
          OPEN_MENU(menuBrightnessSettings);
          break;
      #endif

      #ifdef ST7920_EMULATOR
        case KEY_INDEX_EMULATOR:
          OPEN_MENU(menuMarlinModeSettings);
          break;
      #endif

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }

  saveSettings();  // save settings
}
