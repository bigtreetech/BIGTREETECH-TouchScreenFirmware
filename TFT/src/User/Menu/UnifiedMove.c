#include "UnifiedMove.h"
#include "includes.h"

#if DELTA_PROBE_TYPE != 0  // if Delta printer
  void deltaCalibration(void)
  {
    mustStoreCmd("G33\n");
  }
#endif

void menuUnifiedMove(void)
{
  MENUITEMS UnifiedMoveItems = {
    // title
    LABEL_UNIFIEDMOVE,
    // icon                          label
    {
      {ICON_HOME,                    LABEL_HOME},
      {ICON_MOVE,                    LABEL_MOVE},
      {ICON_EXTRUDE,                 LABEL_EXTRUDE},
      {ICON_DISABLE_STEPPERS,        LABEL_DISABLE_STEPPERS},
      {ICON_BABYSTEP,                LABEL_BABYSTEP},
      #if DELTA_PROBE_TYPE == 0  // if not Delta printer
        {ICON_MANUAL_LEVEL,            LABEL_LEVELING},
      #else
        {ICON_CALIBRATION,             LABEL_CALIBRATION},
      #endif
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoMachineSettings.leveling != BL_DISABLED)
  {
    UnifiedMoveItems.items[6].icon = ICON_LEVELING;
    UnifiedMoveItems.items[6].label.index = LABEL_BED_LEVELING;
  }

  menuDrawPage(&UnifiedMoveItems);

  while (MENU_IS(menuUnifiedMove))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuHome);
        break;

      case KEY_ICON_1:
        OPEN_MENU(menuMove);
        break;

      case KEY_ICON_2:
        OPEN_MENU(menuExtrude);
        break;

      case KEY_ICON_3:
        storeCmd("M84\n");
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuBabystep);
        break;

      case KEY_ICON_5:
        #if DELTA_PROBE_TYPE == 0  // if not Delta printer
          OPEN_MENU(menuManualLeveling);
        #else
          #if DELTA_PROBE_TYPE != 2  // if not removable probe
            deltaCalibration();
          #else  // if removable probe
            setDialogText(LABEL_WARNING, LABEL_CONNECT_PROBE, LABEL_CONTINUE, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, deltaCalibration, NULL, NULL);
          #endif
        #endif
        break;

      case KEY_ICON_6:
        if (infoMachineSettings.leveling != BL_DISABLED)
          OPEN_MENU(menuBedLeveling);
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
