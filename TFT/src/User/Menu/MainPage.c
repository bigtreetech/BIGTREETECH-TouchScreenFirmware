#include "MainPage.h"
#include "includes.h"

//1 title, ITEM_PER_PAGE items(icon+label)
const MENUITEMS mainPageItems = {
// title
LABEL_READY,
// icon                       label
  #ifdef UNIFIED_MENU //if Unified menu is selected
   {{ICON_HEAT_FAN,             lABEL_UNIFIEDHEAT},
    {ICON_HOME_MOVE,            LABEL_UNIFIEDMOVE},
    {ICON_EXTRUDE,              LABEL_EXTRUDE},
    {ICON_STOP,                 LABEL_EMERGENCYSTOP},
    {ICON_GCODE,                LABEL_GCODE},
    {ICON_CUSTOM,               LABEL_CUSTOM},
    {ICON_SETTINGS,             LABEL_SETTINGS},
    {ICON_BACK,                 LABEL_BACK}}
  #else
   {{ICON_HEAT,                 LABEL_PREHEAT},
    {ICON_MOVE,                 LABEL_MOVE},
    {ICON_HOME,                 LABEL_HOME},
    {ICON_LEVELING,             LABEL_LEVELING},
    {ICON_EXTRUDE,              LABEL_EXTRUDE},
    {ICON_SETTINGS,             LABEL_SETTINGS},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK}}
  #endif
};

void menuMain(void)
{
  KEY_VALUES key_num=KEY_IDLE;
  GUI_SetBkColor(ST7920_BKCOLOR);
  menuDrawPage(&mainPageItems);

  while(infoMenu.menu[infoMenu.cur] == menuMain)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      #ifdef UNIFIED_MENU //if Unified menu is selected
        case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;     break;
        case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuUnifiedMove;     break;
        case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuExtrude;         break;
        case KEY_ICON_3: { storeCmd("M112\n");} break;  // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
                                                        // it may need to wait for a space to open up in the command queue.
                                                        // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
        case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuSendGcode;       break;
        case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuCustom ;       break;
        case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;        break;
        case KEY_ICON_7: infoMenu.cur--;        break;
        default:break;
      #else
        case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuPreheat;   break;
        case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove;      break;
        case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuHome;      break;
        case KEY_ICON_3:
          #ifdef AUTO_BED_LEVELING
            infoMenu.menu[++infoMenu.cur] = menuAutoLeveling;
          #else
            infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
          #endif
          break;
        case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuExtrude;   break;
        case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuSettings;  break;
        case KEY_ICON_7: infoMenu.cur--;        break;
        default:break;
      #endif
    }
    loopProcess();
  }
}
