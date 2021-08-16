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
  SETTINGS now = infoSettings;
  uint8_t curItem = 0;

  // fill items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.marlin_mode_font_color == lcd_colors[i])
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

  while (infoMenu.menu[infoMenu.cur] == menuEmulatorFontColor)
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

        infoSettings.marlin_mode_font_color = lcd_colors[curItem];
      }
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuEmulatorBGColor(void)
{
  LABEL title = {LABEL_BG_COLOR};
  LISTITEM totalItems[LCD_COLOR_COUNT];
  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t curItem = 0;

  // fill items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.marlin_mode_bg_color == lcd_colors[i])
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

  while (infoMenu.menu[infoMenu.cur] == menuEmulatorBGColor)
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

        infoSettings.marlin_mode_bg_color = lcd_colors[curItem];
      }
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuMarlinModeSettings(void)
{
  LABEL title = {LABEL_MARLIN_MODE_SETTINGS};
  LISTITEM marlinModeitems[] = {
  // icon                       ItemType          Item Title               item value text(only for custom value)
    {CHARICON_FONT_COLOR,       LIST_CUSTOMVALUE, LABEL_FONT_COLOR,        LABEL_CUSTOM},
    {CHARICON_BACKGROUND_COLOR, LIST_CUSTOMVALUE, LABEL_BG_COLOR,          LABEL_CUSTOM},
    {CHARICON_TOGGLE_ON,        LIST_TOGGLE,      LABEL_MARLIN_FULLSCREEN, LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,        LIST_TOGGLE,      LABEL_MARLIN_SHOW_TITLE, LABEL_BACKGROUND},
    {CHARICON_BLANK,            LIST_CUSTOMVALUE, LABEL_MARLIN_TYPE,       LABEL_DYNAMIC},
  };

  for (uint8_t i = 0; i < LCD_COLOR_COUNT; i++)
  {
    if (infoSettings.marlin_mode_font_color == lcd_colors[i])
      marlinModeitems[0].valueLabel = lcd_color_names[i];

    if (infoSettings.marlin_mode_bg_color == lcd_colors[i])
      marlinModeitems[1].valueLabel = lcd_color_names[i];
  }

  marlinModeitems[2].icon = iconToggle[infoSettings.marlin_mode_fullscreen];
  marlinModeitems[3].icon = iconToggle[infoSettings.marlin_mode_showtitle];

  setDynamicTextValue(4, (char *)labelMarlinType[infoSettings.marlin_type]);

  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;

  listViewCreate(title, marlinModeitems, COUNT(marlinModeitems), NULL, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuMarlinModeSettings)
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case 0:
        infoMenu.menu[++infoMenu.cur] = menuEmulatorFontColor;
        break;

      case 1:
        infoMenu.menu[++infoMenu.cur] = menuEmulatorBGColor;
        break;

      case 2:
        infoSettings.marlin_mode_fullscreen = (infoSettings.marlin_mode_fullscreen + 1) % 2;
        marlinModeitems[2].icon = iconToggle[infoSettings.marlin_mode_fullscreen];
        listViewRefreshItem(curIndex);
        break;

      case 3:
        infoSettings.marlin_mode_showtitle = (infoSettings.marlin_mode_showtitle + 1) % 2;
        marlinModeitems[3].icon = iconToggle[infoSettings.marlin_mode_showtitle];
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

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

#endif  // ST7920_EMULATOR

void menuLanguage(void)
{
  LABEL title = {LABEL_LANGUAGE};
  LISTITEM totalItems[LANGUAGE_NUM];
  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint16_t curItem = infoSettings.language;
  uint16_t curPage = curItem / LISTITEM_PER_PAGE;

  // fill language items
  uint8_t tmp_language = infoSettings.language;
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (i == tmp_language)
      totalItems[i].icon = CHARICON_CHECKED;
    else
      totalItems[i].icon = CHARICON_UNCHECKED;

    infoSettings.language = i;
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = textSelect(LABEL_LANGUAGE);
  }
  infoSettings.language = tmp_language;

  listViewCreate(title, totalItems, COUNT(totalItems), &curPage, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuLanguage)
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < COUNT(totalItems))
    {
      if (curIndex < (uint16_t)LANGUAGE_NUM && curIndex != curItem)  // has changed
      {
        totalItems[curItem].icon = CHARICON_UNCHECKED;
        listViewRefreshItem(curItem);  // refresh unchecked status
        curItem = curIndex;
        totalItems[curItem].icon = CHARICON_CHECKED;
        listViewRefreshItem(curItem);  // refresh checked status

        infoSettings.language = curItem;
        menuDrawTitle(textSelect(LABEL_LANGUAGE));
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
  // icon                ItemType          Item Title               item value text(only for custom value)
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_ACK_NOTIFICATION,  LABEL_DYNAMIC},
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_FILE_SORT_BY,      LABEL_DYNAMIC},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_FILE_LIST_MODE,    LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_FAN_SPEED_PERCENT, LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_PERSISTENT_INFO,   LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_TERMINAL_ACK,      LABEL_BACKGROUND},

    #ifdef LED_COLOR_PIN
      {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_KNOB_LED_COLOR,    LABEL_OFF},

      #ifdef LCD_LED_PWM_CHANNEL
        {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_KNOB_LED_IDLE,     LABEL_BACKGROUND},
      #endif
    #endif
  };

  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;

  setDynamicTextValue(0, (char *)itemNotificationType[infoSettings.ack_notification]);
  setDynamicTextValue(1, (char *)itemSortBy[infoSettings.files_sort_by]);
  uiItems[2].icon = iconToggle[infoSettings.file_listmode];
  uiItems[3].icon = iconToggle[infoSettings.fan_percentage];
  uiItems[4].icon = iconToggle[infoSettings.persistent_info];
  uiItems[5].icon = iconToggle[infoSettings.terminalACK];

  #ifdef LED_COLOR_PIN
    uiItems[6].valueLabel = led_color_names[infoSettings.knob_led_color];

    #ifdef LCD_LED_PWM_CHANNEL
      uiItems[7].icon = iconToggle[infoSettings.knob_led_idle];
    #endif
  #endif

  listViewCreate(title, uiItems, COUNT(uiItems), NULL, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuUISettings)
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
        infoSettings.file_listmode = (infoSettings.file_listmode + 1) % ITEM_TOGGLE_NUM;
        uiItems[curIndex].icon = iconToggle[infoSettings.file_listmode];
        break;

      case 3:
        infoSettings.fan_percentage = (infoSettings.fan_percentage + 1) % ITEM_TOGGLE_NUM;
        uiItems[curIndex].icon = iconToggle[infoSettings.fan_percentage];
        break;

      case 4:
        infoSettings.persistent_info = (infoSettings.persistent_info + 1) % ITEM_TOGGLE_NUM;
        uiItems[curIndex].icon = iconToggle[infoSettings.persistent_info];
        break;

      case 5:
        infoSettings.terminalACK = (infoSettings.terminalACK + 1) % ITEM_TOGGLE_NUM;
        uiItems[curIndex].icon = iconToggle[infoSettings.terminalACK];
        break;

      #ifdef LED_COLOR_PIN
        case 6:
          infoSettings.knob_led_color = (infoSettings.knob_led_color + 1 ) % LED_COLOR_COUNT;
          uiItems[curIndex].valueLabel = led_color_names[infoSettings.knob_led_color];
          Knob_LED_SetColor(led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
          break;

        #ifdef LCD_LED_PWM_CHANNEL
          case 7:
            infoSettings.knob_led_idle = (infoSettings.knob_led_idle + 1) % ITEM_TOGGLE_NUM;
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

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

#ifdef BUZZER_PIN

void menuSoundSettings(void)
{
  LABEL title = {LABEL_SOUND};
  LISTITEM sounditems[] = {
  // icon                 ItemType     Item Title          item value text(only for custom value)
    {CHARICON_TOGGLE_ON,  LIST_TOGGLE, LABEL_TOUCH_SOUND,  LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,  LIST_TOGGLE, LABEL_TOAST_SOUND,  LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,  LIST_TOGGLE, LABEL_ALERT_SOUND,  LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,  LIST_TOGGLE, LABEL_HEATER_SOUND, LABEL_BACKGROUND},
  };

  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;

  sounditems[0].icon = iconToggle[infoSettings.touchSound];
  sounditems[1].icon = iconToggle[infoSettings.toastSound];
  sounditems[2].icon = iconToggle[infoSettings.alertSound];
  sounditems[3].icon = iconToggle[infoSettings.heaterSound];

  listViewCreate(title, sounditems, COUNT(sounditems), NULL, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuSoundSettings)
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case 0:
        infoSettings.touchSound = (infoSettings.touchSound + 1) % 2;
        sounditems[0].icon = iconToggle[infoSettings.touchSound];
        break;

      case 1:
        infoSettings.toastSound = (infoSettings.toastSound + 1) % 2;
        sounditems[1].icon = iconToggle[infoSettings.toastSound];
        break;

      case 2:
        infoSettings.alertSound = (infoSettings.alertSound + 1) % 2;
        sounditems[2].icon = iconToggle[infoSettings.alertSound];
        break;

      case 3:
        infoSettings.heaterSound = (infoSettings.heaterSound + 1) % 2;
        sounditems[3].icon = iconToggle[infoSettings.heaterSound];
        break;

      default:
        break;
    }

    if (curIndex < sizeof(sounditems))
      listViewRefreshItem(curIndex);

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}  // menuSoundSettings

