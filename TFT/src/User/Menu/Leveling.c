#include "Leveling.h"
#include "includes.h"

void moveToLevelingPoint(u8 point)
{
  s16 pointPosition[5][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {(infoSettings.machine_size_min[X_AXIS] + infoSettings.machine_size_max[X_AXIS]) / 2, (infoSettings.machine_size_min[Y_AXIS] + infoSettings.machine_size_max[Y_AXIS]) / 2},
  };

  if (coordinateIsKnown() == false)
  {
    storeCmd("G28\n");
  }

  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[Z_AXIS]);
  storeCmd("G0 X%d Y%d F%d\n", pointPosition[point][0], pointPosition[point][1], infoSettings.level_feedrate[X_AXIS]);
  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_pos, infoSettings.level_feedrate[Z_AXIS]);
}

void menuManualLeveling(void)
{
  MENUITEMS manualLevelingItems = {
    // title
    LABEL_LEVELING,
    // icon                         label
    {{ICON_POINT_1,                 LABEL_POINT_1},
     {ICON_POINT_2,                 LABEL_POINT_2},
     {ICON_POINT_3,                 LABEL_POINT_3},
     {ICON_POINT_4,                 LABEL_POINT_4},
     {ICON_POINT_5,                 LABEL_POINT_5},
     {ICON_LEVEL_EDGE_DISTANCE,     LABEL_DISTANCE},
     {ICON_DISABLE_STEPPERS,        LABEL_XY_UNLOCK},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&manualLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuManualLeveling)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
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

      case KEY_ICON_5:
        {
          char tempstr[30];
          sprintf(tempstr, "%Min:%d | Max:%d", LEVELING_EDGE_DISTANCE_MIN, LEVELING_EDGE_DISTANCE_MAX);
          int val = numPadInt((u8 *)tempstr, infoSettings.level_edge, LEVELING_EDGE_DISTANCE_DEFAULT, false);
          infoSettings.level_edge = NOBEYOND(LEVELING_EDGE_DISTANCE_MIN, val, LEVELING_EDGE_DISTANCE_MAX);
          menuDrawPage(&manualLevelingItems);
        }
        break;

      case KEY_ICON_6:
        storeCmd("M84 X Y E\n");
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
