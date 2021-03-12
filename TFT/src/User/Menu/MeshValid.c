#include "MeshValid.h"
#include "includes.h"

const MENUITEMS MeshValidItems = {
  // title
  LABEL_TEST_LEVEL_VALID,
  // icon                          label
  {
    {ICON_TEST_LEVEL_PLA,          LABEL_TEST_LEVEL_PLA},
    {ICON_TEST_LEVEL_ABS,          LABEL_TEST_LEVEL_ABS},
    {ICON_TEST_LEVEL_petg,         LABEL_TEST_LEVEL_petg},
    {ICON_TEST_LEVEL_ASA,          LABEL_TEST_LEVEL_ASA},
    {ICON_BABYSTEP,                LABEL_BABYSTEP},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuMeshValid(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&MeshValidItems);

  while (infoMenu.menu[infoMenu.cur] == menuMeshValid)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      //MESHVALID PLA
      case KEY_ICON_0:
        storeCmd("G26 H200 B60 R99\n");
        break;

      //MESHVALID ABS
      case KEY_ICON_1:
        storeCmd("G26 H230 B90 R99\n");
        break;

      //MESHVALID petg
      case KEY_ICON_2:
        storeCmd("G26 H240 B70 R99\n");
        break;

      //MESHVALID ASA
      case KEY_ICON_3:
        storeCmd("G26 H240 B90 R99\n");
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuBabystep;
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
