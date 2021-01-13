#include "includes.h"
#include "coordinate.h"

const MENUITEMS touchmiItems = {
// title
LABEL_TOUCHMI,
// icon                         label
 {{ICON_RESET_VALUE,            LABEL_INIT},
  {ICON_NOZZLE_UP,              LABEL_ZOFFSETPOS},
  {ICON_EEPROM_SAVE,            LABEL_TM_SAVE},
  {ICON_PREHEAT_BOTH,           LABEL_TEST},
  {ICON_BACKGROUND,             LABEL_BACKGROUND},
  {ICON_NOZZLE_DOWN,            LABEL_ZOFFSETNEG},
  {ICON_BACKGROUND,          	  LABEL_BACKGROUND},
  {ICON_BACK,                   LABEL_BACK},}
};

void menuTouchMi(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&touchmiItems);
  while(infoMenu.menu[infoMenu.cur] == menuTouchMi)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        storeCmd("M851 Z0\n");
        storeCmd("G28\n");
        //storeCmd("M500\n");
        storeCmd("G1 Z0 F200\n");
        storeCmd("M211 S0\n");
        break;
      case KEY_ICON_1:
        storeCmd("M290 Z0.05\n");
        break;
      case KEY_ICON_2:
        storeCmd("M211 S1\n");
        //storeCmd("M500\n");
        storeCmd("G28 X Y\n");
        storeCmd("M500\n");        
        break;
      case KEY_ICON_3:
        storeCmd("G28\n");
        storeCmd("G1 Z0\n");
        break;
      case KEY_ICON_4:
        storeCmd("G28\n");
        break;
      case KEY_ICON_5:
        storeCmd("M290 Z-0.05\n");
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
