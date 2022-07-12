#include "Heat.h"
#include "includes.h"

static uint8_t tool_index = NOZZLE0;
static uint8_t last_hotend_index = NOZZLE0;
static uint8_t last_bed_index = BED;
static uint8_t degreeSteps_index = 1;

void heatSetCurrentIndex(int8_t index)
{
  if (index >= 0)  // set specific tool
    tool_index = (uint8_t)(index);
  else if (index == -1)  // set last used hotend index
    tool_index = last_hotend_index;
  else  // set last used bed index
    tool_index = last_bed_index;
}

void menuHeat(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS heatItems = {
    // title
    LABEL_HEAT,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_INC,                     LABEL_INC},
      {ICON_NOZZLE,                  LABEL_NOZZLE},
      {ICON_5_DEGREE,                LABEL_5_DEGREE},
      {ICON_STOP,                    LABEL_STOP},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  int16_t lastCurrent = heatGetCurrentTemp(tool_index);
  int16_t lastTarget = heatGetTargetTemp(tool_index);
  int16_t actCurrent;
  int16_t actTarget;
  int16_t setTarget;

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  heatItems.items[KEY_ICON_4] = itemTool[tool_index];
  heatItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  menuDrawPage(&heatItems);
  temperatureReDraw(tool_index, NULL, false);

  while (MENU_IS(menuHeat))
  {
    actCurrent = heatGetCurrentTemp(tool_index);
    actTarget = heatGetTargetTemp(tool_index);
    key_num = menuKeyGetValue();

    setTarget = actTarget;

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        setTarget -= degreeSteps[degreeSteps_index];
        break;

      case KEY_INFOBOX:
      {
        setTarget = editIntValue(0, infoSettings.max_temp[tool_index], 0, actTarget);
        temperatureReDraw(tool_index, NULL, false);
        break;
      }

      case KEY_ICON_3:
      case KEY_INCREASE:
          setTarget += degreeSteps[degreeSteps_index];
        break;

      case KEY_ICON_4:
        do
        {
          tool_index = (tool_index + 1) % MAX_HEATER_COUNT;
        } while (!heaterDisplayIsValid(tool_index));

        heatItems.items[key_num] = itemTool[tool_index];

        menuDrawItem(&heatItems.items[key_num], key_num);
        temperatureReDraw(tool_index, NULL, false);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

        menuDrawItem(&heatItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        heatSetTargetTemp(tool_index, 0, FROM_GUI);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (setTarget != lastTarget)
    {
      heatSetTargetTemp(tool_index, setTarget, FROM_GUI);
      actTarget = setTarget;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      temperatureReDraw(tool_index, NULL, true);
    }

    loopProcess();
  }

  if (tool_index < BED)
    last_hotend_index = tool_index;  // save last used hotend index
  else
    last_bed_index = tool_index;  // save last used bed index

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
