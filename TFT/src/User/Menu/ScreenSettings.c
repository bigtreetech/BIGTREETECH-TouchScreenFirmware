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
  static u8 item_silent_i = 0;
#endif


#ifdef ST7920_SPI // LCD12864 color settings
  #ifdef BUZZER_PIN
    #define LCD12864_BG_INDEX KEY_ICON_4
  #else
    #define LCD12864_BG_INDEX KEY_ICON_3
  #endif
  #define LCD12864_FN_INDEX (LCD12864_BG_INDEX+1)

#endif

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
  LABEL_DARKGRAY
  };
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
  {ICON_BACK,                 LABEL_BACK},}
};

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
    // LCD12864 background color
    screenSettingsItems.items[KEY_ICON_4].icon = ICON_BKCOLOR;
    screenSettingsItems.items[KEY_ICON_4].label = lcd_color_names[infoSettings.marlin_mode_bg_color];

    // LCD12864 font color
    screenSettingsItems.items[KEY_ICON_5].icon = ICON_FONTCOLOR;
    screenSettingsItems.items[KEY_ICON_5].label = lcd_color_names[infoSettings.marlin_mode_font_color];

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
        statusScreen_setMsg(textSelect(LABEL_STATUS), textSelect(infoHost.connected ? LABEL_READY : LABEL_UNCONNECTED)); // restore msg buffer when language is changed
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
        infoSettings.marlin_mode_bg_color = (infoSettings.marlin_mode_bg_color + 1) % LCD_COLOR_COUNT;
        screenSettingsItems.items[key_num].label = lcd_color_names[infoSettings.marlin_mode_bg_color];
        menuDrawItem(&screenSettingsItems.items[key_num], key_num);
        break;

      case LCD12864_FN_INDEX:
        infoSettings.marlin_mode_font_color = (infoSettings.marlin_mode_font_color + 1) % LCD_COLOR_COUNT;
        screenSettingsItems.items[key_num].label = lcd_color_names[infoSettings.marlin_mode_font_color];
        menuDrawItem(&screenSettingsItems.items[key_num], key_num);
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