#endif  // BUZZER_PIN

#ifdef LCD_LED_PWM_CHANNEL

void menuBrightnessSettings(void)
{
  LABEL title = {LABEL_LCD_BRIGHTNESS};
  LISTITEM brightnessitems[] = {
  // icon                ItemType          Item Title                 item value text(only for custom value)
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_LCD_BRIGHTNESS,      LABEL_DYNAMIC},
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_LCD_IDLE_BRIGHTNESS, LABEL_DYNAMIC},
    {CHARICON_BLANK,     LIST_CUSTOMVALUE, LABEL_LCD_IDLE_TIME,       LABEL_DYNAMIC},
    {CHARICON_TOGGLE_ON, LIST_TOGGLE,      LABEL_BLOCK_TOUCH_ON_IDLE, LABEL_BACKGROUND},
  };

  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  char tempstr[8];

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_brightness]);
  setDynamicTextValue(0, tempstr);

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_idle_brightness]);
  setDynamicTextValue(1, tempstr);

  brightnessitems[2].valueLabel = lcd_idle_time_names[infoSettings.lcd_idle_time];
  brightnessitems[3].icon = iconToggle[infoSettings.block_touch_on_idle];

  listViewCreate(title, brightnessitems, COUNT(brightnessitems), NULL, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuBrightnessSettings)
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
        infoSettings.block_touch_on_idle = (infoSettings.block_touch_on_idle + 1) % 2;
        brightnessitems[curIndex].icon = iconToggle[infoSettings.block_touch_on_idle];
        break;

      default:
        break;
    }

    if (curIndex < sizeof(brightnessitems))
      listViewRefreshItem(curIndex);

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

