#include "LevelCorner.h"
#include "includes.h"

#define LC_VALUE_COUNT 5

#define ENABLE_STEPPER_CMD "M17 X Y Z\n"
#define DISABLE_STEPPER_CMD "M18 S0 X Y Z\n"

// Buffer current z value measured in Level Corner = {position 1, position 2, position 3, position 4, probe accuracy(M48)}
static float levelCornerPosition[LC_VALUE_COUNT];
const uint8_t valIconIndex[LC_VALUE_COUNT] = {4, 5, 1, 0, 3};

// Set level corner position the measured Z offset from probe.
void SetLevelCornerPosition(uint8_t point, float position)
{
  levelCornerPosition[point] = position;
}

void ScanLevelCorner(uint8_t point)
{
  uint16_t pointPosition[4][2] = {
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge},
    {infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
    {infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge, infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge},
  };

  if (infoSettings.touchmi_sensor == 1)
    mustStoreCmd("M401\n");

  // move to selected point
  mustStoreCmd("G30 E0 X%d Y%d\n", pointPosition[point][0], pointPosition[point][1]);
  mustStoreCmd("G1 Z10\n");

  mustStoreCmd(ENABLE_STEPPER_CMD);
  mustStoreCmd(DISABLE_STEPPER_CMD);
}

// Draw values under icons
static inline void refreshValue(MENUITEMS * levelItems, uint8_t index)
{
  sprintf((char *)levelItems->items[valIconIndex[index]].label.address, "%1.4f", levelCornerPosition[index]);
  menuDrawIconText(&levelItems->items[valIconIndex[index]], valIconIndex[index]);
}

// Show M48 on icon
static inline void drawProbeAccuracyIcon(MENUITEMS * levelItems)
{
  uint8_t index = 4;
  GUI_POINT loc;
  LIVE_INFO lvIcon;
  const ITEM * menuitem = &levelItems->items[valIconIndex[index]];
  char * str = "M48";

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
  lvIcon.lines[0].font = FONT_SIZE_NORMAL;

  lvIcon.lines[1].h_align = LEFT;
  lvIcon.lines[1].v_align = BOTTOM;
  lvIcon.lines[1].fn_color = WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = (GUI_POINT){loc.x - 2, loc.y - 2};
  lvIcon.lines[1].font = FONT_SIZE_NORMAL;

  lvIcon.lines[0].text = (uint8_t *)str;
  lvIcon.lines[1].text = (uint8_t *)str;

  showLiveInfo(valIconIndex[index], &lvIcon, menuitem);
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
  float oldValue[5];
  char iconText[5][15];

  levelCornerItems.items[0].label.address = (uint8_t *)iconText[0];
  levelCornerItems.items[1].label.address = (uint8_t *)iconText[1];
  levelCornerItems.items[3].label.address = (uint8_t *)iconText[2];
  levelCornerItems.items[4].label.address = (uint8_t *)iconText[3];
  levelCornerItems.items[5].label.address = (uint8_t *)iconText[4];

  for (uint8_t i = 0; i < LC_VALUE_COUNT; i++)
  {
    refreshValue(&levelCornerItems, i);
    oldValue[i] = levelCornerPosition[i];
  }

  menuDrawPage(&levelCornerItems);
  drawProbeAccuracyIcon(&levelCornerItems);

  mustStoreCmd("G28\n");  // Init Coordinate

  // Check min edge limit for the probe with probe offset set in parseACK.c
  uint8_t edge_min = MAX(ABS(getParameter((int16_t)P_PROBE_OFFSET, AXIS_INDEX_X)),
                         ABS((int16_t)getParameter(P_PROBE_OFFSET, AXIS_INDEX_Y))) + 1;

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

      case KEY_ICON_1:
        ScanLevelCorner(2);
        break;

      case KEY_ICON_2:
      {
        infoSettings.level_edge = editIntValue(edge_min, LEVELING_EDGE_DISTANCE_MAX,
                                               LEVELING_EDGE_DISTANCE_DEFAULT, infoSettings.level_edge);
        menuDrawPage(&levelCornerItems);
        break;
      }

      case KEY_ICON_3:
        mustStoreCmd("M48\n");
        mustStoreCmd(ENABLE_STEPPER_CMD);
        mustStoreCmd(DISABLE_STEPPER_CMD);
        drawProbeAccuracyIcon(&levelCornerItems);
        break;

      case KEY_ICON_4:
        ScanLevelCorner(0);
        break;

      case KEY_ICON_5:
        ScanLevelCorner(1);
        break;

      case KEY_ICON_6:
        ScanLevelCorner(0);
        ScanLevelCorner(1);
        ScanLevelCorner(2);
        ScanLevelCorner(3);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    for (uint8_t i = 0; i < LC_VALUE_COUNT; i++)
    {
      if (oldValue[i] != levelCornerPosition[i])
      {
        refreshValue(&levelCornerItems, i);
        oldValue[i] = levelCornerPosition[i];
      }
    }

    loopProcess();
  }
}
