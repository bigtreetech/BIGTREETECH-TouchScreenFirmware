#include "UnifiedMove.h"
#include "includes.h"

void menuUnifiedMove(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS UnifiedMoveItems = {
    // title
    LABEL_UNIFIEDMOVE,
    // icon                         label
    {{ICON_HOME,                    LABEL_HOME},
     {ICON_MOVE,                    LABEL_MOVE},
     {ICON_MANUAL_LEVEL,            LABEL_LEVELING},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BABYSTEP,                LABEL_BABYSTEP},
     {ICON_DISABLE_STEPPERS,        LABEL_DISABLE_STEPPERS},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK}}
  };

  KEY_VALUES key_num = KEY_IDLE;
  bool blAvailable = false;

  if (infoMachineSettings.blType != BL_UNKNOWN)
  {
    blAvailable = true;

    UnifiedMoveItems.items[3].icon = ICON_LEVELING;
    UnifiedMoveItems.items[3].label.index = LABEL_BED_LEVELING;
  }

  menuDrawPage(&UnifiedMoveItems);

  while(infoMenu.menu[infoMenu.cur] == menuUnifiedMove)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuHome;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuMove;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
        break;

      case KEY_ICON_3:
        if (blAvailable)
          infoMenu.menu[++infoMenu.cur] = menuBedLeveling;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuBabystep;
        break;

      case KEY_ICON_5:
        storeCmd("M84\n");
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
