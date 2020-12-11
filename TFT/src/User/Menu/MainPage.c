#include "MainPage.h"
#include "includes.h"

void menuMain(void)
{
  // 1 title, ITEM_PER_PAGE items(icon+label)
  MENUITEMS mainPageItems = {
    // title
    LABEL_MAINMENU,
    // icon                         label
    {{ICON_HEAT_FAN,                LABEL_UNIFIEDHEAT},
     {ICON_HOME_MOVE,               LABEL_UNIFIEDMOVE},
     {ICON_EXTRUDE,                 LABEL_EXTRUDE},
     {ICON_STOP,                    LABEL_EMERGENCYSTOP},
     {ICON_GCODE,                   LABEL_TERMINAL},
     {ICON_CUSTOM,                  LABEL_CUSTOM},
     {ICON_SETTINGS,                LABEL_SETTINGS},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

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
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuUnifiedMove;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuExtrude;
        break;

      case KEY_ICON_3:
        // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
        // it may need to wait for a space to open up in the command queue.
        // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
        Serial_Puts(SERIAL_PORT, "M112\n");
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuCustom;
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

