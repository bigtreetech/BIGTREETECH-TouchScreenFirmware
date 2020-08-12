#include "leveling.h"
#include "includes.h"

// 1 title, ITEM_PER_PAGE items (icon + label)
const MENUITEMS autoLevelingItems = {
  // title
  LABEL_ABL,
  // icon                         label
  {{ICON_LEVELING,                LABEL_START},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_Z_FADE,                  LABEL_ABL_Z},
   {ICON_PROBE_OFFSET,            LABEL_Z_OFFSET},
   {ICON_BABYSTEP,                LABEL_BABYSTEP},
   {ICON_BLTOUCH,                 LABEL_BLTOUCH},
   {ICON_BACK,                    LABEL_BACK},}
};

void menuAutoLeveling(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  bool leveled = false;

  menuDrawPage(&autoLevelingItems);

  while(infoMenu.menu[infoMenu.cur] == menuAutoLeveling)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        storeCmd("G28\n");
        storeCmd("G29\n");
        leveled = true;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuZFade;
        break;

      case KEY_ICON_4:
        storeCmd("M851\n");
        infoMenu.menu[++infoMenu.cur] = menuProbeOffset;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuBabyStep;
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuBLTouch;
        break;

      case KEY_ICON_7:
        if (leveled == true && infoMachineSettings.EEPROM == 1)
        {
          showDialog(DIALOG_TYPE_QUESTION, textSelect(autoLevelingItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                     textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        else
        {
          infoMenu.cur--;
        }
        break;

      default:
        break;
    }
    loopProcess();
  }
}

#define ITEM_MANUAL_LEVELING_SUBMENU_NUM 3

ITEM itemManualLevelingSubmenu[ITEM_MANUAL_LEVELING_SUBMENU_NUM] = {
  // icon                         label
  {ICON_XY_UNLOCK,                LABEL_XY_UNLOCK},
  {ICON_LEVEL_EDGE_DISTANCE,      LABEL_DISTANCE},
  {ICON_BABYSTEP,                 LABEL_BABYSTEP},
};

static u8 curSubmenu = 0;

// 1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS manualLevelingItems = {
  // title
  LABEL_LEVELING,
  // icon                         label
  {{ICON_POINT_1,                 LABEL_POINT_1},
   {ICON_POINT_2,                 LABEL_POINT_2},
   {ICON_POINT_3,                 LABEL_POINT_3},
   {ICON_POINT_4,                 LABEL_POINT_4},
   {ICON_POINT_5,                 LABEL_POINT_5},
   {ICON_PAGE_DOWN,               LABEL_NEXT},
   {ICON_XY_UNLOCK,               LABEL_XY_UNLOCK},
   {ICON_BACK,                    LABEL_BACK},}
};

void moveToLevelingPoint(u8 point)
{
  s16 pointPosition[5][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {(infoSettings.machine_size_min[X_AXIS] + infoSettings.machine_size_max[X_AXIS]) / 2, (infoSettings.machine_size_min[Y_AXIS] + infoSettings.machine_size_max[Y_AXIS]) / 2},
  };

  if(coordinateIsKnown() == false)
  {
    storeCmd("G28\n");
  }

  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[Z_AXIS]);
  storeCmd("G0 X%d Y%d F%d\n", pointPosition[point][0], pointPosition[point][1], infoSettings.level_feedrate[X_AXIS]);
  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_pos, infoSettings.level_feedrate[Z_AXIS]);
}

void menuManualLeveling(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  manualLevelingItems.items[KEY_ICON_6] = itemManualLevelingSubmenu[curSubmenu];

  menuDrawPage(&manualLevelingItems);

  while(infoMenu.menu[infoMenu.cur] == menuManualLeveling)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        moveToLevelingPoint(0);
        break;

      case KEY_ICON_1:
        moveToLevelingPoint(1);
        break;

      case KEY_ICON_2:
        moveToLevelingPoint(2);
        break;

      case KEY_ICON_3:
        moveToLevelingPoint(3);
        break;

      case KEY_ICON_4:
        moveToLevelingPoint(4);
        break;

      // change submenu
      case KEY_ICON_5:
        curSubmenu = (curSubmenu + 1) % ITEM_MANUAL_LEVELING_SUBMENU_NUM;

        manualLevelingItems.items[KEY_ICON_6] = itemManualLevelingSubmenu[curSubmenu];

        menuDrawItem(&manualLevelingItems.items[KEY_ICON_6], KEY_ICON_6);
        break;

      // handle submenu
      case KEY_ICON_6:
        switch (curSubmenu)
        {
          // unlock XY axis
          case 0:
            storeCmd("M84 X Y E\n");
            break;

          // load LevelingEdgeDistrance menu
          case 1:
            infoMenu.menu[++infoMenu.cur] = menuLevelingEdgeDistance;
            break;

          // load BabyStep menu
          case 2:
            infoMenu.menu[++infoMenu.cur] = menuBabyStep;
            break;
        }
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
