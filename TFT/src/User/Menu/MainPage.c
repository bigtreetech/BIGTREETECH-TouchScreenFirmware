#include "MainPage.h"
#include "includes.h"

void menuMain(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS mainPageItems = {
    // title
    LABEL_MAINMENU,
    // icon                          label
    {
      {ICON_HEAT_FAN,                LABEL_UNIFIEDHEAT},
      {ICON_HOME_MOVE,               LABEL_UNIFIEDMOVE},
      #ifdef LOAD_UNLOAD_M701_M702
        {ICON_EXTRUDE,                 LABEL_LOAD_UNLOAD_SHORT},
      #else
        {ICON_EXTRUDE,                 LABEL_EXTRUDE},
      #endif
      {ICON_STOP,                    LABEL_EMERGENCYSTOP},
      {ICON_GCODE,                   LABEL_TERMINAL},
      {ICON_CUSTOM,                  LABEL_CUSTOM},
      {ICON_SETTINGS,                LABEL_SETTINGS},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
    mainPageItems.items[5].label.index = LABEL_MACROS;

  if (infoSettings.status_screen != 1)
  {
    mainPageItems.items[7].icon = ICON_PRINT;
    mainPageItems.items[7].label.index = LABEL_PRINT;
  }

  menuDrawPage(&mainPageItems);

  while (MENU_IS(menuMain))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuUnifiedHeat);
        break;

      case KEY_ICON_1:
        OPEN_MENU(menuUnifiedMove);
        break;

      case KEY_ICON_2:
        #ifdef LOAD_UNLOAD_M701_M702
          OPEN_MENU(menuLoadUnload);
        #else
          OPEN_MENU(menuExtrude);
        #endif
        break;

      case KEY_ICON_3:
        // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
        // it may need to wait for a space to open up in the command queue.
        // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
        sendEmergencyCmd("M112\n");
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuTerminal);
        break;

      case KEY_ICON_5:
        if (infoMachineSettings.firmwareType != FW_REPRAPFW)
        {
          OPEN_MENU(menuCustom);
        }
        else
        {
          strcpy(infoFile.path, "Macros");
          OPEN_MENU(menuCallMacro);
        }
        break;

      case KEY_ICON_6:
        OPEN_MENU(menuSettings);
        break;

      case KEY_ICON_7:
        if (infoSettings.status_screen != 1)
          OPEN_MENU(menuPrint);
        else
          CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
