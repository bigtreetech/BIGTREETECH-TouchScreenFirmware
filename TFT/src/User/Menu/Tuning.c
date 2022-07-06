#include "Tuning.h"
#include "includes.h"

void menuTuning(void)
{
  MENUITEMS TuningItems = {
    // title
    LABEL_TUNING,
    // icon                          label
    {
      {ICON_MPC_PID,                 LABEL_MPC},
      {ICON_MPC_PID,                 LABEL_PID},
      {ICON_TUNE_EXTRUDER,           LABEL_TUNE_EXTRUDER},
      #if DELTA_PROBE_TYPE == 0  // if not Delta printer
        {ICON_PROBE_OFFSET,          LABEL_H_OFFSET},
      #else
        {ICON_NULL,                  LABEL_NULL},
      #endif
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!hasMPC())
    for (uint8_t i = 0; i < 4; i++)
    {
      TuningItems.items[i] = TuningItems.items[i + 1];
    }
  else if (!infoSettings.bed_en)
    for (uint8_t i = 1; i < 4; i++)
    {
      TuningItems.items[i] = TuningItems.items[i + 1];
    }
  
  menuDrawPage(&TuningItems);

  while (MENU_IS(menuTuning))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (hasMPC())
          OPEN_MENU(menuMPC);
        else
          OPEN_MENU(menuPid);

        break;

      case KEY_ICON_1:
        if (hasMPC() && infoSettings.bed_en)
          OPEN_MENU(menuPid);
        else
          OPEN_MENU(menuTuneExtruder);

        break;

      case KEY_ICON_2:
        if (hasMPC() && infoSettings.bed_en)
          OPEN_MENU(menuTuneExtruder);
        #if DELTA_PROBE_TYPE == 0  // if not Delta printer
          else
          {
            storeCmd("M206\n");
            zOffsetSetMenu(false);  // use Home Offset menu
            OPEN_MENU(menuZOffset);
          }
        #endif

        break;

      case KEY_ICON_3:
        #if DELTA_PROBE_TYPE == 0  // if not Delta printer
          if (hasMPC() && infoSettings.bed_en)
          {
            storeCmd("M206\n");
            zOffsetSetMenu(false);  // use Home Offset menu
            OPEN_MENU(menuZOffset);
          }
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
