#include "leveling.h"
#include "includes.h"

const MENUITEMS levelingItems = {
// title
LABEL_LEVELING,
// icon                       label
  {{ICON_POINT_1,             LABEL_POINT_1},
  {ICON_POINT_2,              LABEL_POINT_2},
  {ICON_POINT_3,              LABEL_POINT_3},
  {ICON_POINT_4,              LABEL_POINT_4},
  #ifdef AUTO_BED_LEVELING
    {ICON_LEVELING,             LABEL_ABL},
  #else
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
  #endif
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

void moveToLevelingPoint(u8 point)
{
  static const s16 pointPosition[][2] = {
    {LEVELING_POINT_1_X, LEVELING_POINT_1_Y},
    {LEVELING_POINT_2_X, LEVELING_POINT_2_Y},
    {LEVELING_POINT_3_X, LEVELING_POINT_3_Y},
    {LEVELING_POINT_4_X, LEVELING_POINT_4_Y},
  };
  if(coordinateIsClear() == false)
  {
    storeCmd("G28\n");
  }
  storeCmd("G0 Z%.3f F%d\n", LEVELING_POINT_MOVE_Z, LEVELING_POINT_Z_FEEDRATE);
  storeCmd("G0 X%d Y%d F%d\n", pointPosition[point][0], pointPosition[point][1], LEVELING_POINT_XY_FEEDRATE);
  storeCmd("G0 Z%.3f F%d\n", LEVELING_POINT_Z, LEVELING_POINT_Z_FEEDRATE);
}

void menuLeveling(void)
{
  KEY_VALUES key_num=KEY_IDLE;

  menuDrawPage(&levelingItems);

  while(infoMenu.menu[infoMenu.cur] == menuLeveling)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: moveToLevelingPoint(0); break;
      case KEY_ICON_1: moveToLevelingPoint(1); break;
      case KEY_ICON_2: moveToLevelingPoint(2); break;
      case KEY_ICON_3: moveToLevelingPoint(3); break;
      
      #ifdef AUTO_BED_LEVELING
      case KEY_ICON_4:
        storeCmd("G28\n");
        storeCmd("G29\n");
        break;
      #endif
      
      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
