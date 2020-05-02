#include "ScreenSettings.h"
#include "includes.h"

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
  {ICON_BACK,                 LABEL_BACK},}
};

#ifdef BUZZER_PIN // Speaker
  #define BUZZER_KEY_INDEX KEY_ICON_3

  #define ITEM_SILENT_NUM 2
  const ITEM itemSilent[ITEM_SILENT_NUM] = {
  // icon                       label
    {ICON_SILENT_OFF,           LABEL_SILENT_OFF},
    {ICON_SILENT_ON,            LABEL_SILENT_ON},
  };
  const  u8 item_silent[ITEM_SILENT_NUM] = {0, 1};
  static u8 item_silent_i = 0;
#endif


#ifdef ST7920_SPI // LCD12864 color settings
  #ifdef BUZZER_PIN
    #define LCD12864_BG_INDEX KEY_ICON_4
  #else
    #define LCD12864_BG_INDEX KEY_ICON_3
  #endif
  #define LCD12864_FN_INDEX (LCD12864_BG_INDEX+1)

  #define ITEM_COLOR_NUM 9
  const ITEM itemBGcolor[ITEM_COLOR_NUM] = {
  // icon                      label
    {ICON_BKCOLOR,             LABEL_WHITE},
    {ICON_BKCOLOR,             LABEL_BLACK},
    {ICON_BKCOLOR,             LABEL_BLUE},
    {ICON_BKCOLOR,             LABEL_RED},
    {ICON_BKCOLOR,             LABEL_GREEN},
    {ICON_BKCOLOR,             LABEL_CYAN},
    {ICON_BKCOLOR,             LABEL_YELLOW},
    {ICON_BKCOLOR,             LABEL_BROWN},
    {ICON_BKCOLOR,             LABEL_GRAY},
  };

  const ITEM itemFontcolor[ITEM_COLOR_NUM] = {
  // icon                      label
    {ICON_FONTCOLOR,           LABEL_WHITE},
    {ICON_FONTCOLOR,           LABEL_BLACK},
    {ICON_FONTCOLOR,           LABEL_BLUE},
    {ICON_FONTCOLOR,           LABEL_RED},
    {ICON_FONTCOLOR,           LABEL_GREEN},
    {ICON_FONTCOLOR,           LABEL_CYAN},
    {ICON_FONTCOLOR,           LABEL_YELLOW},
    {ICON_FONTCOLOR,           LABEL_BROWN},
    {ICON_FONTCOLOR,           LABEL_GRAY},
  };
  const  u32 item_color[ITEM_COLOR_NUM] = {WHITE, BLACK, BLUE, RED, GREEN, CYAN, YELLOW, BROWN, GRAY};
  static u8  item_bgcolor_i = 0;
  static u8  item_fontcolor_i = 0;
#endif

void menuScreenSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  #ifdef BUZZER_PIN
    for(u8 i=0; i<ITEM_SILENT_NUM; i++)
    {
      if(infoSettings.silent == item_silent[i])
      {
        item_silent_i = i;
        screenSettingsItems.items[BUZZER_KEY_INDEX] = itemSilent[i];
      }
    }
  #endif

  #ifdef ST7920_SPI
    for(u8 i=0; i<ITEM_COLOR_NUM; i++) // LCD12864 background color
    {
      if(infoSettings.bg_color == item_color[i])
      {
        item_bgcolor_i = i;
        screenSettingsItems.items[KEY_ICON_4] = itemBGcolor[item_bgcolor_i];
      }
    }
    for(u8 i=0; i<ITEM_COLOR_NUM; i++) // LCD12864 font color
    {
      if(infoSettings.font_color == item_color[i])
      {
        item_fontcolor_i = i;
        screenSettingsItems.items[KEY_ICON_5] = itemFontcolor[item_fontcolor_i];
      }
    }
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
        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
        menuDrawPage(&screenSettingsItems);
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
        item_bgcolor_i = (item_bgcolor_i + 1) % ITEM_COLOR_NUM;
        screenSettingsItems.items[key_num] = itemBGcolor[item_bgcolor_i];
        menuDrawItem(&screenSettingsItems.items[key_num], key_num);
        infoSettings.bg_color = item_color[item_bgcolor_i];
        break;

      case LCD12864_FN_INDEX:
        item_fontcolor_i = (item_fontcolor_i + 1) % ITEM_COLOR_NUM;
        screenSettingsItems.items[key_num] = itemFontcolor[item_fontcolor_i];
        menuDrawItem(&screenSettingsItems.items[key_num], key_num);
        infoSettings.font_color = item_color[item_fontcolor_i];
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
