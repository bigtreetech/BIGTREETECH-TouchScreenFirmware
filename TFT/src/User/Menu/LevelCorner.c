#include "LevelCorner.h"
#include "includes.h"

const MENUITEMS levelCornerItems = {
  // title
  LABEL_LEVEL_CORNER,
  // icon                      label
  {
    {ICON_POINT_4,             LABEL_BACKGROUND},
    {ICON_POINT_3,             LABEL_BACKGROUND},
    {ICON_LEVEL_EDGE_DISTANCE, LABEL_DISTANCE},
    {ICON_BLTOUCH,             LABEL_BACKGROUND},
    {ICON_POINT_1,             LABEL_BACKGROUND},
    {ICON_POINT_2,             LABEL_BACKGROUND},
    {ICON_RESUME,              LABEL_START},
    {ICON_BACK,                LABEL_BACK},
  }
};

LIVE_INFO lvIcon;

void ScanLevelCorner(u8 pointer)
{
  s16 pointPosition[4][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
  };

  if (infoSettings.touchmi_sensor != 0)
  {
    mustStoreCmd("M401\n");
    mustStoreCmd("G30 E0 X%d Y%d\n", (s16)pointPosition[pointer][0], (s16)pointPosition[pointer][1]);
    mustStoreCmd("G1 Z10\n");
  }
  else
  {
    mustStoreCmd("G30 E1 X%d Y%d\n", (s16)pointPosition[pointer][0], (s16)pointPosition[pointer][1]);
  }

  mustStoreCmd("M17 X Y Z\n");
  mustStoreCmd("M18 S0 X Y Z\n");
}

void refreshLevelCornerValue(MENUITEMS levelItems)
{
  char tempstr[10];
  int valIndex[4] = {4,5,1,0};
  int valPos;
  int valPosSub;
  LIVE_INFO lvIcon;

  if ((int)GetLevelCornerPosition(0) != 0)
  {
    lvIcon.lines[0].pos = ss_val_point;
    lvIcon.lines[1].pos = ss_val_point;
    lvIcon.lines[2].pos = ss_val_point;
    lvIcon.lines[3].pos = ss_val_point;

    if (((int)GetLevelCornerPosition(0) >= 1) && ((int)GetLevelCornerPosition(0) <= 4))
    {
      valPos = (int)GetLevelCornerPosition(0);
      valPosSub = (int)GetLevelCornerPosition(0)-1;
      sprintf(tempstr, "%1.4f", GetLevelCornerPosition(valPos));
      lvIcon.lines[valPosSub].text = (uint8_t *)tempstr;
      showLevelCornerLiveInfo(valIndex[valPosSub], valPosSub, &lvIcon, &levelItems.items[valIndex[valPosSub]]);
      SetLevelCornerPosition(0, 0);
    }
  }
}

void refreshProbeAccuracy(MENUITEMS levelItems)
{
  char tempstr[8];
  LIVE_INFO lvIcon;
  LIVE_INFO lvIconM48;

  if ((int)GetLevelCornerPosition(0) == 5)
  {
    lvIcon.lines[4].pos = ss_val_point;
    sprintf(tempstr, "%1.4f", GetLevelCornerPosition(5));
    lvIcon.lines[4].text = (uint8_t *)tempstr;
    showLevelCornerLiveInfo(3, 4, &lvIcon, &levelItems.items[3]);
    lvIconM48.lines[0].pos = ss_val_point;
    sprintf(tempstr, "%s", " M48    ");
    lvIconM48.lines[0].text = (uint8_t *)tempstr;
    showTextOnIcon(3, 0, &lvIconM48, &levelCornerItems.items[3]);
    SetLevelCornerPosition(0, 0);
  }
}

void menuLevelCorner(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  int ReadValuestored = 6;

  menuDrawPage(&levelCornerItems);

  // Init Probe Offset in parseAck to get probe offset X and Y
  mustStoreCmd("M851\n");

  // Init Coordinate
  mustStoreCmd("G28\n");

  // Check min edge limit for the probe with probe offset set in parseACK.c
  uint8_t edge_min = MAX(ABS(getParameter((s16)P_PROBE_OFFSET, X_STEPPER)),ABS((s16)getParameter(P_PROBE_OFFSET, Y_STEPPER))) + 1;
  if (infoSettings.level_edge < edge_min)
  {
    infoSettings.level_edge = ((LEVELING_EDGE_DISTANCE >= edge_min) ? LEVELING_EDGE_DISTANCE : edge_min);
  }

  while (infoMenu.menu[infoMenu.cur] == menuLevelCorner)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        ScanLevelCorner(3);
        break;

      case KEY_ICON_6:
        ScanLevelCorner(0);
        ScanLevelCorner(1);
        ScanLevelCorner(2);
        ScanLevelCorner(3);
        break;

      case KEY_ICON_1:
        ScanLevelCorner(2);
        break;

      case KEY_ICON_2:
        {
          infoSettings.level_edge = editIntValue(edge_min, LEVELING_EDGE_DISTANCE_MAX, LEVELING_EDGE_DISTANCE_DEFAULT, infoSettings.level_edge);
          menuDrawPage(&levelCornerItems);
          ReadValuestored = 6;
        }
        break;

      case KEY_ICON_4:
        ScanLevelCorner(0);
        break;

      case KEY_ICON_3:
        mustStoreCmd("M48\n");
        mustStoreCmd("M17 X Y Z\n");
        mustStoreCmd("M18 S0 X Y Z\n");
        break;

      case KEY_ICON_5:
        ScanLevelCorner(1);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    while (ReadValuestored != 0)
    {
      SetLevelCornerPosition(0, ReadValuestored--);
      refreshLevelCornerValue(levelCornerItems);
      refreshProbeAccuracy(levelCornerItems);
    }

    refreshProbeAccuracy(levelCornerItems);
    refreshLevelCornerValue(levelCornerItems);
    loopProcess();
  }
}
