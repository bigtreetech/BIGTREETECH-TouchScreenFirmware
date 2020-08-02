#include "Tuning.h"
#include "includes.h"

void menuTuning(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  const MENUITEMS TuningItems = {
    // title
    LABEL_TUNING,
    // icon                         label
    {{ICON_PID,                     LABEL_PID},
     {ICON_EXTRUDE_100,             LABEL_EXTRUDE_100},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

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
        storeCmd("G90\nG0 F3000 X0 Y0 Z100\n");            // present extruder
        storeCmd("M83\nG1 F50\nG1 E100\nM82\n");           // extrude
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
