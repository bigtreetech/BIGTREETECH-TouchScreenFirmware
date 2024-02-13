#include "LevelCorner.h"
#include "includes.h"

// correlation between KEY position and measure point index
const uint8_t valIconIndex[LEVELING_POINT_COUNT - 1] = {KEY_ICON_4, KEY_ICON_5, KEY_ICON_1, KEY_ICON_0};

// buffer current Z value measured in Level Corner = {position 1, position 2, position 3, position 4}
float levelCornerPosition[LEVELING_POINT_COUNT - 1] = {0};

int16_t origLevelEdge = -1;

uint8_t getLevelEdgeMin(void)
{
  // min edge limit for the probe with probe offset set in Mainboard_AckHandler.c
  int16_t maxXedge = getParameter(P_PROBE_OFFSET, AXIS_INDEX_X) + getParameter(P_HOME_OFFSET, AXIS_INDEX_X);
  int16_t maxYedge = getParameter(P_PROBE_OFFSET, AXIS_INDEX_Y) + getParameter(P_HOME_OFFSET, AXIS_INDEX_Y);

  maxXedge = ABS(maxXedge);
  maxYedge = ABS(maxYedge);

  return MAX(maxXedge, maxYedge) + 1;
}

uint8_t getLevelEdgeDefault(void)
{
  return MAX(origLevelEdge, getLevelEdgeMin());
}

void setLevelEdgeMin(void)
{
  infoSettings.level_edge = getLevelEdgeMin();
}

// draw values under icons
void refreshValue(MENUITEMS * levelItems, uint8_t index)
{
  sprintf((char *)levelItems->items[valIconIndex[index]].label.address, "%.4f", levelCornerPosition[index]);
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
  char iconText[LEVELING_POINT_COUNT - 1][10] = {"---", "---", "---", "---"};

  if (origLevelEdge < 0)  // initialize leveling edge value to be used for leveling corner if not yet initialized (-1)
  {
    origLevelEdge = infoSettings.level_edge;          // save leveling edge value to restore after leveling corner completion
    infoSettings.level_edge = getLevelEdgeDefault();  // set leveling edge value for leveling corner
  }

  for (uint8_t i = 0; i < LEVELING_POINT_COUNT - 1; i++)
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
        for (uint8_t i = 0; i < LEVELING_POINT_COUNT - 1; i++)
        {
          if (key_num < KEY_ICON_6 && key_num != valIconIndex[i])
            continue;

          levelingProbePoint(i);

          // wait until point probing is executed
          TASK_LOOP_WHILE(levelingGetProbedPoint() == LEVEL_NO_POINT);

          levelCornerPosition[i] = levelingGetProbedZ();
          refreshValue(&levelCornerItems, i);
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
        infoSettings.level_edge = origLevelEdge;  // restore original leveling edge value
        origLevelEdge = -1;
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
