#include "ScreenSettings.h"
#include "includes.h"
#include "Colors.h"

#ifdef BUZZER_PIN // Speaker
  #define BUZZER_KEY_INDEX KEY_ICON_3

  #define ITEM_SILENT_NUM 2
  const ITEM itemSilent[ITEM_SILENT_NUM] = {
  // icon                       label
    {ICON_SILENT_OFF,           LABEL_SILENT_OFF},
    {ICON_SILENT_ON,            LABEL_SILENT_ON},
  };
  const  u8 item_silent[ITEM_SILENT_NUM] = {0, 1};
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
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (i == tmp_language) {
      totalItems[i].icon = ICONCHAR_CHECKED;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    infoSettings.language = i;
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel.address = textSelect(LABEL_LANGUAGE);
  }
  infoSettings.language = tmp_language;

  listWidgetCreat(title, totalItems, COUNT(totalItems), infoSettings.language / LISTITEM_PER_PAGE);

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
      if(key_num < LISTITEM_PER_PAGE){
        uint16_t cur_item = infoSettings.language;
        uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
        if (tmp_i != cur_item) { // has changed
          totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
          listWidgetRefreshItem(cur_item); // refresh unchecked status
          cur_item = tmp_i;
          totalItems[cur_item].icon = ICONCHAR_CHECKED;
          listWidgetRefreshItem(cur_item); // refresh checked status

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
    statusScreen_setMsg(textSelect(LABEL_STATUS), textSelect(infoHost.connected ? LABEL_READY : LABEL_UNCONNECTED)); // restore msg buffer when language is changed
    storePara();
  }
}

#ifdef ST7920_SPI
const LABEL lcd_colors_names[LCD_COLOR_COUNT] =
{
  LABEL_WHITE,
  LABEL_BLACK,
  LABEL_RED,
  LABEL_GREEN,
  LABEL_BLUE,
  LABEL_CYAN,
  LABEL_MAGENTA,
  LABEL_YELLOW,
  LABEL_ORANGE,
  LABEL_PURPLE,
  LABEL_LIME,
  LABEL_BROWN,
  LABEL_DARKBLUE,
  LABEL_DARKGREEN,
  LABEL_GRAY,
  LABEL_DARKGRAY,
};

void menuSimulatorBackGroundColor(void)
{
  LABEL title = {LABEL_SIMULATOR_BG_COLOR};
  LISTITEM totalItems[LCD_COLOR_COUNT];
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  uint8_t cur_item;

  // fill items
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (infoSettings.marlin_mode_bg_color == lcd_colors[i]) {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel = lcd_colors_names[i];
  }

  listWidgetCreat(title, totalItems, COUNT(totalItems), cur_item/ LISTITEM_PER_PAGE);

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
      if(key_num < LISTITEM_PER_PAGE){
        uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
        if (tmp_i != cur_item) { // has changed
          totalItems[cur_item].icon = ICONCHAR_UNCHECKED;
          listWidgetRefreshItem(cur_item); // refresh unchecked status
          cur_item = tmp_i;
          totalItems[cur_item].icon = ICONCHAR_CHECKED;
          listWidgetRefreshItem(cur_item); // refresh checked status

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
  uint8_t cur_item;

  // fill items
  for(uint8_t i = 0; i < COUNT(totalItems); i++) {
    if (infoSettings.marlin_mode_font_color == lcd_colors[i]) {
      totalItems[i].icon = ICONCHAR_CHECKED;
      cur_item = i;
    } else {
      totalItems[i].icon = ICONCHAR_UNCHECKED;
    }
    totalItems[i].itemType = LIST_LABEL;
    totalItems[i].titlelabel = lcd_colors_names[i];
  }

  listWidgetCreat(title, totalItems, COUNT(totalItems), cur_item/ LISTITEM_PER_PAGE);

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
      if(key_num < LISTITEM_PER_PAGE){
        uint16_t tmp_i = listWidgetGetCurPage() * LISTITEM_PER_PAGE + key_num;
        if (tmp_i != cur_item) { // has changed
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
#endif

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
  #ifdef ST7920_SPI
    {ICON_BKCOLOR,              LABEL_BACKGROUND},
    {ICON_FONTCOLOR,            LABEL_BACKGROUND},
  #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
  #endif
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},}
  };
  u8 item_silent_i = 0;
  u8 item_marlin_bg_color_i = 0;
  u8 item_marlin_font_color_i = 0;

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  #ifdef BUZZER_PIN
    for(u8 i = 0; i < ITEM_SILENT_NUM; i++)
    {
      if(infoSettings.silent == item_silent[i])
      {
        item_silent_i = i;
        screenSettingsItems.items[BUZZER_KEY_INDEX] = itemSilent[i];
      }
    }
  #endif

  #ifdef ST7920_SPI
    // LCD12864 background color
    bool inArry = false;
    for(u8 i = 0; i < LCD_COLOR_COUNT; i++)
    {
      if(infoSettings.marlin_mode_bg_color == lcd_colors[i])
      {
        inArry = true;
        item_marlin_bg_color_i = i;
        screenSettingsItems.items[KEY_ICON_4].label = lcd_color_names[item_marlin_bg_color_i];
      }
    }
    if (!inArry) screenSettingsItems.items[KEY_ICON_4].label.index = LABEL_CUSTOM;

    // LCD12864 font color
    inArry = false;
    for(u8 i = 0; i < LCD_COLOR_COUNT; i++)
    {
      if(infoSettings.marlin_mode_font_color == lcd_colors[i])
      {
        inArry = true;
        item_marlin_font_color_i = i;
        screenSettingsItems.items[KEY_ICON_5].label = lcd_color_names[item_marlin_font_color_i];
      }
    }
    if (!inArry) screenSettingsItems.items[KEY_ICON_5].label.index = LABEL_CUSTOM;

  #endif

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
        infoMenu.menu[++infoMenu.cur] = menuLanguage;
        break;

      #ifdef BUZZER_PIN
      case BUZZER_KEY_INDEX:
        item_silent_i = (item_silent_i + 1) % ITEM_SILENT_NUM;
        screenSettingsItems.items[key_num] = itemSilent[item_silent_i];
        menuDrawItem(&screenSettingsItems.items[key_num], key_num);
        infoSettings.silent = item_silent[item_silent_i];
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
