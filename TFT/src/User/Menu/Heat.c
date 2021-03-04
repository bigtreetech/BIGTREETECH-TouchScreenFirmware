#include "Heat.h"
#include "includes.h"
#include "Numpad.h"
#include "Settings.h"

static uint8_t degreeSteps_index = 1;
static uint8_t c_heater = NOZZLE0;

void heatSetCurrentIndex(uint8_t index)
{
  c_heater = index;
}

// Show/draw temperature in heat menu
void showTemperature(uint8_t index)
{
  char tempstr[20];

  sprintf(tempstr, "%-15s", heatDisplayID[index]);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);

  sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(index), heatGetTargetTemp(index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

void menuHeat(void)
{
  int16_t lastCurrent = heatGetCurrentTemp(c_heater);
  int16_t lastTarget = heatGetTargetTemp(c_heater);

  MENUITEMS heatItems = {
    // title
    LABEL_HEAT,
    // icon                         label
    {{ICON_DEC,                     LABEL_DEC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_INC,                     LABEL_INC},
     {ICON_NOZZLE,                  LABEL_NOZZLE},
     {ICON_5_DEGREE,                LABEL_5_DEGREE},
     {ICON_STOP,                    LABEL_STOP},
     {ICON_BACK,                    LABEL_BACK},}
  };

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  heatItems.items[KEY_ICON_4] = itemTool[c_heater];
  heatItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];
  menuDrawPage(&heatItems);
  showTemperature(c_heater);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    int16_t actCurrent = heatGetCurrentTemp(c_heater);
    int16_t actTarget = heatGetTargetTemp(c_heater);

    switch(key_num)
    {
      case KEY_ICON_0:
        heatSetTargetTemp(c_heater, actTarget - degreeSteps[degreeSteps_index]);
        break;

      case KEY_INFOBOX:
      {
        char titlestr[30];
        sprintf(titlestr, "Min:0 | Max:%i", infoSettings.max_temp[c_heater]);

        int16_t val = numPadInt((uint8_t *) titlestr, actTarget, 0, false);
        val = NOBEYOND(0, val, infoSettings.max_temp[c_heater]);

        if (val != actTarget)
          heatSetTargetTemp(c_heater, val);

        menuDrawPage(&heatItems);
        showTemperature(c_heater);
        break;
      }

      case KEY_ICON_3:
        heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index]);
        break;

      case KEY_ICON_4:
        do
        {
          c_heater = (c_heater + 1) % MAX_HEATER_COUNT;
        } while (!heaterIsValid(c_heater));

        heatItems.items[key_num] = itemTool[c_heater];
        menuDrawItem(&heatItems.items[key_num], key_num);
        showTemperature(c_heater);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegreeSteps[degreeSteps_index];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        heatSetTargetTemp(c_heater, 0);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (encoderPosition > 0)
              heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index]);
            else // if < 0)
              heatSetTargetTemp(c_heater, actTarget - degreeSteps[degreeSteps_index]);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      showTemperature(c_heater);
    }

    loopProcess();
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
