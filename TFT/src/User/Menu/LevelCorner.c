#include "LevelCorner.h"
#include "includes.h"

const uint8_t valIconIndex[4] = {4,5,1,0};

//Show text on icons
void showTextOnIcon(uint8_t index, const ITEM * menuitem, char * str)
{
  if (str == NULL)
  {
    menuDrawIconOnly(menuitem, index);
    return;
  }

  GUI_POINT loc;
  LIVE_INFO lvIcon;

  loc.x = 5;
  loc.y = ICON_HEIGHT - 5;

  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = false;

  lvIcon.lines[0].h_align = LEFT;
  lvIcon.lines[0].v_align = BOTTOM;
  lvIcon.lines[0].fn_color = ORANGE;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = loc;
  lvIcon.lines[0].large_font = false;

  lvIcon.lines[1].h_align = LEFT;
  lvIcon.lines[1].v_align = BOTTOM;
  lvIcon.lines[1].fn_color = WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = (GUI_POINT){loc.x - 2, loc.y - 2};
  lvIcon.lines[1].large_font = false;

  lvIcon.lines[0].text = (uint8_t *)str;
  lvIcon.lines[1].text = (uint8_t *)str;

  showLiveInfo(index, &lvIcon, menuitem);
}

void ScanLevelCorner(uint8_t pointer)
{
  uint16_t pointPosition[4][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
  };

  if (infoSettings.touchmi_sensor != 0)
  {
    mustStoreCmd("M401\n");
    mustStoreCmd("G30 E0 X%d Y%d\n", pointPosition[pointer][0], pointPosition[pointer][1]);
    mustStoreCmd("G1 Z10\n");
  }
  else
  {
    mustStoreCmd("G30 E1 X%d Y%d\n", pointPosition[pointer][0], pointPosition[pointer][1]);
  }

  mustStoreCmd("M17 X Y Z\n");
  mustStoreCmd("M18 S0 X Y Z\n");
}

void refreshLevelCornerValue(MENUITEMS * levelItems)
{
  if ((int)GetLevelCornerPosition(0) != 0)
  {
    if (((int)GetLevelCornerPosition(0) >= 1) && ((int)GetLevelCornerPosition(0) <= 4))
    {
      uint8_t valPos = (uint8_t)GetLevelCornerPosition(0);
      uint8_t valPosSub = (uint8_t)GetLevelCornerPosition(0)-1;

      sprintf((char *)levelItems->items[valIconIndex[valPosSub]].label.address, "%1.4f", GetLevelCornerPosition(valPos));
      menuDrawIconText(&levelItems->items[valIconIndex[valPosSub]], valIconIndex[valPosSub]);
      SetLevelCornerPosition(0, 0);
    }
  }
}

void refreshProbeAccuracy(MENUITEMS * levelItems)
{
  char tempstr[8];

  if ((int)GetLevelCornerPosition(0) == 5)
  {
    uint8_t valPos = (uint8_t)GetLevelCornerPosition(0);
    uint8_t valPosSub = (uint8_t)GetLevelCornerPosition(0)-1;

    sprintf(tempstr, "%1.4f", GetLevelCornerPosition(5));
    sprintf((char *)levelItems->items[valIconIndex[valPosSub]].label.address, "%1.4f", GetLevelCornerPosition(valPos));
    menuDrawIconText(&levelItems->items[valIconIndex[valPosSub]], valIconIndex[valPosSub]);

    showTextOnIcon(3, &levelItems->items[3], "M48");
    SetLevelCornerPosition(0, 0);
  }
}

void menuLevelCorner(void)
{
  MENUITEMS levelCornerItems = {
    // title
    LABEL_LEVEL_CORNER,
    // icon                      label
    {
      {ICON_POINT_4,             LABEL_DYNAMIC},
      {ICON_POINT_3,             LABEL_DYNAMIC},
      {ICON_LEVEL_EDGE_DISTANCE, LABEL_DISTANCE},
      {ICON_BLTOUCH,             LABEL_DYNAMIC},
      {ICON_POINT_1,             LABEL_DYNAMIC},
      {ICON_POINT_2,             LABEL_DYNAMIC},
      {ICON_RESUME,              LABEL_START},
      {ICON_BACK,                LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  uint8_t ReadValuestored = 6;
  char iconText[5][15];

  levelCornerItems.items[0].label.address = (uint8_t *)iconText[0];
  levelCornerItems.items[1].label.address = (uint8_t *)iconText[1];
  levelCornerItems.items[3].label.address = (uint8_t *)iconText[2];
  levelCornerItems.items[4].label.address = (uint8_t *)iconText[3];
  levelCornerItems.items[5].label.address = (uint8_t *)iconText[4];

  menuDrawPage(&levelCornerItems);

  mustStoreCmd("M851\n");  // Init Probe Offset in parseAck to get probe offset X and Y
  mustStoreCmd("G28\n");  // Init Coordinate

  // Check min edge limit for the probe with probe offset set in parseACK.c
  uint8_t edge_min = MAX(ABS(getParameter((int16_t)P_PROBE_OFFSET, X_STEPPER)), ABS((int16_t)getParameter(P_PROBE_OFFSET, Y_STEPPER))) + 1;
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
        break;
      }

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
      refreshLevelCornerValue(&levelCornerItems);
      refreshProbeAccuracy(&levelCornerItems);
    }

    refreshProbeAccuracy(&levelCornerItems);
    refreshLevelCornerValue(&levelCornerItems);
    loopProcess();
  }
}
