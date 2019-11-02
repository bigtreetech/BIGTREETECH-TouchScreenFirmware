#include "MachineSettings.h"
#include "includes.h"

MENUITEMS RGBItems = {
// title
LABEL_RGB_SETTINGS,
// icon                       label
 {{ICON_RGB_RED,              LABEL_RGB_RED},
  {ICON_RGB_GREEN,            LABEL_RGB_GREEN},
  {ICON_RGB_BLUE,             LABEL_RGB_BLUE},
  {ICON_RGB_WHITE,            LABEL_RGB_WHITE},
  {ICON_RGB_OFF,              LABEL_RGB_OFF},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuRGBSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&RGBItems);

  while(infoMenu.menu[infoMenu.cur] == menuRGBSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:  //Red
        storeCmd("M150 R255 U0 B0 P255\n");
        break;
      
      case KEY_ICON_1:  //Green
        storeCmd("M150 R0 U255 B0 P255\n");
        break;
      
      case KEY_ICON_2:  //Blue
        storeCmd("M150 R0 U0 B255 P255\n");
        break;
      
      case KEY_ICON_3:  //White
        storeCmd("M150 R255 U255 B255 P255\n");
        break;
      
      case KEY_ICON_4:  //Turn Off
        storeCmd("M150 R0 U0 B0 P0\n");
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    loopProcess();		
  }
}


MENUITEMS machineSettingsItems = {
// title
LABEL_MACHINE_SETTINGS,
// icon                       label
 {{ICON_CUSTOM,               LABEL_CUSTOM},
  {ICON_RGB_SETTINGS,         LABEL_RGB_SETTINGS},
//  {ICON_BLTOUCH,              LABEL_BLTOUCH},
  {ICON_GCODE,                LABEL_GCODE},
  {ICON_SHUT_DOWN,            LABEL_SHUT_DOWN},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuMachineSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  
  menuDrawPage(&machineSettingsItems);
  
  while(infoMenu.menu[infoMenu.cur] == menuMachineSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuRGBSettings;
        break;
      
      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        break;
      
      case KEY_ICON_3:
        storeCmd("M81\n");
      break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    
    loopProcess();  
  }
}

//void moremenuSettings(void)
//{
//  KEY_VALUES key_num = KEY_IDLE;
//  SETTINGS now = infoSettings;
//  

//  
//  menuDrawPage(&moresettingsItems);
//  
//  while(infoMenu.menu[infoMenu.cur] == moremenuSettings)
//  {
//    key_num = menuKeyGetValue();
//    switch(key_num)
//    {
//      case KEY_ICON_0:
//        infoMenu.menu[++infoMenu.cur] = menuInfo;
//        break;
//      

//      
//      case KEY_ICON_3:
//        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
//        break;
//      
//      case KEY_ICON_7:
//        infoMenu.cur--;
//        break;
//      
//      default:
//        break;
//    }
//    
//    loopProcess();
//  }
//  
//  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
//  {
//    storePara();
//  }
//}
///*-------------------------------more setting--------------end*/




//MENUITEMS settingsItems = {
//// title
//LABEL_SETTINGS,
//// icon                       label
// {{ICON_SCREEN_SETTINGS,      LABEL_TSC_SETTINGS},
//  {ICON_MACHINE_SETTINGS,     LABEL_MACHINE_SETTINGS},
//  {ICON_FEATURE_SETTINGS,     LABEL_FEATURE_SETTINGS},
//  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
//  {ICON_BACKGROUND,           LABEL_BACKGROUND},
//  {ICON_BACKGROUND,           LABEL_BACKGROUND},
//  {ICON_BACKGROUND,           LABEL_BACKGROUND},
//  {ICON_BACK,                 LABEL_BACK},}
//};


//#define ITEM_BAUDRATE_NUM 2
//const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
//// icon                       label
//  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
//  {ICON_BAUDRATE,             LABEL_BAUDRATE_250000},
//};
//const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200,250000};
//static u8  item_baudrate_i = 0;

//#define ITEM_RUNOUT_NUM 3
//const ITEM itemRunout[ITEM_RUNOUT_NUM] = {
//// icon                       label
//  {ICON_RUNOUT,             LABEL_RUNOUT_OFF},
//  {ICON_RUNOUT,             LABEL_RUNOUT_ON},
//  {ICON_RUNOUT,             LABEL_SMART_RUNOUT_ON},
//};
//const  u32 item_runout[ITEM_RUNOUT_NUM] = {FILAMENT_RUNOUT_OFF, FILAMENT_RUNOUT_ON, FILAMENT_SMART_RUNOUT_ON};
//static u8  item_runout_i = 0;

//void menuSettings(void)
//{
//  KEY_VALUES key_num = KEY_IDLE;
//  SETTINGS now = infoSettings;

//  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
//  {
//    if(infoSettings.baudrate == item_baudrate[i])
//    {
//      item_baudrate_i = i;
//      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
//    }
//  }
//  #ifdef FIL_RUNOUT_PIN
//  for(u8 i=0; i<ITEM_RUNOUT_NUM; i++)
//  {
//    if(infoSettings.runout == item_runout[i])
//    {
//      item_runout_i = i;
//      settingsItems.items[KEY_ICON_6] = itemRunout[item_runout_i];
//    }
//  }
//  #endif
//  menuDrawPage(&settingsItems);

//  while(infoMenu.menu[infoMenu.cur] == menuSettings)
//  {
//    key_num = menuKeyGetValue();
//    switch(key_num)
//    {
//      case KEY_ICON_0:
//        infoSettings.rotate_ui = !infoSettings.rotate_ui;
//        LCD_RefreshDirection();
//        TSC_Calibration();
//        menuDrawPage(&settingsItems);        
//        break;
//      
//      case KEY_ICON_1: 
//        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
//        menuDrawPage(&settingsItems);
//        break;
//      
//      case KEY_ICON_2:
//        TSC_Calibration();
//        menuDrawPage(&settingsItems);
//        break;
//      
//      case KEY_ICON_3:
//        infoMenu.menu[++infoMenu.cur] = moremenuSettings;
//        break;
//      case KEY_ICON_4:
//        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
//        break;
//      
//      case KEY_ICON_5:
//        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;                
//        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
//        menuDrawItem(&settingsItems.items[key_num], key_num);
//        infoSettings.baudrate = item_baudrate[item_baudrate_i];
//        Serial_Config(infoSettings.baudrate);
//        break;
//      #ifdef FIL_RUNOUT_PIN
//      case KEY_ICON_6:
//        item_runout_i = (item_runout_i + 1) % ITEM_RUNOUT_NUM;                
//        settingsItems.items[key_num] = itemRunout[item_runout_i];
//        menuDrawItem(&settingsItems.items[key_num], key_num);
//        infoSettings.runout = item_runout[item_runout_i];
//        break;
//      #endif
//      case KEY_ICON_7:
//        infoMenu.cur--;
//        break;
//      
//      default:
//        break;
//    }
//    loopProcess();		
//  }

//  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
//  {
//    storePara();
//  }
//}
