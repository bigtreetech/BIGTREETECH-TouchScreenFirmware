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
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  // fill language items
  uint8_t tmp_language = infoSettings.language;
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (i == tmp_language)
      totalItems[i].icon = ICONCHAR_CHECKED;
    else
      totalItems[i].icon = ICONCHAR_UNCHECKED;

    infoSettings.language = i;
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = textSelect(LABEL_LANGUAGE);
  }
  infoSettings.language = tmp_language;

  listWidgetCreate(title, totalItems, COUNT(totalItems), infoSettings.language / LISTITEM_PER_PAGE);

  while (infoMenu.menu[infoMenu.cur] == menuLanguage)
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
        if (key_num < LISTITEM_PER_PAGE)
        {
          uint16_t cur_item = infoSettings.language;
          uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
          if (tmp_i < LANGUAGE_NUM && tmp_i != cur_item)  // has changed
          {
            totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
            listWidgetRefreshItem(cur_item);  // refresh unchecked status
            cur_item = tmp_i;
            totalItems[cur_item].icon = ICONCHAR_CHECKED;
            listWidgetRefreshItem(cur_item);  // refresh checked status

            infoSettings.language = cur_item;
            menuDrawTitle(textSelect(LABEL_LANGUAGE));
          }
        }
        break;
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
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t cur_item = 0;

  // fill items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.marlin_mode_bg_color == lcd_colors[i])
    {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    }
    else
    {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel = lcd_color_names[i];
  }

  listWidgetCreate(title, totalItems, COUNT(totalItems), cur_item/ LISTITEM_PER_PAGE);

  while (infoMenu.menu[infoMenu.cur] == menuEmulatorBGColor)
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
        if (key_num < LISTITEM_PER_PAGE)
        {
          uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
          if (tmp_i < LCD_COLOR_COUNT && tmp_i != cur_item)  // has changed
          {
            totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
            listWidgetRefreshItem(cur_item);  // refresh unchecked status
            cur_item = tmp_i;
            totalItems[cur_item].icon = ICONCHAR_CHECKED;
            listWidgetRefreshItem(cur_item);  // refresh checked status

            infoSettings.marlin_mode_bg_color = lcd_colors[cur_item];
          }
        }
        break;
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
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t cur_item = 0;

  // fill items
  for (uint8_t i = 0; i < COUNT(totalItems); i++)
  {
    if (infoSettings.marlin_mode_font_color == lcd_colors[i])
    {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    }
    else
    {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel = lcd_color_names[i];
  }

  listWidgetCreate(title, totalItems, COUNT(totalItems), cur_item/ LISTITEM_PER_PAGE);

  while (infoMenu.menu[infoMenu.cur] == menuEmulatorFontColor)
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
        if (key_num < LISTITEM_PER_PAGE)
        {
          uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
          if (tmp_i < LCD_COLOR_COUNT && tmp_i != cur_item)  // has changed
          {
            totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
            listWidgetRefreshItem(cur_item);  // refresh unchecked status
            cur_item = tmp_i;
            totalItems[cur_item].icon = ICONCHAR_CHECKED;
            listWidgetRefreshItem(cur_item);  // refresh checked status

            infoSettings.marlin_mode_font_color = lcd_colors[cur_item];
          }
        }
        break;
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
  LISTITEMS marlinModeitems = {
    // title
    LABEL_MARLIN_MODE_SETTINGS,
    // icon                       ItemType          Item Title               item value text(only for custom value)
    {
      {ICONCHAR_FONT_COLOR,       LIST_CUSTOMVALUE, LABEL_FONT_COLOR,        LABEL_CUSTOM},
      {ICONCHAR_BACKGROUND_COLOR, LIST_CUSTOMVALUE, LABEL_BG_COLOR,          LABEL_CUSTOM},
      {ICONCHAR_TOGGLE_ON,        LIST_TOGGLE,      LABEL_MARLIN_FULLSCREEN, LABEL_BACKGROUND},
      {ICONCHAR_TOGGLE_ON,        LIST_TOGGLE,      LABEL_MARLIN_SHOW_TITLE, LABEL_BACKGROUND},
      {ICONCHAR_BLANK,            LIST_CUSTOMVALUE, LABEL_MARLIN_TYPE,       LABEL_DYNAMIC},
      {ICONCHAR_BACKGROUND,       LIST_LABEL,       LABEL_BACKGROUND,        LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND,       LIST_LABEL,       LABEL_BACKGROUND,        LABEL_BACKGROUND},
      {ICONCHAR_BACK,             LIST_LABEL,       LABEL_BACKGROUND,        LABEL_BACKGROUND},
    }
  };

  for (uint8_t i = 0; i < LCD_COLOR_COUNT; i++)
  {
    if (infoSettings.marlin_mode_font_color == lcd_colors[i])
      marlinModeitems.items[0].valueLabel = lcd_color_names[i];

    if (infoSettings.marlin_mode_bg_color == lcd_colors[i])
      marlinModeitems.items[1].valueLabel = lcd_color_names[i];
  }

  marlinModeitems.items[2].icon = (infoSettings.marlin_mode_fullscreen == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  marlinModeitems.items[3].icon = (infoSettings.marlin_mode_showtitle == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  setDynamicTextValue(4, (char *)labelMarlinType[infoSettings.marlin_type]);

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  menuDrawListPage(&marlinModeitems);

  while (infoMenu.menu[infoMenu.cur] == menuMarlinModeSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuEmulatorFontColor;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuEmulatorBGColor;
        break;

      case KEY_ICON_2:
        infoSettings.marlin_mode_fullscreen = (infoSettings.marlin_mode_fullscreen + 1) % 2;
        marlinModeitems.items[key_num].icon = (infoSettings.marlin_mode_fullscreen == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
        menuDrawListItem(&marlinModeitems.items[key_num], key_num);
        break;

      case KEY_ICON_3:
        infoSettings.marlin_mode_showtitle = (infoSettings.marlin_mode_showtitle + 1) % 2;
        marlinModeitems.items[key_num].icon = (infoSettings.marlin_mode_showtitle == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
        menuDrawListItem(&marlinModeitems.items[key_num], key_num);
        break;

      case KEY_ICON_4:
        infoSettings.marlin_type = (infoSettings.marlin_type + 1) % ITEM_MARLIN_TYPE_NUM;
        setDynamicTextValue(key_num, (char *)labelMarlinType[infoSettings.marlin_type]);
        menuDrawListItem(&marlinModeitems.items[key_num], key_num);
        break;

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

#endif // ST7920_EMULATOR

#ifdef BUZZER_PIN

void menuSoundSettings(void)
{
  LISTITEMS sounditems = {
    // title
    LABEL_FEATURE_SETTINGS,
    // icon                 ItemType     Item Title          item value text(only for custom value)
    {
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE, LABEL_TOUCH_SOUND,  LABEL_BACKGROUND},
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE, LABEL_TOAST_SOUND,  LABEL_BACKGROUND},
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE, LABEL_ALERT_SOUND,  LABEL_BACKGROUND},
      {ICONCHAR_TOGGLE_ON,  LIST_TOGGLE, LABEL_HEATER_SOUND, LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
      {ICONCHAR_BACK,       LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  sounditems.items[0].icon = (infoSettings.touchSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[2].icon = (infoSettings.toastSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[1].icon = (infoSettings.alertSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[3].icon = (infoSettings.heaterSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;

  menuDrawListPage(&sounditems);

  while (infoMenu.menu[infoMenu.cur] == menuSoundSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoSettings.touchSound = (infoSettings.touchSound + 1) % 2;
        sounditems.items[key_num].icon = (infoSettings.touchSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
        menuDrawListItem(&sounditems.items[key_num], key_num);
        break;

      case KEY_ICON_1:
        infoSettings.toastSound = (infoSettings.toastSound + 1) % 2;
        sounditems.items[key_num].icon = (infoSettings.toastSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
        menuDrawListItem(&sounditems.items[key_num], key_num);
        break;

      case KEY_ICON_2:
        infoSettings.alertSound = (infoSettings.alertSound + 1) % 2;
        sounditems.items[key_num].icon = (infoSettings.alertSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
        menuDrawListItem(&sounditems.items[key_num], key_num);
        break;

      case KEY_ICON_3:
        infoSettings.heaterSound = (infoSettings.heaterSound + 1) % 2;
        sounditems.items[key_num].icon = (infoSettings.heaterSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
        menuDrawListItem(&sounditems.items[key_num], key_num);
        break;

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
} //menuSoundSettings

#endif // BUZZER_PIN

#ifdef LCD_LED_PWM_CHANNEL

void menuBrightnessSettings(void)
{
  LISTITEMS brightnessitems = {
    // title
    LABEL_LCD_BRIGHTNESS,
    // icon                 ItemType          Item Title                 item value text(only for custom value)
    {
      {ICONCHAR_BLANK,      LIST_CUSTOMVALUE, LABEL_LCD_BRIGHTNESS,      LABEL_DYNAMIC},
      {ICONCHAR_BLANK,      LIST_CUSTOMVALUE, LABEL_LCD_IDLE_BRIGHTNESS, LABEL_DYNAMIC},
      {ICONCHAR_BLANK,      LIST_CUSTOMVALUE, LABEL_LCD_IDLE_DELAY,      LABEL_DYNAMIC},
      {ICONCHAR_BACKGROUND, LIST_LABEL,       LABEL_BACKGROUND,          LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,       LABEL_BACKGROUND,          LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,       LABEL_BACKGROUND,          LABEL_BACKGROUND},
      {ICONCHAR_BACKGROUND, LIST_LABEL,       LABEL_BACKGROUND,          LABEL_BACKGROUND},
      {ICONCHAR_BACK,       LIST_LABEL,       LABEL_BACKGROUND,          LABEL_BACKGROUND},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  char tempstr[8];

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
  setDynamicTextValue(KEY_ICON_0, tempstr);

  sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
  setDynamicTextValue(KEY_ICON_1, tempstr);

  brightnessitems.items[KEY_ICON_2].valueLabel = itemDimTime[infoSettings.lcd_idle_timer];

  menuDrawListPage(&brightnessitems);

  while (infoMenu.menu[infoMenu.cur] == menuBrightnessSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      {
        infoSettings.lcd_brightness = (infoSettings.lcd_brightness + 1) % ITEM_BRIGHTNESS_NUM;

        if (infoSettings.lcd_brightness == 0)
          infoSettings.lcd_brightness = 1;  //In Normal it should not be off. Set back to 5%

        char tempstr[8];
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        setDynamicTextValue(key_num, tempstr);
        Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
        menuDrawListItem(&brightnessitems.items[key_num], key_num);
        break;
      }

      case KEY_ICON_1:
      {
        infoSettings.lcd_idle_brightness = (infoSettings.lcd_idle_brightness + 1) % ITEM_BRIGHTNESS_NUM;
        char tempstr[8];
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), LCD_BRIGHTNESS[infoSettings.lcd_idle_brightness]);
        setDynamicTextValue(key_num, tempstr);
        menuDrawListItem(&brightnessitems.items[key_num], key_num);
        break;
      }

      case KEY_ICON_2:
        infoSettings.lcd_idle_timer = (infoSettings.lcd_idle_timer + 1) % ITEM_SECONDS_NUM;
        brightnessitems.items[key_num].valueLabel = itemDimTime[infoSettings.lcd_idle_timer];
        menuDrawListItem(&brightnessitems.items[key_num], key_num);
        break;

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

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  menuDrawPage(&screenSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuScreenSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
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
