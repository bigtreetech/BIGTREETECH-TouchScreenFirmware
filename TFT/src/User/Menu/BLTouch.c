#include "BLTouch.h"
#include "includes.h"

#define SERVO_GCODE "M280 P0 S%d\n"

static BLT_HS_MODE bltHSmode = HS_DISABLED;

void setHSmode(BLT_HS_MODE hsMode)
{
  bltHSmode = hsMode;
}

MENUITEMS BLTouchItems = {
  // title
  LABEL_BLTOUCH,
  // icon                          label
  {
    {ICON_BLTOUCH_RESET,           LABEL_RESET},
    {ICON_BLTOUCH_TEST,            LABEL_TEST},
    {ICON_BLTOUCH_DEPLOY,          LABEL_DEPLOY},
    {ICON_BLTOUCH_STOW,            LABEL_STOW},
    {ICON_BLTOUCH_REPEAT,          LABEL_REPEAT},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuBLTouch(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  uint8_t hsModeOld = HS_DISABLED;

  menuDrawPage(&BLTouchItems);

  while (MENU_IS(menuBLTouch))
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

      case KEY_ICON_5:
        if (infoMachineSettings.firmwareType == FW_MARLIN && bltHSmode != HS_DISABLED)
        {
          bltHSmode = HS_ON - bltHSmode;
          storeCmd("M401 S%u\n", bltHSmode);
        }
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (infoMachineSettings.firmwareType == FW_MARLIN && bltHSmode != hsModeOld)
    {
      hsModeOld = bltHSmode;
      BLTouchItems.items[5].icon = (bltHSmode == HS_ON) ? ICON_FAST_SPEED : ICON_SLOW_SPEED;
      BLTouchItems.items[5].label.index = (bltHSmode == HS_ON) ? LABEL_HS_ON : LABEL_HS_OFF;
      menuDrawItem(&BLTouchItems.items[5], 5);
    }

    loopProcess();
  }
}
