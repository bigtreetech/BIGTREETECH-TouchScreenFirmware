//TG MODIFIED BY T.GIOIOSA
#include "MainPage.h"
#include "includes.h"

void menuMain(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS mainPageItems = {
    // title
    LABEL_MAINMENU,
    // icon                          label
    {{ICON_SPINDLE,                 LABEL_SPINDLE},       // was Heat/Fan
     {ICON_HOME_MOVE,               LABEL_UNIFIEDMOVE},
     {ICON_VACUUM,                  LABEL_VACUUM},        //TG 2/8/21 removed Extrude module
     {ICON_STOP,                    LABEL_EMERGENCYSTOP},
     {ICON_GCODE,                   LABEL_TERMINAL},
     {ICON_CUSTOM,                  LABEL_CUSTOM},
     {ICON_SETTINGS,                LABEL_SETTINGS},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoSettings.rrf_macros_enable)
  {
    mainPageItems.items[5].label.index = LABEL_MACROS;
  }

  if (infoSettings.status_screen != 1)
  {
    mainPageItems.items[7].icon = ICON_PRINT;
    mainPageItems.items[7].label.index = LABEL_PRINT;
  }

  menuDrawPage(&mainPageItems);

  while (infoMenu.menu[infoMenu.cur] == menuMain)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:  infoMenu.menu[++infoMenu.cur] = menuSpindle;  break;
      case KEY_ICON_1:  infoMenu.menu[++infoMenu.cur] = menuUnifiedMove;  break;
      case KEY_ICON_2:  infoMenu.menu[++infoMenu.cur] = menuVacuum;  break;  //TG 2/8/21 removed Extrude module
      case KEY_ICON_3:
        // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
        // it may need to wait for a space to open up in the command queue.
        // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
        Serial_Puts(SERIAL_PORT, "M112\n");
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;

      case KEY_ICON_5:
        if (infoSettings.rrf_macros_enable)
        {
          infoFile.title[0] = 0;
          infoMenu.menu[++infoMenu.cur] = menuCallMacro;
        }
        else
        {
          infoMenu.menu[++infoMenu.cur] = menuCustom;
        }
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuSettings;
        break;

      case KEY_ICON_7:
        if (infoSettings.status_screen != 1)
          infoMenu.menu[++infoMenu.cur] = menuPrint;
        else
          infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}

void cncMenu(void){                               //TG 1/12/20  this entire function cncMenu(void) is new
  //1 title, ITEM_PER_PAGE items(icon+label)
  MENUITEMS cncPageItems = {
  // title
  LABEL_MAINMENU,
  // icon               label
  {{ICON_HOME,          LABEL_HOME},
   {ICON_MOVE,          LABEL_MOVE},
   {ICON_GCODE,         LABEL_TERMINAL},
   {ICON_STOP,          LABEL_EMERGENCYSTOP},
   {ICON_VACUUM,        LABEL_VACUUM},
   {ICON_SPINDLE,       LABEL_SPINDLE},
   {ICON_SETTINGS,      LABEL_SETTINGS},
   {ICON_BACK,          LABEL_BACK}}
  };

  cncPageItems.items[5].icon = (infoSettings.laser_mode == 1) ? ICON_LASER : ICON_SPINDLE;
  cncPageItems.items[5].label.index = (infoSettings.laser_mode == 1) ? LABEL_LASER : LABEL_SPINDLE;
  if (infoSettings.status_screen != 1) {
    cncPageItems.items[7].icon =  ICON_PRINT;
    cncPageItems.items[7].label.index =  LABEL_PRINT;  
  }

  KEY_VALUES key_num = KEY_IDLE;
  GUI_SetBkColor(infoSettings.bg_color);

  menuDrawPage(&cncPageItems);

  while(infoMenu.menu[infoMenu.cur] == cncMenu)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHome;        break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove;        break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuTerminal;   break;
      case KEY_ICON_3: storeCmd("M112\n"); break;     // Emergency Stop : Used for emergency stopping, a reset is required to return to operational m
                                                      // it may need to wait for a space to open up in the command queue.
                                                      // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuVacuum;      break;
      case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = infoSettings.laser_mode == 1 ? menuLaser : menuSpindle;  break;
      case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;    break;
      case KEY_ICON_7: 
        if (infoSettings.status_screen != 1)
          infoMenu.menu[++infoMenu.cur] = menuPrint;
        else
          infoMenu.cur--;
      
      default:break;
      // Uh Oh...case KEY_ICON_7: infoMenu.cur--;        break;
      // This would be nice too...case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuCustom;          break;
    }
    loopProcess();
    }
}
