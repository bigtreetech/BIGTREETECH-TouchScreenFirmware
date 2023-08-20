#include "LevelCorner.h"
#include "includes.h"

const uint8_t valIconIndex[] = {KEY_ICON_4, KEY_ICON_5, KEY_ICON_1, KEY_ICON_0};  // correlation between KEY position and measure point index

// buffer current Z value measured in Level Corner = {position 1, position 2, position 3, position 4}
float levelCornerPosition[LEVELING_POINT_COUNT - 1] = {0};

int16_t origLevelEdge = -1;

static uint8_t getLevelEdgeMin(void)
{
  uint16_t min_edge = 0;
  int16_t single_edge;
  int16_t probeOffset;
  int16_t homeOffset;

  // check edges of X axis
  probeOffset = getParameter(P_PROBE_OFFSET, X_AXIS);
  homeOffset = getParameter(P_HOME_OFFSET, X_AXIS);

  single_edge = infoSettings.endstop_min[X_AXIS] + probeOffset + homeOffset;
  if (single_edge > min_edge) min_edge = single_edge;

  single_edge = infoSettings.bed_size[X_AXIS] - infoSettings.endstop_max[X_AXIS] - probeOffset - homeOffset;
  if (single_edge > min_edge) min_edge = single_edge;

  // check edges of Y axis
  probeOffset = getParameter(P_PROBE_OFFSET, Y_AXIS);
  homeOffset = getParameter(P_HOME_OFFSET, Y_AXIS);

  single_edge = infoSettings.endstop_min[Y_AXIS] + probeOffset + homeOffset;
  if (single_edge > min_edge) min_edge = single_edge;

  single_edge = infoSettings.bed_size[Y_AXIS] - infoSettings.endstop_max[Y_AXIS] - probeOffset - homeOffset;
  if (single_edge > min_edge) min_edge = single_edge;

  return min_edge + 1;
}

static uint8_t getLevelEdgeDefault(void)
{
  return MAX(origLevelEdge, getLevelEdgeMin());
}

static void setLevelEdgeMin(void)
{
  infoSettings.level_edge = getLevelEdgeMin();
}

// draw values under icons
static void refreshValue(MENUITEMS * levelItems, uint8_t index)
{
  sprintf((char *)levelItems->items[valIconIndex[index]].label.address, "%.3f", levelCornerPosition[index]);
  menuDrawIconText(&levelItems->items[valIconIndex[index]], valIconIndex[index]);
}

void menuLevelCorner(void)
{
  MENUITEMS levelCornerItems = {
    // title
    LABEL_LEVEL_CORNER,
    // icon                      label
    {
      {ICON_POINT_4,             LABEL_NULL},
      {ICON_POINT_3,             LABEL_NULL},
      {ICON_LEVEL_EDGE_DISTANCE, LABEL_DISTANCE},
      {ICON_Z_HOME,              LABEL_HOME},
      {ICON_POINT_1,             LABEL_NULL},
      {ICON_POINT_2,             LABEL_NULL},
      {ICON_RESUME,              LABEL_START},
      {ICON_BACK,                LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  char iconText[LEVELING_POINT_COUNT - 1][5] = {"---", "---", "---", "---"};

  if (origLevelEdge < 0)  // initialize leveling min_edge value to be used for leveling corner if not yet initialized (-1)
  {
    origLevelEdge = infoSettings.level_edge;          // save leveling min_edge value to restore after leveling corner completion
    infoSettings.level_edge = getLevelEdgeDefault();  // set leveling min_edge value for leveling corner
  }

  for (uint8_t i = LEVEL_BOTTOM_LEFT; i <= LEVEL_TOP_LEFT; i++)
  {
    levelCornerItems.items[valIconIndex[i]].label.address = (uint8_t *)iconText[i];
  }

  menuDrawPage(&levelCornerItems);

  while (MENU_IS(menuLevelCorner))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_4:
      case KEY_ICON_5:
      case KEY_ICON_6:
        for (uint8_t lvlPoint = LEVEL_BOTTOM_LEFT; lvlPoint <= LEVEL_TOP_LEFT; lvlPoint++)
        {
          if (key_num < KEY_ICON_6 && key_num != valIconIndex[lvlPoint])
            continue;

          levelingProbePoint(lvlPoint);

          // wait until point probing is executed
          TASK_LOOP_WHILE(levelingGetProbedPoint() == LEVEL_NO_POINT);

          levelCornerPosition[lvlPoint] = levelingGetProbedZ();
          refreshValue(&levelCornerItems, lvlPoint);
          levelingResetProbedPoint();  // reset to check for new updates
        }

        break;

      case KEY_ICON_2:
      {
        uint8_t curLevelEdge = infoSettings.level_edge;

        infoSettings.level_edge = editIntValue(LEVELING_EDGE_DISTANCE_MIN, LEVELING_EDGE_DISTANCE_MAX,
                                               getLevelEdgeDefault(), infoSettings.level_edge);

        if (curLevelEdge >= getLevelEdgeMin() && infoSettings.level_edge < getLevelEdgeMin())  // if new value is below min limit
          popupDialog(DIALOG_TYPE_QUESTION, LABEL_WARNING, LABEL_LEVEL_CORNER_INFO, LABEL_CONFIRM, LABEL_CANCEL, setLevelEdgeMin, NULL, NULL);

        break;
      }

      case KEY_ICON_3:
        mustStoreCmd("G28 Z\n");
        break;

      case KEY_ICON_7:
        infoSettings.level_edge = origLevelEdge;  // restore original leveling min_edge value
        origLevelEdge = -1;
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
