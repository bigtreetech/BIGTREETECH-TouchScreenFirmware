#include "Settings.h"
#include "includes.h"


SETTINGS infoSettings;

void infoSettingsReset(void)
{
  TSC_Calibration();
  infoSettings.baudrate = 115200;
  infoSettings.language = ENGLISH;
  infoSettings.mode = SERIAL_TSC;
  infoSettings.runout = 0;
  infoSettings.rotate_ui = 0;
  infoSettings.bg_color = ST7920_BKCOLOR;
  infoSettings.font_color = ST7920_FNCOLOR;
  storePara();  
}

void menuInfo(void)
{
  const char* hardware = "Board   : BIGTREETECH_" HARDWARE_VERSION;
  const char* firmware = "Firmware: "HARDWARE_VERSION"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__;
  
  u16 HW_X = (LCD_WIDTH - my_strlen((u8 *)hardware)*BYTE_WIDTH)/2;
  u16 FW_X = (LCD_WIDTH - my_strlen((u8 *)firmware)*BYTE_WIDTH)/2;
  u16 centerY = LCD_HEIGHT/2;
  u16 startX = MIN(HW_X, FW_X);
  
  GUI_Clear(BLACK);

  GUI_DispString(startX, centerY - BYTE_HEIGHT, (u8 *)hardware, 0);
  GUI_DispString(startX, centerY, (u8 *)firmware, 0);

  while(!isPress()) loopProcess();
  while(isPress())  loopProcess();

  infoMenu.cur--;
}

void menuDisconnect(void)
{
  GUI_Clear(BLACK);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO), 0);

  Serial_DeConfig();
  while(!isPress());
  while(isPress());
  Serial_Config(infoSettings.baudrate);
  
  infoMenu.cur--;
}

/*-------------------------------more setting--------------top*/
MENUITEMS moresettingsItems = {
// title
LABEL_SETTINGS,
// icon                       label
 {{ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_BKCOLOR,              LABEL_BACKGROUND},
  {ICON_FONTCOLOR,            LABEL_BACKGROUND},
  {ICON_GCODE,                LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_color_NUM 9  
const ITEM itemBGcolor[ITEM_color_NUM] = {
// icon                       label
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

const ITEM itemFontcolor[ITEM_color_NUM] = {
// icon                       label
  {ICON_FONTCOLOR,             LABEL_WHITE},
  {ICON_FONTCOLOR,             LABEL_BLACK},
  {ICON_FONTCOLOR,             LABEL_BLUE},
  {ICON_FONTCOLOR,             LABEL_RED},
  {ICON_FONTCOLOR,             LABEL_GREEN},
  {ICON_FONTCOLOR,             LABEL_CYAN},
  {ICON_FONTCOLOR,             LABEL_YELLOW},
  {ICON_FONTCOLOR,             LABEL_BROWN},
  {ICON_FONTCOLOR,             LABEL_GRAY},
};
const  u32 item_color[ITEM_color_NUM] = {WHITE,BLACK,BLUE,RED,GREEN,CYAN,YELLOW,BROWN,GRAY};
static u8  item_bgcolor_i = 0;
static u8  item_fontcolor_i = 0;

void moremenuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;
  
  for(u8 i=0; i<ITEM_color_NUM; i++)
  {
    if(infoSettings.bg_color == item_color[i])
    {
      item_bgcolor_i = i;
      moresettingsItems.items[KEY_ICON_1] = itemBGcolor[item_bgcolor_i];
    }
  }
  
  for(u8 i=0; i<ITEM_color_NUM; i++)
  {
    if(infoSettings.font_color == item_color[i])
    {
      item_fontcolor_i = i;
      moresettingsItems.items[KEY_ICON_2] = itemFontcolor[item_fontcolor_i];
    }
  }
  
  menuDrawPage(&moresettingsItems);
  
  while(infoMenu.menu[infoMenu.cur] == moremenuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;
      
      case KEY_ICON_1:
        item_bgcolor_i = (item_bgcolor_i + 1) % ITEM_color_NUM;                
        moresettingsItems.items[key_num] = itemBGcolor[item_bgcolor_i];
        menuDrawItem(&moresettingsItems.items[key_num], key_num);
        infoSettings.bg_color = item_color[item_bgcolor_i];
        break;
      
      case KEY_ICON_2:
        item_fontcolor_i = (item_fontcolor_i + 1) % ITEM_color_NUM;                
        moresettingsItems.items[key_num] = itemFontcolor[item_fontcolor_i];
        menuDrawItem(&moresettingsItems.items[key_num], key_num);
        infoSettings.font_color = item_color[item_fontcolor_i];
        break;
      
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        break;
      
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
/*-------------------------------more setting--------------end*/
MENUITEMS settingsItems = {
// title
LABEL_SETTINGS,
// icon                       label
 {{ICON_ROTATE_UI,            LABEL_ROTATE_UI},
  {ICON_LANGUAGE,             LABEL_LANGUAGE},
  {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
  {ICON_MORESETTING,          LABEL_MORESETTING},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};


#define ITEM_BAUDRATE_NUM 2
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
// icon                       label
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_250000},
};
const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200,250000};
static u8  item_baudrate_i = 0;

#define ITEM_RUNOUT_NUM 2
const ITEM itemRunout[ITEM_RUNOUT_NUM] = {
// icon                       label
  {ICON_RUNOUT,             LABEL_RUNOUT_OFF},
  {ICON_RUNOUT,             LABEL_RUNOUT_ON},
};
const  u32 item_runout[ITEM_RUNOUT_NUM] = {0,1};
static u8  item_runout_i = 0;

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }
  #ifdef FIL_RUNOUT_PIN
  for(u8 i=0; i<ITEM_RUNOUT_NUM; i++)
  {
    if(infoSettings.runout == item_runout[i])
    {
      item_runout_i = i;
      settingsItems.items[KEY_ICON_6] = itemRunout[item_runout_i];
    }
  }
  #endif
  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoSettings.rotate_ui = !infoSettings.rotate_ui;
        LCD_RefreshDirection();
        TSC_Calibration();
        menuDrawPage(&settingsItems);        
        break;
      
      case KEY_ICON_1: 
        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_2:
        TSC_Calibration();
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = moremenuSettings;
        break;
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;
      
      case KEY_ICON_5:
        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;                
        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.baudrate = item_baudrate[item_baudrate_i];
        Serial_Config(infoSettings.baudrate);
        break;
      #ifdef FIL_RUNOUT_PIN
      case KEY_ICON_6:
        item_runout_i = (item_runout_i + 1) % ITEM_RUNOUT_NUM;                
        settingsItems.items[key_num] = itemRunout[item_runout_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.runout = item_runout[item_runout_i];
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
