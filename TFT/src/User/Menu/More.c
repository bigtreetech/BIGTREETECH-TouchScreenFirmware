#include "More.h"
#include "includes.h"

const MENUITEMS moreItems = {
  // title
  LABEL_MORE,
  // icon                          label
  {
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_FAN,                     LABEL_FAN},
    {ICON_EXTRUDE,                 LABEL_EXTRUDE},
    {ICON_PERCENTAGE,              LABEL_PERCENTAGE},
    {ICON_FEATURE_SETTINGS,        LABEL_FEATURE_SETTINGS},
    {ICON_MACHINE_SETTINGS,        LABEL_MACHINE_SETTINGS},
    #ifdef LOAD_UNLOAD_M701_M702
      {ICON_EXTRUDE,                 LABEL_LOAD_UNLOAD_SHORT},
    #else
      {ICON_GCODE,                   LABEL_TERMINAL},
    #endif
    {ICON_BACK,                    LABEL_BACK},
  }
};

void isPauseExtrude(void)
{
  if (printPause(true, PAUSE_NORMAL))
    REPLACE_MENU(menuExtrude);
}

void isPauseLoadUnload(void)
{
  if (printPause(true, PAUSE_NORMAL))
    REPLACE_MENU(menuLoadUnload);
}

void menuMore(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&moreItems);

  while (MENU_IS(menuMore))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuHeat);
        break;

      case KEY_ICON_1:
        OPEN_MENU(menuFan);
        break;

      case KEY_ICON_2:
        if (isPrinting() && !isPaused())  // need paused before extrude
        {
          popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL, isPauseExtrude, NULL, NULL);
        }
        else
        {
          OPEN_MENU(menuExtrude);
        }
        break;

      case KEY_ICON_3:
        OPEN_MENU(menuSpeed);
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuFeatureSettings);
        break;

      case KEY_ICON_5:
        OPEN_MENU(menuMachineSettings);
        break;

      case KEY_ICON_6:
        #ifdef LOAD_UNLOAD_M701_M702
          if (isPrinting() && !isPaused())  // need paused before extrude
          {
            popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL, isPauseLoadUnload, NULL, NULL);
          }
          else
          {
            OPEN_MENU(menuLoadUnload);
          }
        #else
          OPEN_MENU(menuTerminal);
        #endif
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
