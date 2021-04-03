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
// icon                          label
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_BED,                     LABEL_BED},
  {ICON_CHAMBER,                 LABEL_CHAMBER},
};

// Icons list for Temperature step change
const ITEM itemDegreeSteps[ITEM_DEGREE_NUM] =
{
// icon                          label
  {ICON_1_DEGREE,                LABEL_1_DEGREE},
  {ICON_5_DEGREE,                LABEL_5_DEGREE},
  {ICON_10_DEGREE,               LABEL_10_DEGREE},
};

// List for temperature step change
const uint8_t degreeSteps[ITEM_DEGREE_NUM] = {1, 5, 10};

// Icons list for speed change steps
const ITEM itemSpeed[ITEM_SPEED_NUM] =
{
// icon                          label
  {ICON_SLOW_SPEED,              LABEL_SLOW},
  {ICON_NORMAL_SPEED,            LABEL_NORMAL},
  {ICON_FAST_SPEED,              LABEL_FAST},
};

// Icons list for percent change steps
const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM] =
{
// icon                          label
  {ICON_E_1_PERCENT,             LABEL_1_PERCENT},
  {ICON_E_5_PERCENT,             LABEL_5_PERCENT},
  {ICON_E_10_PERCENT,            LABEL_10_PERCENT},
};

// List for percent change steps
const uint8_t percentSteps[ITEM_PERCENT_STEPS_NUM] = {1, 5, 10};

// Icons list for axis length/distance change steps
const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] =
{
// icon                          label
  {ICON_001_MM,                  LABEL_001_MM},
  {ICON_01_MM,                   LABEL_01_MM},
  {ICON_1_MM,                    LABEL_1_MM},
  {ICON_10_MM,                   LABEL_10_MM},
  {ICON_100_MM,                  LABEL_100_MM},
};

// List for length/distance change steps
const float moveLenSteps[ITEM_MOVE_LEN_NUM] = {0.01f, 0.1f, 1, 10, 100};

// Icons list for Extruder length/distance change steps
const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM] =
{
// icon                          label
  {ICON_E_1_MM,                  LABEL_1_MM},
  {ICON_E_5_MM,                  LABEL_5_MM},
  {ICON_E_10_MM,                 LABEL_10_MM},
  {ICON_E_100_MM,                LABEL_100_MM},
  {ICON_E_200_MM,                LABEL_200_MM},
};

// List for extruder length/distance change steps
const float extlenSteps[ITEM_EXT_LEN_NUM] = {1.0f, 5.0f, 10.0f, 100.0f, 200.0f};

// Labels list for ON/OFF settings
const LABEL itemToggle[ITEM_TOGGLE_NUM] =
{
  LABEL_OFF,
  LABEL_ON
};

const uint16_t iconToggle[ITEM_TOGGLE_NUM] =
{
  ICONCHAR_TOGGLE_OFF,
  ICONCHAR_TOGGLE_ON
};

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

const bool warmupTemperature(uint8_t toolIndex, void (* callback)(void))
{
  if (heatGetCurrentTemp(toolIndex) < infoSettings.min_ext_temp)
  { // low temperature warning
    char tempMsg[120];
    LABELCHAR(tempStr, LABEL_EXT_TEMPLOW);

    sprintf(tempMsg, tempStr, infoSettings.min_ext_temp);
    strcat(tempMsg, "\n");
    sprintf(tempStr, (char *) textSelect(LABEL_HEAT_HOTEND), infoSettings.min_ext_temp);
    strcat(tempMsg, tempStr);

    setDialogText(LABEL_WARNING, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
    showDialog(DIALOG_TYPE_ERROR, callback, NULL, NULL);

    return false;
  }

  return true;
}

const void cooldownTemperature(void)
{
  if (!isPrinting())
  {
    for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
    {
      if (heatGetTargetTemp(i) > 0)
      {
        setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_CONFIRM, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
        break;
      }
    }
  }
}

// Show/draw a temperature in a standard menu
const void temperatureReDraw(uint8_t toolIndex, int16_t * temp, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    sprintf(tempstr, "%-15s", heatDisplayID[toolIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "ºC");
  }

  if (temp != NULL)
    sprintf(tempstr, "  %d  ", *temp);
  else
    sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(toolIndex), heatGetTargetTemp(toolIndex));

  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Show/draw fan in a standard menu
const void fanReDraw(uint8_t fanIndex, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    sprintf(tempstr, "%-15s", fanID[fanIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);

    if (infoSettings.fan_percentage == 1)
    {
      GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "%");
    }
    else
    {
      GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "PWM");
    }
  }

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%4d/%-4d", fanGetCurPercent(fanIndex), fanGetSetPercent(fanIndex));
  else
    sprintf(tempstr, "%4d/%-4d", fanGetCurSpeed(fanIndex), fanGetSetSpeed(fanIndex));

  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Show/draw extruder in a standard menu
const void extruderReDraw(uint8_t extruderIndex, float extrusion, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    sprintf(tempstr, "%-15s", extruderDisplayID[extruderIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "mm");
  }

  sprintf(tempstr, "  %.2f  ", extrusion);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Show/draw percentage in a standard menu
const void percentageReDraw(uint8_t itemIndex, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    setLargeFont(false);

    if (itemIndex == 0)
      sprintf(tempstr, "%-15s", textSelect(LABEL_PERCENTAGE_SPEED));
    else
      sprintf(tempstr, "%-15s", textSelect(LABEL_PERCENTAGE_FLOW));

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "%");
  }

  sprintf(tempstr, "%4d/%-4d", speedGetCurPercent(itemIndex), speedGetSetPercent(itemIndex));
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Edit an integer value in a standard menu
const int16_t editIntValue(int16_t minValue, int16_t maxValue, int16_t resetValue, int16_t value)
{
  int16_t val;
  char tempstr[30];

  sprintf(tempstr, "Min:%i | Max:%i", minValue, maxValue);

  val = numPadInt((uint8_t *) tempstr, value, resetValue, false);
  val = NOBEYOND(minValue, val, maxValue);

  return val;
}

// Edit a float value in a standard menu
const float editFloatValue(float minValue, float maxValue, float resetValue, float value)
{
  float val;
  char tempstr[30];

  sprintf(tempstr, "Min:%.2f | Max:%.2f", minValue, maxValue);

  val = numPadFloat((uint8_t *) tempstr, value, resetValue, true);
  val = NOBEYOND(minValue, val, maxValue);

  return val;
}