#endif  // LCD_LED_PWM_CHANNEL

void menuScreenSettings(void)
{
  MENUITEMS screenSettingsItems = {
    // title
    LABEL_SCREEN_SETTINGS,
    // icon                          label
    {
      {ICON_ROTATE_UI,               LABEL_ROTATE_UI},
      {ICON_TOUCHSCREEN_ADJUST,      LABEL_TOUCHSCREEN_ADJUST},
      {ICON_LANGUAGE,                LABEL_LANGUAGE},
      {ICON_FEATURE_SETTINGS,        LABEL_UI_SETTINGS},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
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
  SETTINGS now = infoSettings;

  menuDrawPage(&screenSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuScreenSettings)
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        infoSettings.rotate_ui = !infoSettings.rotate_ui;
        LCD_RefreshDirection(infoSettings.rotate_ui);
        TSC_Calibration();
        menuDrawPage(&screenSettingsItems);
        break;

      case KEY_ICON_1:
        TSC_Calibration();
        menuDrawPage(&screenSettingsItems);
        break;

      case KEY_ICON_2:
        if (getFlashSignStatus(lang_sign))
          infoMenu.menu[++infoMenu.cur] = menuLanguage;
        else
          popupReminder(DIALOG_TYPE_ALERT, (uint8_t *)"Language not available",
                        (uint8_t *)"To change Language first flash a Language pack ini file.");
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuUISettings;
        break;

      #ifdef BUZZER_PIN
        case KEY_INDEX_BUZZER:
          infoMenu.menu[++infoMenu.cur] = menuSoundSettings;
          break;
      #endif

      #ifdef LCD_LED_PWM_CHANNEL
        case KEY_INDEX_BRIGHTNESS:
          infoMenu.menu[++infoMenu.cur] = menuBrightnessSettings;
          break;
      #endif

      #ifdef ST7920_EMULATOR
        case KEY_INDEX_EMULATOR:
          infoMenu.menu[++infoMenu.cur] = menuMarlinModeSettings;
          break;
      #endif

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
