#include "common.h"
#include "includes.h"

// indexes for status icon toggles
uint8_t currentTool = NOZZLE0;
uint8_t currentFan = 0;
uint8_t currentSpeedID = 0;
static uint32_t lastTime = 0;

//Icons list for tool change
const ITEM itemTool[MAX_HEATER_COUNT] =
{
// icon                       label
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_BED,                  LABEL_BED},
  {ICON_CHAMBER,              LABEL_CHAMBER},
};

// Icons list for Temperature step change
const ITEM itemDegreeSteps[ITEM_DEGREE_NUM] =
{
// icon                       label
  {ICON_1_DEGREE,             LABEL_1_DEGREE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_10_DEGREE,            LABEL_10_DEGREE},
};

// List for temperature step change
const u8 degreeSteps[ITEM_DEGREE_NUM] = {1, 5, 10};

// Icons list for speed change steps
const ITEM itemSpeed[ITEM_SPEED_NUM] =
{
// icon                       label
  {ICON_SLOW_SPEED,           LABEL_SLOW_SPEED},
  {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
  {ICON_FAST_SPEED,           LABEL_FAST_SPEED},
};

// Icons list for percent change steps
const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM] =
{
// icon                       label
  {ICON_E_1_PERCENT,          LABEL_1_PERCENT},
  {ICON_E_5_PERCENT,          LABEL_5_PERCENT},
  {ICON_E_10_PERCENT,         LABEL_10_PERCENT},
};

// List for percent change steps
const u8 percentSteps[ITEM_PERCENT_STEPS_NUM] = {1, 5, 10};

// Icons list for axis length/distance change steps
const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] =
{
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_10_MM,                LABEL_10_MM},
  {ICON_100_MM,               LABEL_100_MM},
};

// List for length/distance change steps
const float moveLenSteps[ITEM_MOVE_LEN_NUM] = {0.01f, 0.1f, 1, 10, 100};

// Icons list for Extruder length/distance change steps
const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM] =
{
// icon                       label
  {ICON_E_1_MM,               LABEL_1_MM},
  {ICON_E_5_MM,               LABEL_5_MM},
  {ICON_E_10_MM,              LABEL_10_MM},
  {ICON_E_100_MM,             LABEL_100_MM},
  {ICON_E_200_MM,             LABEL_200_MM},
};

// List for extruder length/distance change steps
const float extlenSteps[ITEM_EXT_LEN_NUM] = {1.0f, 5.0f, 10.0f, 100.0f, 200.0f};

// Check time elapsed against the time specified in milliseconds for displaying/updating info on screen
// Use this for timed screen updates in menu loops only
bool nextScreenUpdate(uint32_t duration)
{
  uint32_t curTime = OS_GetTimeMs();
  if (curTime > (lastTime + duration))
  {
    lastTime = curTime;
    return true;
  }
  else
  {
    return false;
  }
}
