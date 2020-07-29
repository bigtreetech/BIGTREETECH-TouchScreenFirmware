#include "Spindle.h"
#include "includes.h"

//1��title(����), ITEM_PER_PAGE��item(ͼ��+��ǩ)
const MENUITEMS spindleItems = {
//   title
LABEL_SPINDLE,
// icon                       label
 {{ICON_EEPROM_SAVE,          LABEL_ON},
  {ICON_STOP,                 LABEL_OFF},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuSpindle(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&spindleItems);
  while(infoMenu.menu[infoMenu.cur] == menuSpindle)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: storeCmd("M03\n");   break;
      case KEY_ICON_1: storeCmd("M05\n"); break;
      // case KEY_ICON_4: storeCmd("G92 X0\n"); break;
      // case KEY_ICON_5: storeCmd("G92 Y0\n"); break;
      // case KEY_ICON_6: storeCmd("G92 Z19.75\n"); break;
      case KEY_ICON_7: infoMenu.cur--;      break;
      default:break;
    }
    loopProcess();
  }
}

