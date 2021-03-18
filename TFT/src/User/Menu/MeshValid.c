#include "MeshValid.h"
#include "includes.h"


void menuMeshValid(void)
{
  MENUITEMS meshValidItems = {
    // title
    LABEL_MESH_VALID,
    // icon                          label
    {
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_PREHEAT,                 LABEL_BACKGROUND},
      {ICON_BABYSTEP,                LABEL_BABYSTEP},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES  key_num;

  menuDrawPage(&meshValidItems);
  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshPreheatIcon(i, i, &meshValidItems.items[i]);
  }

  while (infoMenu.menu[infoMenu.cur] == menuMeshValid)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
        //MESHVALID PLA
        case KEY_ICON_0:
        //MESHVALID PETG
        case KEY_ICON_1:
        //MESHVALID ABS
        case KEY_ICON_2:
        //MESHVALID WOOD
        case KEY_ICON_3:
        //MESHVALID TPU
        case KEY_ICON_4:
        //MESHVALID NYLON
        case KEY_ICON_5:
        storeCmd("G26 H%u B%u R99\n", infoSettings.preheat_temp[key_num], infoSettings.preheat_bed[key_num]);
        break;

        //Menu babystep 
        case KEY_ICON_6:
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