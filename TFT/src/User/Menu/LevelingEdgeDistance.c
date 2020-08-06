#include "LevelingEdgeDistance.h"
#include "includes.h"

#define ITEM_LEVELING_EDGE_DISTANCE_UNIT_NUM 3

const ITEM itemLevelingEdgeDistanceUnit[ITEM_LEVELING_EDGE_DISTANCE_UNIT_NUM] = {
  // icon                           label
  {ICON_E_1_MM,                     LABEL_1_MM},
  {ICON_E_5_MM,                     LABEL_5_MM},
  {ICON_E_10_MM,                    LABEL_10_MM},
};

const u8  levelingEdgeDistanceUnit[ITEM_LEVELING_EDGE_DISTANCE_UNIT_NUM] = {1, 5, 10};
static u8 curUnit = 1;

s16 edgeDistance[2];

/* Initialize leveling edge distance */
void levInitEdgeDistance(void)
{
  infoSettings.level_edge =
    limitValue(LEVELING_EDGE_DISTANCE_MIN_VALUE, infoSettings.level_edge, LEVELING_EDGE_DISTANCE_MAX_VALUE);

  edgeDistance[0] = infoSettings.level_edge;
  edgeDistance[1] = infoSettings.level_edge;
}

/* Decrease leveling edge distance */
void levDecEdgeDistance(u8 axis)
{
  if (axis > 1)
    return;

  if (edgeDistance[axis] > LEVELING_EDGE_DISTANCE_MIN_VALUE)
  {
    edgeDistance[axis] =
      limitValue(LEVELING_EDGE_DISTANCE_MIN_VALUE, edgeDistance[axis] - levelingEdgeDistanceUnit[curUnit], LEVELING_EDGE_DISTANCE_MAX_VALUE);

    infoSettings.level_edge = edgeDistance[axis];
  }
}

/* Increase leveling edge distance */
void levIncEdgeDistance(u8 axis)
{
  if (axis > 1)
    return;

  if (edgeDistance[axis] < LEVELING_EDGE_DISTANCE_MAX_VALUE)
  {
    edgeDistance[axis] =
      limitValue(LEVELING_EDGE_DISTANCE_MIN_VALUE, edgeDistance[axis] + levelingEdgeDistanceUnit[curUnit], LEVELING_EDGE_DISTANCE_MAX_VALUE);

    infoSettings.level_edge = edgeDistance[axis];
  }
}

/* Set default leveling edge distance */
void levSetDefaultEdgeDistance(void)
{
  for (int i = 0; i < 2; i++)
  {
    edgeDistance[i] = LEVELING_EDGE_DISTANCE_DEFAULT_VALUE;

    infoSettings.level_edge = edgeDistance[i];
  }
}

void levelingEdgeDistanceReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%s    ", LEVELING_EDGE_DISTANCE_DISPLAY_ID);

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  }

  sprintf(tempstr, "  %d%s%d  ", edgeDistance[0], "/", edgeDistance[1]);

  setLargeFont(true);

  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);

  setLargeFont(false);
}

void menuLevelingEdgeDistance(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS levelingEdgeDistanceItems = {
    // title
    LABEL_LEVELING_EDGE_DISTANCE,
    // icon                         label
    {{ICON_DEC,                     LABEL_DEC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_INC,                     LABEL_INC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_E_5_MM,                  LABEL_5_MM},
     {ICON_RESET_VALUE,             LABEL_DEFAULT},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  while (infoCmd.count != 0)
  {
    loopProcess();
  }

  levInitEdgeDistance();

  levelingEdgeDistanceItems.items[KEY_ICON_5] = itemLevelingEdgeDistanceUnit[curUnit];

  menuDrawPage(&levelingEdgeDistanceItems);
  levelingEdgeDistanceReDraw(false);

  while (infoMenu.menu[infoMenu.cur] == menuLevelingEdgeDistance)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        levDecEdgeDistance(0);
        levDecEdgeDistance(1);

        levelingEdgeDistanceReDraw(true);
        break;

      case KEY_ICON_3:
        levIncEdgeDistance(0);
        levIncEdgeDistance(1);

        levelingEdgeDistanceReDraw(true);
        break;

      case KEY_ICON_5:
        curUnit = (curUnit + 1) % ITEM_LEVELING_EDGE_DISTANCE_UNIT_NUM;

        levelingEdgeDistanceItems.items[key_num] = itemLevelingEdgeDistanceUnit[curUnit];

        menuDrawItem(&levelingEdgeDistanceItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        levSetDefaultEdgeDistance();

        levelingEdgeDistanceReDraw(true);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
