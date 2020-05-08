#include "leveling.h"
#include "includes.h"

const MENUITEMS autoLevelingItems = {
// title
LABEL_ABL,
// icon                        label
 {{ICON_LEVELING,              LABEL_ABL},
  {ICON_BLTOUCH_DEPLOY,        LABEL_BLTOUCH_DEPLOY},
  {ICON_BLTOUCH_STOW,          LABEL_BLTOUCH_STOW},
  {ICON_BLTOUCH_TEST,          LABEL_BLTOUCH_TEST},
  {ICON_BLTOUCH_REPEAT,        LABEL_BLTOUCH_REPEAT},
  {ICON_PROBE_OFFSET,          LABEL_Z_OFFSET},
  {ICON_BABYSTEP,              LABEL_BABYSTEP},
  {ICON_BACK,                  LABEL_BACK},}
};

void menuAutoLeveling(void)
{
  KEY_VALUES key_num=KEY_IDLE;
  menuDrawPage(&autoLevelingItems);
  while(infoMenu.menu[infoMenu.cur] == menuAutoLeveling)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        storeCmd("G28\n");
        storeCmd("G29\n");
        if(infoMachineSettings.EEPROM == 1){
           storeCmd("M500\n");
        }
        break;
      case KEY_ICON_1:
        storeCmd("M280 P0 S10\n");
        break;
      case KEY_ICON_2:
        storeCmd("M280 P0 S90\n");
        break;
      case KEY_ICON_3:
        storeCmd("M280 P0 S120\n");
        break;
      case KEY_ICON_4:
        storeCmd("G28\n");
        storeCmd("M48\n");
        break;
      case KEY_ICON_5:
        storeCmd("M851\n");
        infoMenu.menu[++infoMenu.cur] = menuProbeOffset;
        break;
      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuBabyStep;
        break;
      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}


const MENUITEMS manualLevelingItems = {
// title
LABEL_LEVELING,
// icon                        label
 {{ICON_POINT_1,               LABEL_POINT_1},
  {ICON_POINT_2,               LABEL_POINT_2},
  {ICON_POINT_3,               LABEL_POINT_3},
  {ICON_POINT_4,               LABEL_POINT_4},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACK,                  LABEL_BACK},}
};

void moveToLevelingPoint(u8 point)
{
  s16 pointPosition[4][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
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
  KEY_VALUES key_num=KEY_IDLE;
  menuDrawPage(&manualLevelingItems);
  while(infoMenu.menu[infoMenu.cur] == menuManualLeveling)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: moveToLevelingPoint(0); break;
      case KEY_ICON_1: moveToLevelingPoint(1); break;
      case KEY_ICON_2: moveToLevelingPoint(2); break;
      case KEY_ICON_3: moveToLevelingPoint(3); break;
      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
