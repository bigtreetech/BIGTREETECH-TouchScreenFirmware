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
