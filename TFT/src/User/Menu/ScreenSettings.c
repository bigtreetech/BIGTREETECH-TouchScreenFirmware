#include "ScreenSettings.h"
#include "includes.h"
#include "Colors.h"

enum
{
  KEY_INDEX_OFFSET = 2,
  #ifdef LCD_LED_PWM_CHANNEL
    KEY_INDEX_BRIGHTNESS,
  #endif
  #ifdef BUZZER_PIN
    KEY_INDEX_BUZZER,
  #endif
  #ifdef HAS_EMULATOR
    KEY_INDEX_EMULATOR,
  #endif
};

#define ITEM_MARLIN_TYPE_NUM 2
const char *const labelMarlinType[ITEM_MARLIN_TYPE_NUM] =
{
  //item value text(only for custom value)
  "128x64",
  "20x4"
};

void menuLanguage(void)
{
  #define LANGUAGE_PAGE_COUNT  (LANGUAGE_NUM + LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE
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

#ifdef HAS_EMULATOR

void menuEmulatorBGColor(void)
{
  LABEL title = {LABEL_BG_COLOR};
  LISTITEM totalItems[LCD_COLOR_COUNT];
  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t curItem = 0;
  uint16_t curPage = curItem / LISTITEM_PER_PAGE;

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

void menuEmulatorFontColor(void)
{
  LABEL title = {LABEL_FONT_COLOR};
  LISTITEM totalItems[LCD_COLOR_COUNT];
  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t curItem = 0;
  uint16_t curPage = curItem / LISTITEM_PER_PAGE;

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

  marlinModeitems[2].icon = (infoSettings.marlin_mode_fullscreen == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
  marlinModeitems[3].icon = (infoSettings.marlin_mode_showtitle == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;

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
        marlinModeitems[curIndex].icon = (infoSettings.marlin_mode_fullscreen == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
        listViewRefreshItem(curIndex);
        break;

      case 3:
        infoSettings.marlin_mode_showtitle = (infoSettings.marlin_mode_showtitle + 1) % 2;
        marlinModeitems[curIndex].icon = (infoSettings.marlin_mode_showtitle == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
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

#endif // ST7920_EMULATOR

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

  sounditems[0].icon = (infoSettings.touchSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
  sounditems[1].icon = (infoSettings.toastSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
  sounditems[2].icon = (infoSettings.alertSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
  sounditems[3].icon = (infoSettings.heaterSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;

  listViewCreate(title, sounditems, COUNT(sounditems), NULL, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuSoundSettings)
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case 0:
        infoSettings.touchSound = (infoSettings.touchSound + 1) % 2;
        sounditems[curIndex].icon = (infoSettings.touchSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
        listViewRefreshItem(curIndex);
        break;

      case 1:
        infoSettings.toastSound = (infoSettings.toastSound + 1) % 2;
        sounditems[curIndex].icon = (infoSettings.toastSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
        listViewRefreshItem(curIndex);
        break;

      case 2:
        infoSettings.alertSound = (infoSettings.alertSound + 1) % 2;
        sounditems[curIndex].icon = (infoSettings.alertSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
        listViewRefreshItem(curIndex);
        break;

      case 3:
        infoSettings.heaterSound = (infoSettings.heaterSound + 1) % 2;
        sounditems[curIndex].icon = (infoSettings.heaterSound == 1) ? CHARICON_TOGGLE_ON : CHARICON_TOGGLE_OFF;
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
} //menuSoundSettings

#endif // BUZZER_PIN

#ifdef LCD_LED_PWM_CHANNEL

void menuBrightnessSettings(void)
{
  LABEL title = {LABEL_LCD_BRIGHTNESS};
  LISTITEM brightnessitems[] = {
    // icon                 ItemType          Item Title                 item value text(only for custom value)
    {CHARICON_BLANK,      LIST_CUSTOMVALUE, LABEL_LCD_BRIGHTNESS,      LABEL_DYNAMIC},
    {CHARICON_BLANK,      LIST_CUSTOMVALUE, LABEL_LCD_IDLE_BRIGHTNESS, LABEL_DYNAMIC},
    {CHARICON_BLANK,      LIST_CUSTOMVALUE, LABEL_LCD_IDLE_DELAY,      LABEL_DYNAMIC},
  };

  uint16_t curIndex = KEY_IDLE;
  SETTINGS now = infoSettings;
  char tempstr[8];

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
  setDynamicTextValue(KEY_ICON_0, tempstr);

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
  setDynamicTextValue(KEY_ICON_1, tempstr);

  brightnessitems[2].valueLabel = itemDimTime[infoSettings.lcd_idle_timer];

  listViewCreate(title, brightnessitems, COUNT(brightnessitems), NULL, true, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuBrightnessSettings)
  {
    curIndex = listViewGetSelectedIndex();
    switch (curIndex)
    {
      case 0:
        infoSettings.lcd_brightness = (infoSettings.lcd_brightness + 1) % ITEM_BRIGHTNESS_NUM;

        if (infoSettings.lcd_brightness == 0)
          infoSettings.lcd_brightness = 1;  //In Normal it should not be off. Set back to 5%

        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        setDynamicTextValue(curIndex, tempstr);
        Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        listViewRefreshItem(curIndex);
        break;

      case 1:
        infoSettings.lcd_idle_brightness = (infoSettings.lcd_idle_brightness + 1) % ITEM_BRIGHTNESS_NUM;
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
        setDynamicTextValue(curIndex, tempstr);
        listViewRefreshItem(curIndex);
        break;

      case 2:
        infoSettings.lcd_idle_timer = (infoSettings.lcd_idle_timer + 1) % ITEM_SECONDS_NUM;
        brightnessitems[curIndex].valueLabel = itemDimTime[infoSettings.lcd_idle_timer];
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

#endif //LCD_LED_PWM_CHANNEL

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
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  #ifdef LCD_LED_PWM_CHANNEL
    screenSettingsItems.items[KEY_INDEX_BRIGHTNESS].icon = ICON_BRIGHTNESS;
    screenSettingsItems.items[KEY_INDEX_BRIGHTNESS].label.index = LABEL_LCD_BRIGHTNESS;
  #endif

  //load buzzer icon
  #ifdef BUZZER_PIN
    screenSettingsItems.items[KEY_INDEX_BUZZER].icon = ICON_SOUND;
    screenSettingsItems.items[KEY_INDEX_BUZZER].label.index = LABEL_SOUND;
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
        LCD_RefreshDirection();
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

      #ifdef LCD_LED_PWM_CHANNEL
        case KEY_INDEX_BRIGHTNESS:
          infoMenu.menu[++infoMenu.cur] = menuBrightnessSettings;
          break;
      #endif

      #ifdef BUZZER_PIN
        case KEY_INDEX_BUZZER:
          infoMenu.menu[++infoMenu.cur] = menuSoundSettings;
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
