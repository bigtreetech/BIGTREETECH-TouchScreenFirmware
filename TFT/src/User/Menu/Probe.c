#include "probe.h"
#include "includes.h"

const MENUITEMS probeItems = {
// title
LABEL_PROBE,
// icon                        label
 {{ICON_BLTOUCH_DEPLOY,        LABEL_BLTOUCH_DEPLOY},
  {ICON_BLTOUCH_STOW,          LABEL_BLTOUCH_STOW},
  {ICON_BLTOUCH_TEST,          LABEL_BLTOUCH_TEST},
  {ICON_BLTOUCH_REPEAT,        LABEL_BLTOUCH_REPEAT},
  {ICON_TOUCHMI,               LABEL_TOUCHMI_INIT},
  {ICON_TOUCHMI,               LABEL_TOUCHMI_ZTEST},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACK,                  LABEL_BACK},}
};

void menuProbe(void)
{
  KEY_VALUES key_num=KEY_IDLE;
  menuDrawPage(&probeItems);
  while(infoMenu.menu[infoMenu.cur] == menuProbe)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        storeCmd("M280 P0 S10\n");
        break;
      case KEY_ICON_1:
        storeCmd("M280 P0 S90\n");
        break;
      case KEY_ICON_2:
        storeCmd("M280 P0 S120\n");
        break;
      case KEY_ICON_3:
        storeCmd("M48\n");
        break;
      case KEY_ICON_4:
        storeCmd("M851 Z0\nG28\nG1 F200 Z0");
        break;
      case KEY_ICON_5:
        storeCmd("G28\nG1 F200 Z0");
        break;
      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
