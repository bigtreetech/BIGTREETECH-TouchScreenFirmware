#include "Home.h"
#include "includes.h"

const MENUITEMS homeItems = {
  // title
  LABEL_HOME,
  // icon                          label
  {
    {ICON_HOME,                    LABEL_HOME},
    {ICON_X_HOME,                  LABEL_X},
    {ICON_Y_HOME,                  LABEL_Y},
    {ICON_Z_HOME,                  LABEL_Z},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

const MENUITEMS cncHomeItems = {
//   title
LABEL_HOME,
// icon                       label
 {{ICON_HOME,                 LABEL_XY},
  {ICON_Z_HOME,               LABEL_Z},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_ZERO_X,               LABEL_ZERO_X},
  {ICON_ZERO_Y,               LABEL_ZERO_Y},
  {ICON_ZERO_Z,               LABEL_ZERO_Z},
  {ICON_BACK,                 LABEL_BACK},}
};

void menuHome(void)
{
  KEY_VALUES key_num = KEY_IDLE;
<<<<<<< HEAD
  if (infoSettings.cnc_mode == 1)
  {
    menuDrawPage(&cncHomeItems);
  }
  else
  {
    menuDrawPage(&homeItems);
  }

  while(infoMenu.menu[infoMenu.cur] == menuHome)
  {
    key_num = menuKeyGetValue();
    if (infoSettings.cnc_mode == 1)
    {
      switch(key_num)
      {
        case KEY_ICON_0: storeCmd("G28 XY\n");   break;
        case KEY_ICON_1:
          storeCmd("G28 Z\n");
          if(infoSettings.touchplate_on == 1)
          {
            storeCmd("G92 Z%.3f\n", infoSettings.touchplate_height);
          }
          break;
        case KEY_ICON_4: storeCmd("G92 X0\n"); break;
        case KEY_ICON_5: storeCmd("G92 Y0\n"); break;
        case KEY_ICON_6: storeCmd("G92 Z0\n"); break;
        default:break;
        case KEY_ICON_7: infoMenu.cur--;      break;
      }
    }
    else
    {
      switch(key_num)
      {
        case KEY_ICON_0: storeCmd("G28\n");   break;
        case KEY_ICON_1: storeCmd("G28 X\n"); break;
        case KEY_ICON_2: storeCmd("G28 Y\n"); break;
        case KEY_ICON_3: storeCmd("G28 Z\n"); break;
        default:break;
        case KEY_ICON_7: infoMenu.cur--;      break;
      }
=======

  menuDrawPage(&homeItems);

  while (MENU_IS(menuHome))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0: storeCmd("G28\n");   break;
      case KEY_ICON_1: storeCmd("G28 X\n"); break;
      case KEY_ICON_2: storeCmd("G28 Y\n"); break;
      case KEY_ICON_3: storeCmd("G28 Z\n"); break;
      case KEY_ICON_7: CLOSE_MENU();      break;
      default: break;
>>>>>>> btt_main/master
    }

    loopProcess();
  }
}
