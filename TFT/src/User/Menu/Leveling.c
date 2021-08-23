#include "Leveling.h"
#include "includes.h"

const MENUITEMS manualLevelingItems = {
  // title
  LABEL_LEVELING,
  // icon                          label
  {
    {ICON_POINT_4,                 LABEL_POINT_4},
    {ICON_POINT_3,                 LABEL_POINT_3},
    {ICON_LEVEL_EDGE_DISTANCE,     LABEL_DISTANCE},
    {ICON_DISABLE_STEPPERS,        LABEL_XY_UNLOCK},
    {ICON_POINT_1,                 LABEL_POINT_1},
    {ICON_POINT_2,                 LABEL_POINT_2},
    {ICON_POINT_5,                 LABEL_POINT_5},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuManualLeveling(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&manualLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuManualLeveling)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        levelingMoveToPoint(LEVEL_TOP_LEFT);
        break;

      case KEY_ICON_1:
        levelingMoveToPoint(LEVEL_TOP_RIGHT);
        break;

      case KEY_ICON_2:
      {
        infoSettings.level_edge = editIntValue(LEVELING_EDGE_DISTANCE_MIN, LEVELING_EDGE_DISTANCE_MAX, LEVELING_EDGE_DISTANCE_DEFAULT, infoSettings.level_edge);

        menuDrawPage(&manualLevelingItems);
        break;
      }

      case KEY_ICON_3:
        storeCmd("M84 X Y E\n");
        break;

      case KEY_ICON_4:
        levelingMoveToPoint(LEVEL_BOTTOM_LEFT);
        break;

      case KEY_ICON_5:
        levelingMoveToPoint(LEVEL_BOTTOM_RIGHT);
        break;

      case KEY_ICON_6:
        levelingMoveToPoint(LEVEL_CENTER);
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
