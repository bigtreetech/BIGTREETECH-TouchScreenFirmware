#include "Heat.h"
#include "includes.h"

static uint8_t tool_index = NOZZLE0;
static uint8_t degreeSteps_index = 1;

void heatSetCurrentIndex(uint8_t index)
{
  tool_index = index;
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
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
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

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  heatItems.items[KEY_ICON_4] = itemTool[tool_index];
  heatItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  menuDrawPage(&heatItems);
  temperatureReDraw(tool_index, NULL, false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    actCurrent = heatGetCurrentTemp(tool_index);
    actTarget = heatGetTargetTemp(tool_index);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetTargetTemp(tool_index, actTarget - degreeSteps[degreeSteps_index]);
        break;

      case KEY_INFOBOX:
      {
        int16_t val = editIntValue(0, infoSettings.max_temp[tool_index], 0, actTarget);

        if (val != actTarget)
          heatSetTargetTemp(tool_index, val);

        menuDrawPage(&heatItems);
        temperatureReDraw(tool_index, NULL, false);
        break;
      }

      case KEY_ICON_3:
        heatSetTargetTemp(tool_index, actTarget + degreeSteps[degreeSteps_index]);
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
        heatSetTargetTemp(tool_index, 0);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (encoderPosition > 0)
              heatSetTargetTemp(tool_index, actTarget + degreeSteps[degreeSteps_index]);
            else  // if < 0)
              heatSetTargetTemp(tool_index, actTarget - degreeSteps[degreeSteps_index]);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      temperatureReDraw(tool_index, NULL, true);
    }

    loopProcess();
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
