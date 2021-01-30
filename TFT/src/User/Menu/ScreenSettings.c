#include "ScreenSettings.h"
#include "includes.h"
#include "Colors.h"

#ifdef BUZZER_PIN // Speaker
  #define BUZZER_KEY_INDEX KEY_ICON_3
#endif


#ifdef ST7920_SPI // LCD12864 color settings
  #ifdef BUZZER_PIN
    #define LCD12864_BG_INDEX KEY_ICON_4
  #else
    #define LCD12864_BG_INDEX KEY_ICON_3
  #endif
  #define LCD12864_FN_INDEX (LCD12864_BG_INDEX+1)
#endif


void menuLanguage(void)
{
  #define LANGUAGE_PAGE_COUNT  (LANGUAGE_NUM + LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE
  LABEL title = {LABEL_LANGUAGE};
  LISTITEM totalItems[LANGUAGE_NUM];
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  // fill language items
  uint8_t tmp_language = infoSettings.language;
  for(uint8_t i = 0; i < COUNT(totalItems); i++)
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
      if(key_num < LISTITEM_PER_PAGE)
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

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    statusScreen_setReady(); // restore msg buffer when language is changed
    storePara();
  }
}

#ifdef ST7920_SPI

void menuSimulatorBackGroundColor(void)
{
  LABEL title = {LABEL_SIMULATOR_BG_COLOR};
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

  while (infoMenu.menu[infoMenu.cur] == menuSimulatorBackGroundColor)
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
        if (tmp_i < LCD_COLOR_COUNT && tmp_i != cur_item) // has changed
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

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}

void menuSimulatorFontColor(void)
{
  LABEL title = {LABEL_SIMULATOR_FONT_COLOR};
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

  while (infoMenu.menu[infoMenu.cur] == menuSimulatorFontColor)
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
          listWidgetRefreshItem(cur_item); // refresh unchecked status
          cur_item = tmp_i;
          totalItems[cur_item].icon = ICONCHAR_CHECKED;
          listWidgetRefreshItem(cur_item); // refresh checked status

          infoSettings.marlin_mode_font_color = lcd_colors[cur_item];
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
#endif // ST7920_SPI

#ifdef BUZZER_PIN

void menuSoundSettings(void)
{
  LISTITEMS sounditems = {
  // title
  LABEL_FEATURE_SETTINGS,
  // icon                 ItemType      Item Title        item value text(only for custom value)
  {
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_TOUCH_SOUND,  LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_TOAST_SOUND,  LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_ALERT_SOUND,  LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE,  LABEL_HEATER_SOUND,  LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND, LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},
    {ICONCHAR_BACK,       LIST_LABEL,  LABEL_BACKGROUND,   LABEL_BACKGROUND},}
  };

  sounditems.items[0].icon = (infoSettings.touchSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[2].icon = (infoSettings.toastSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[1].icon = (infoSettings.alertSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;
  sounditems.items[3].icon = (infoSettings.heaterSound == 1) ? ICONCHAR_TOGGLE_ON : ICONCHAR_TOGGLE_OFF;

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  menuDrawListPage(&sounditems);

  while (infoMenu.menu[infoMenu.cur] == menuSoundSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
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

void menuScreenSettings(void)
{
  MENUITEMS screenSettingsItems = {
  // title
  LABEL_SCREEN_SETTINGS,
  // icon                       label
   {{ICON_ROTATE_UI,            LABEL_ROTATE_UI},
    {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
    {ICON_LANGUAGE,             LABEL_LANGUAGE},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK}}
  };

  //load buzzer icon
  #ifdef BUZZER_PIN
    screenSettingsItems.items[BUZZER_KEY_INDEX].icon = ICON_SOUND;
    screenSettingsItems.items[BUZZER_KEY_INDEX].label.index = LABEL_SOUND;
  #endif

  #ifdef ST7920_SPI
    // LCD12864 background color
    screenSettingsItems.items[LCD12864_BG_INDEX].icon = ICON_BKCOLOR;
    screenSettingsItems.items[LCD12864_BG_INDEX].label.index = LABEL_CUSTOM;

    screenSettingsItems.items[LCD12864_FN_INDEX].icon = ICON_FONTCOLOR;
    screenSettingsItems.items[LCD12864_FN_INDEX].label.index = LABEL_CUSTOM;

    for(u8 i = 0; i < LCD_COLOR_COUNT; i++)
    {
      if(infoSettings.marlin_mode_bg_color == lcd_colors[i])
      {
        screenSettingsItems.items[LCD12864_BG_INDEX].label = lcd_color_names[i];
      }
    }

    // LCD12864 font color
    for(u8 i = 0; i < LCD_COLOR_COUNT; i++)
    {
      if(infoSettings.marlin_mode_font_color == lcd_colors[i])
      {
        screenSettingsItems.items[LCD12864_FN_INDEX].label = lcd_color_names[i];
      }
    }

  #endif

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  menuDrawPage(&screenSettingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuScreenSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
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
          popupReminder(DIALOG_TYPE_ALERT,(u8*)"Language not available", (u8*)"To change Language first flash a Language pack ini file.");
        break;

      #ifdef BUZZER_PIN
      case BUZZER_KEY_INDEX:
        infoMenu.menu[++infoMenu.cur] = menuSoundSettings;
        break;
      #endif

      #ifdef ST7920_SPI
      case LCD12864_BG_INDEX:
        infoMenu.menu[++infoMenu.cur] = menuSimulatorBackGroundColor;
        break;

      case LCD12864_FN_INDEX:
        infoMenu.menu[++infoMenu.cur] = menuSimulatorFontColor;
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

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
