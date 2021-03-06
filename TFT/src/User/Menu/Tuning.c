#include "Tuning.h"
#include "includes.h"

const MENUITEMS TuningItems = {
  // title
  LABEL_TUNING,
  // icon                          label
  {
    {ICON_PID,                     LABEL_PID},
    {ICON_TUNE_EXTRUDER,           LABEL_TUNE_EXTRUDER},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuTuning(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&TuningItems);

  while (infoMenu.menu[infoMenu.cur] == menuTuning)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuPid;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuTuneExtruder;
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
