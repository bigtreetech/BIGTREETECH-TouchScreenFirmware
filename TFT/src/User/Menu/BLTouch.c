#include "BLTouch.h"
#include "includes.h"

const char * const SERVO_GCODE = "M280 P0 S%d\n";

const MENUITEMS BLTouchItems = {
  // title
  LABEL_BLTOUCH,
  // icon                          label
  {
    {ICON_BLTOUCH_RESET,           LABEL_RESET},
    {ICON_BLTOUCH_TEST,            LABEL_TEST},
    {ICON_BLTOUCH_DEPLOY,          LABEL_DEPLOY},
    {ICON_BLTOUCH_STOW,            LABEL_STOW},
    {ICON_BLTOUCH_REPEAT,          LABEL_REPEAT},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuBLTouch(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&BLTouchItems);

  while (infoMenu.menu[infoMenu.cur] == menuBLTouch)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        storeCmd(SERVO_GCODE, 160);
        break;

      case KEY_ICON_1:
        storeCmd(SERVO_GCODE, 120);
        break;

      case KEY_ICON_2:
        storeCmd(SERVO_GCODE, 10);
        break;

      case KEY_ICON_3:
        storeCmd(SERVO_GCODE, 90);
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
