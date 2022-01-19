//TG MODIFIED BY T.GIOIOSA
#include "Tuning.h"
#include "includes.h"

const MENUITEMS TuningItems = {
  // title
  LABEL_TUNING,
  // icon                          label
  {
    {ICON_REMOVED,                 LABEL_REMOVED},          //TG 8/22/21 removed for CNC, was LABEL_PID
    {ICON_BACKGROUND,              LABEL_BACKGROUND},   //TG 2/10/21 removed for CNC, was TUNE_EXTRUDER
    {ICON_PROBE_OFFSET,            LABEL_H_OFFSET},
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
      //case KEY_ICON_0:                                      //TG 2/14/21 removed Pid.c for CNC
      //  infoMenu.menu[++infoMenu.cur] = menuPid;
      //  break;

      //case KEY_ICON_1:
      //  infoMenu.menu[++infoMenu.cur] = menuTuneExtruder;   //TG 2/10/21 removed for CNC, was for TUNE_EXTRUDER
      //  break;

      case KEY_ICON_2:
        storeCmd("M206\n");
        zOffsetSetMenu(false);  // use Home Offset menu
        infoMenu.menu[++infoMenu.cur] = menuZOffset;
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
