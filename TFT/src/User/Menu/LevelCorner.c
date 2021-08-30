#include "LevelCorner.h"
#include "includes.h"

const uint8_t valIconIndex[LEVEL_POINT_COUNT] = {4, 5, 1, 0, 3};

// Buffer current Z value measured in Level Corner = {position 1, position 2, position 3, position 4, probe accuracy(M48)}
float levelCornerPosition[LEVEL_POINT_COUNT] = {0};

// Set level corner position from the Z offset measured by probe
void setLevelCornerPosition(int16_t x, int16_t y, float position)
{
  levelCornerPosition[levelingGetPoint(x, y)] = position;
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
  float oldValue[LEVEL_POINT_COUNT];
  char iconText[LEVEL_POINT_COUNT][15];

  for (uint8_t i = 0; i < LEVEL_POINT_COUNT; i++)
  {
    levelCornerItems.items[valIconIndex[i]].label.address = (uint8_t *)iconText[i];
  }

  for (uint8_t i = 0; i < LEVEL_POINT_COUNT; i++)
  {
    refreshValue(&levelCornerItems, i);
    oldValue[i] = levelCornerPosition[i];
  }

  menuDrawPage(&levelCornerItems);
  drawProbeAccuracyIcon(&levelCornerItems);

  if (coordinateIsKnown() == false)
    probeHeightHomeAndRaise();  // home and raise nozzle

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
        levelingProbePoint(LEVEL_TOP_LEFT);
        break;

      case KEY_ICON_1:
        levelingProbePoint(LEVEL_TOP_RIGHT);
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
        levelingProbePoint(LEVEL_BOTTOM_LEFT);
        break;

      case KEY_ICON_5:
        levelingProbePoint(LEVEL_BOTTOM_RIGHT);
        break;

      case KEY_ICON_6:
        levelingProbePoint(LEVEL_BOTTOM_LEFT);
        levelingProbePoint(LEVEL_BOTTOM_RIGHT);
        levelingProbePoint(LEVEL_TOP_RIGHT);
        levelingProbePoint(LEVEL_TOP_LEFT);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    for (uint8_t i = 0; i < LEVEL_POINT_COUNT; i++)
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
