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
<<<<<<< HEAD
}

void cncMenu(void){
  //1 title, ITEM_PER_PAGE items(icon+label)
  MENUITEMS cncPageItems = {
  // title
  LABEL_MAINMENU,
  // icon               label
  {{ICON_HOME,          LABEL_HOME},
   {ICON_MOVE,          LABEL_MOVE},
   {ICON_GCODE,         LABEL_TERMINAL},
   {ICON_STOP,          LABEL_EMERGENCYSTOP},
   {ICON_LASER,         LABEL_LASER},
   {ICON_SPINDLE,       LABEL_SPINDLE},
   {ICON_SETTINGS,      LABEL_SETTINGS},
   {ICON_SPINDLE,       LABEL_CUT}}
  };

  cncPageItems.items[4].icon = (infoSettings.laser_mode == 1) ? ICON_LASER : ICON_FAN;
  cncPageItems.items[4].label.index = (infoSettings.laser_mode == 1) ? LABEL_LASER : LABEL_FAN;

  KEY_VALUES key_num = KEY_IDLE;
  GUI_SetBkColor(infoSettings.bg_color);

  menuDrawPage(&cncPageItems);

  while(infoMenu.menu[infoMenu.cur] == cncMenu)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHome;     break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove;     break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuSendGcode;       break;
      case KEY_ICON_3: storeCmd("M112\n"); break;     // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
                                                      // it may need to wait for a space to open up in the command queue.
                                                      // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuFan;      break;
      case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuSpindle;          break;
      case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings;        break;
      case KEY_ICON_7: infoMenu.menu[++infoMenu.cur] = menuPrint;     break;
      default:break;
      // Uh Oh...case KEY_ICON_7: infoMenu.cur--;        break;
      // This would be nice too...case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuCustom;          break;
    }
    loopProcess();
    }
}

void classicMenu(void)
{
  //1 title, ITEM_PER_PAGE items(icon+label)
  MENUITEMS classicPageItems = {
  // title
  LABEL_READY,
  // icon             label
     {{ICON_HEAT,     LABEL_PREHEAT},
      {ICON_MOVE,     LABEL_MOVE},
      {ICON_HOME,     LABEL_HOME},
      {ICON_PRINT,    LABEL_PRINT},
      {ICON_EXTRUDE,  LABEL_EXTRUDE},
      {ICON_FAN,      LABEL_FAN},
      {ICON_SETTINGS, LABEL_SETTINGS},
      {ICON_LEVELING, LABEL_LEVELING}}
=======
>>>>>>> btt_main/master
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    mainPageItems.items[5].label.index = LABEL_MACROS;
  }

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
        if (infoMachineSettings.firmwareType == FW_REPRAPFW)
        {
          strcpy(infoFile.path, "Macros");
          OPEN_MENU(menuCallMacro);
        }
        else
        {
          OPEN_MENU(menuCustom);
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
