#include "MachineSettings.h"
#include "includes.h"

MENUITEMS customItems = {
// title
LABEL_CUSTOM,
// icon                       label
 {
   #ifdef CUSTOM_0_GCODE
    {ICON_CUSTOM_0,             LABEL_CUSTOM_0},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
   #ifdef CUSTOM_1_GCODE
    {ICON_CUSTOM_1,             LABEL_CUSTOM_1},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
   #ifdef CUSTOM_2_GCODE
    {ICON_CUSTOM_2,             LABEL_CUSTOM_2},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
   #ifdef CUSTOM_3_GCODE
    {ICON_CUSTOM_3,             LABEL_CUSTOM_3},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
   #ifdef CUSTOM_4_GCODE
    {ICON_CUSTOM_4,             LABEL_CUSTOM_4},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
   #ifdef CUSTOM_5_GCODE
    {ICON_CUSTOM_5,             LABEL_CUSTOM_5},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
   #ifdef CUSTOM_6_GCODE
    {ICON_CUSTOM_6,             LABEL_CUSTOM_6},
   #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
   #endif
  {ICON_BACK,                 LABEL_BACK},}
};

void menuCustom(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&customItems,false);

  while(infoMenu.menu[infoMenu.cur] == menuCustom)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
     #ifdef CUSTOM_0_GCODE
      case KEY_ICON_0:
        storeCmd(CUSTOM_0_GCODE);
        break;
     #endif
     #ifdef CUSTOM_1_GCODE
      case KEY_ICON_1:
        storeCmd(CUSTOM_1_GCODE);
        break;
     #endif
     #ifdef CUSTOM_2_GCODE
      case KEY_ICON_2:
        storeCmd(CUSTOM_2_GCODE);
        break;
     #endif
     #ifdef CUSTOM_3_GCODE
      case KEY_ICON_3:
        storeCmd(CUSTOM_3_GCODE);
        break;
     #endif
     #ifdef CUSTOM_4_GCODE
      case KEY_ICON_4:
        storeCmd(CUSTOM_4_GCODE);
        break;
     #endif
     #ifdef CUSTOM_5_GCODE
      case KEY_ICON_5:
        storeCmd(CUSTOM_5_GCODE);
        break;
     #endif
     #ifdef CUSTOM_6_GCODE
      case KEY_ICON_6:
        storeCmd(CUSTOM_6_GCODE);
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
}

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

  menuDrawPage(&RGBItems,false);

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
  
  menuDrawPage(&machineSettingsItems,false);
  
  while(infoMenu.menu[infoMenu.cur] == menuMachineSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] =  menuCustom;
        break;
      
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
