#include "Heat.h"
#include "includes.h"

static uint8_t tool_index = NOZZLE0;
static uint8_t last_nozzle_index = NOZZLE0;

void heatSetCurrentIndex(uint8_t index)
{
  tool_index = (index == LAST_NOZZLE) ? last_nozzle_index : index;
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

  static uint8_t degreeSteps_index = 1;

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
  temperatureReDraw(tool_index, NULL, true);

  while (MENU_IS(menuHeat))
  {
    actCurrent = heatGetCurrentTemp(tool_index);
    actTarget = setTarget = heatGetTargetTemp(tool_index);
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        setTarget -= degreeSteps[degreeSteps_index];
        break;

      case KEY_INFOBOX:
      {
        setTarget = editIntValue(0, infoSettings.max_temp[tool_index], 0, actTarget);

        temperatureReDraw(tool_index, NULL, true);
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
        temperatureReDraw(tool_index, NULL, true);
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
      actTarget = setTarget;
      heatSetTargetTemp(tool_index, setTarget, FROM_GUI);
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;

      temperatureReDraw(tool_index, NULL, false);
    }

    loopProcess();
  }

  if (WITHIN(tool_index, NOZZLE0, NOZZLE5))
    last_nozzle_index = tool_index;  // save last used hotend index

  // set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
