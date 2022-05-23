#include "LevelCorner.h"
#include "includes.h"

const uint8_t valIconIndex[LEVELING_POINT_COUNT] = {4, 5, 1, 0, 3};

// buffer current Z value measured in Level Corner = {position 1, position 2, position 3, position 4, probe accuracy(M48)}
float levelCornerPosition[LEVELING_POINT_COUNT] = {0};

int16_t origLevelEdge = -1;

uint8_t getLevelEdgeMin(void)
{
  // min edge limit for the probe with probe offset set in parseACK.c
  return MAX(ABS((int16_t)getParameter(P_PROBE_OFFSET, AXIS_INDEX_X)),
             ABS((int16_t)getParameter(P_PROBE_OFFSET, AXIS_INDEX_Y))) + 1;
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

void checkRefreshValue(MENUITEMS * levelItems)
{
  LEVELING_POINT levelingPoint = levelingGetProbedPoint();

  if (levelingPoint != LEVEL_NO_POINT)
  {
    levelCornerPosition[levelingPoint] = levelingGetProbedZ();
    refreshValue(levelItems, levelingPoint);

    levelingResetProbedPoint();  // reset to check for new updates
  }
}

// show M48 on icon
void drawProbeAccuracyIcon(MENUITEMS * levelItems)
{
  uint8_t index = 4;
  GUI_POINT loc;
  LIVE_INFO lvIcon;
  char * str = "M48";

  loc.x = 5;
  loc.y = ICON_HEIGHT - 5;

  lvIcon.iconIndex = levelItems->items[valIconIndex[index]].icon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = false;

  lvIcon.lines[0].h_align = LEFT;
  lvIcon.lines[0].v_align = BOTTOM;
  lvIcon.lines[0].fn_color = LC_VAL_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = loc;
  lvIcon.lines[0].font = FONT_SIZE_NORMAL;

  lvIcon.lines[1].h_align = LEFT;
  lvIcon.lines[1].v_align = BOTTOM;
  lvIcon.lines[1].fn_color = LC_VAL_COLOR_2;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = (GUI_POINT){loc.x - 2, loc.y - 2};
  lvIcon.lines[1].font = FONT_SIZE_NORMAL;

  lvIcon.lines[0].text = (uint8_t *)str;
  lvIcon.lines[1].text = (uint8_t *)str;

  showLiveInfo(valIconIndex[index], &lvIcon, false);
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
  char iconText[LEVELING_POINT_COUNT][15];

  if (origLevelEdge < 0)  // initialize leveling edge value to be used for leveling corner if not yet initialized (-1)
  {
    mustStoreCmd(UNLOCK_STEPPER_CMD);  // disarm all steppers just to force initial homing
    mustStoreCmd(LOCK_STEPPER_CMD);

    origLevelEdge = infoSettings.level_edge;          // save leveling edge value to restore after leveling corner completion
    infoSettings.level_edge = getLevelEdgeDefault();  // set leveling edge value for leveling corner
  }

  for (uint8_t i = 0; i < LEVELING_POINT_COUNT; i++)
  {
    levelCornerItems.items[valIconIndex[i]].label.address = (uint8_t *)iconText[i];

    refreshValue(&levelCornerItems, i);
  }

  menuDrawPage(&levelCornerItems);
  drawProbeAccuracyIcon(&levelCornerItems);

  while (MENU_IS(menuLevelCorner))
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
        uint8_t curLevelEdge = infoSettings.level_edge;

        infoSettings.level_edge = editIntValue(LEVELING_EDGE_DISTANCE_MIN, LEVELING_EDGE_DISTANCE_MAX,
                                               getLevelEdgeDefault(), infoSettings.level_edge);

        if (curLevelEdge >= getLevelEdgeMin() && infoSettings.level_edge < getLevelEdgeMin())  // if new value is below min limit
        {
          setDialogText(LABEL_WARNING, LABEL_LEVEL_CORNER_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, setLevelEdgeMin, NULL, NULL);
        }

        break;
      }

      case KEY_ICON_3:
        mustStoreCmd("M48\n");

        drawProbeAccuracyIcon(&levelCornerItems);
        break;

      case KEY_ICON_4:
        levelingProbePoint(LEVEL_BOTTOM_LEFT);
        break;

      case KEY_ICON_5:
        levelingProbePoint(LEVEL_BOTTOM_RIGHT);
        break;

      case KEY_ICON_6:
        for (int i = LEVEL_BOTTOM_LEFT; i < LEVELING_POINT_COUNT - 1; i++)
        {
          levelingProbePoint(i);

          while (isNotEmptyCmdQueue())  // wait for the communication to be clean
          {
            loopProcess();

            checkRefreshValue(&levelCornerItems);
          }
        }

        break;

      case KEY_ICON_7:
        mustStoreCmd(UNLOCK_STEPPER_CMD);

        infoSettings.level_edge = origLevelEdge;  // restore original leveling edge value
        origLevelEdge = -1;
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();

    checkRefreshValue(&levelCornerItems);
  }
}
