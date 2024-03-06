#include "MeshValid.h"
#include "includes.h"

const MENUITEMS meshValidItems = {
  // title
  LABEL_MESH_VALID,
  // icon             label
  {
    {ICON_PREHEAT,    LABEL_NULL},
    {ICON_PREHEAT,    LABEL_NULL},
    {ICON_PREHEAT,    LABEL_NULL},
    {ICON_PREHEAT,    LABEL_NULL},
    {ICON_PREHEAT,    LABEL_NULL},
    {ICON_PREHEAT,    LABEL_NULL},
    {ICON_NULL,       LABEL_NULL},
    {ICON_BACK,       LABEL_BACK},
  }
};

void menuMeshValid(void)
{
  KEY_VALUES key_num;
  PREHEAT_STORE preheatStore;

  W25Qxx_ReadBuffer((uint8_t*)&preheatStore, PREHEAT_STORE_ADDR, sizeof(PREHEAT_STORE));
  menuDrawPage(&meshValidItems);

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshPreheatIcon(&preheatStore, i, false);
  }

  while (MENU_IS(menuMeshValid))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // MESHVALID PLA
      case KEY_ICON_0:
      // MESHVALID PETG
      case KEY_ICON_1:
      // MESHVALID ABS
      case KEY_ICON_2:
      // MESHVALID WOOD
      case KEY_ICON_3:
      // MESHVALID TPU
      case KEY_ICON_4:
      // MESHVALID NYLON
      case KEY_ICON_5:
        mustStoreCmd("G28\n");
        mustStoreCmd("G26 H%u B%u R99\n", preheatStore.preheat_hotend[key_num], preheatStore.preheat_bed[key_num]);
        mustStoreCmd("G1 Z10 F%d\n", infoSettings.level_feedrate[FEEDRATE_Z]);
        mustStoreCmd("G1 X0 F%d\n", infoSettings.level_feedrate[FEEDRATE_XY]);
        refreshPreheatIcon(&preheatStore, key_num, false);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
