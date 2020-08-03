#include "leveling.h"
#include "includes.h"

const MENUITEMS BLTouchItems = {
// title
LABEL_BLTOUCH,
// icon                        label
 {{ICON_BLTOUCH_RESET,         LABEL_RESET},
  {ICON_BLTOUCH_TEST,          LABEL_BLTOUCH_TEST},
  {ICON_BLTOUCH_DEPLOY,        LABEL_BLTOUCH_DEPLOY},
  {ICON_BLTOUCH_STOW,          LABEL_BLTOUCH_STOW},
  {ICON_BLTOUCH_REPEAT,        LABEL_BLTOUCH_REPEAT},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACK,                  LABEL_BACK},}
};

void menuBLTouch(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&BLTouchItems);
  while(infoMenu.menu[infoMenu.cur] == menuBLTouch)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        storeCmd("M280 P0 S160\n");
        break;
      case KEY_ICON_1:
        storeCmd("M280 P0 S120\n");
        break;
      case KEY_ICON_2:
        storeCmd("M280 P0 S10\n");
        break;
      case KEY_ICON_3:
        storeCmd("M280 P0 S90\n");
        break;
      case KEY_ICON_4:
        storeCmd("G28\n");
        storeCmd("M48\n");
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
