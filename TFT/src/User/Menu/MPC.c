#include "MPC.h"
#include "includes.h"

#define DEF_HEATER_POWER      40
#define DEF_FIL_HEAT_CAPACITY 0.0046f
#define MPC_PARAM_ITEMS_COUNT 3
#define MPC_METHOD_COUNT 3

typedef struct
{
  uint8_t heater_power;
  float fil_heat_capcity;
  enum {AUTOMATIC = 0, DIFFERENTIAL, ASYMPTOMATIC} method;
} MPC_Parameter;

static MPC_Parameter mpcParameter[MAX_HOTEND_COUNT] = {0};

static struct
{
  MPC_STATUS status;
  MPC_RESULT result;
} mpcTuning = {DONE, NO_RESULT};

static uint8_t curTool_index = NOZZLE0;
char * mpcMethod_label[] = {"Auto", "Diff.", "Asym."};

// 1 title, ITEM_PER_PAGE items (icon + label)
const MENUITEMS mpcItems = {
  // title
  LABEL_MPC_TITLE,
  // icon                          label
  {
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NOZZLE,                  LABEL_NOZZLE},
    {ICON_PARAMETER,               LABEL_PARAMETER_SETTINGS},
    {ICON_RESUME,                  LABEL_START},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void mpcDisplayValues(void)
{
  char tmpStr[15];
  MPC_Parameter * parameter = &mpcParameter[curTool_index];

  setFontSize(FONT_SIZE_LARGE);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)toolChange[curTool_index]);
  setFontSize(FONT_SIZE_NORMAL);

  sprintf(tmpStr, "P: %d W  ", parameter->heater_power);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0 + 2 * BYTE_HEIGHT, (uint8_t *)tmpStr);

  sprintf(tmpStr, "H: %.5f J/mm", parameter->fil_heat_capcity);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0 + 3 * BYTE_HEIGHT, (uint8_t *)tmpStr);

  sprintf(tmpStr, "%s %s", textSelect(LABEL_MPC_METHOD), mpcMethod_label[parameter->method]);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0 + 4 * BYTE_HEIGHT, (uint8_t *)tmpStr);
}

void menuSetMpcParam(void)
{
  LABEL title = {LABEL_PARAMETER_SETTINGS};
  uint16_t curIndex = KEY_IDLE;
  MPC_Parameter * parameter = &mpcParameter[curTool_index];

  LISTITEM paramItems[MPC_PARAM_ITEMS_COUNT] = {
    {CHARICON_EDIT,   LIST_CUSTOMVALUE, (LABEL){.address = "P: "}, LABEL_CUSTOM_VALUE},
    {CHARICON_EDIT,   LIST_CUSTOMVALUE, (LABEL){.address = "H: "}, LABEL_CUSTOM_VALUE},
    {CHARICON_DETAIL, LIST_CUSTOMVALUE, LABEL_MPC_METHOD,          LABEL_CUSTOM_VALUE}
  };

  setDynamicValue(0, parameter->heater_power);
  setDynamicValue(1, parameter->fil_heat_capcity);
  setDynamicTextValue(2, mpcMethod_label[parameter->method]);

  listViewCreate(title, paramItems, MPC_PARAM_ITEMS_COUNT, NULL, true, NULL, NULL);

  while (MENU_IS(menuSetMpcParam))
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case KEY_ICON_0:
      {
        int32_t tmpVal;

        do
        {
          tmpVal = numPadInt((uint8_t *)"1 ~ 255", parameter->heater_power, DEF_HEATER_POWER, false);

          if (WITHIN(tmpVal, 1, 255))
            break;
          else
            BUZZER_PLAY(SOUND_ERROR);
        } while (true);

        parameter->heater_power = tmpVal;
        setDynamicValue(0, parameter->heater_power);
        listViewRefreshMenu();
        break;
      }

      case KEY_ICON_1:
      {
        float tmpVal;

        do
        {
          tmpVal = numPadFloat((uint8_t *)"0.001 ~ 0.1", parameter->fil_heat_capcity, DEF_FIL_HEAT_CAPACITY, false);

          if (WITHIN(tmpVal, 0.001f, 0.1f))
            break;
          else
            BUZZER_PLAY(SOUND_ERROR);
        } while (true);

        parameter->fil_heat_capcity = tmpVal;
        setDynamicValue(1, parameter->fil_heat_capcity);
        listViewRefreshMenu();
        break;
      }

      case KEY_ICON_2:
        parameter->method = (parameter->method + 1) % MPC_METHOD_COUNT;
        setDynamicTextValue(2, mpcMethod_label[parameter->method]);
        listViewRefreshItem(curIndex);
        break;

      case KEY_BACK:
        mustStoreCmd("M306 E%d P%d H%.5f\n", curTool_index, parameter->heater_power, parameter->fil_heat_capcity);
        // CLOSE_MENU();  // commented because listViewCreate() handles it
        break;
    }

    loopProcess();
  }
}

void mpcStart(void)
{
  if (heatSetTool(curTool_index))
  {
    if (storeCmd("M306 T S%u\n", mpcParameter[curTool_index].method))
    {
      mpcTuning.status = REQUESTED;
      mpcTuning.result = NO_RESULT;
    }
  }
}

void setMpcTuningStatus(MPC_STATUS status)
{
  mpcTuning.status = status;
}

MPC_STATUS getMpcTuningStatus(void)
{
  return mpcTuning.status;
}

void setMpcTuningResult(MPC_RESULT result)
{
  mpcTuning.result = result;
}

void setMpcHeaterPower(uint8_t index, uint8_t power)
{
  mpcParameter[index].heater_power = power;
}

void setMpcFilHeatCapacity(uint8_t index, float capacity)
{
  mpcParameter[index].fil_heat_capcity = capacity;
}

bool hasMPC(void)
{
  return (mpcParameter[NOZZLE0].heater_power > 0);
}

void menuMPC(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&mpcItems);
  mpcDisplayValues();

  while (MENU_IS(menuMPC))
  {
    if (mpcTuning.status == DONE)
    {
      key_num = menuKeyGetValue();
      switch (key_num)
      {
        case KEY_ICON_4:
          do
          {
            curTool_index = (curTool_index + 1) % MAX_HOTEND_COUNT;
          } while (!heaterDisplayIsValid(curTool_index));

          mpcDisplayValues();
          break;

        case KEY_INFOBOX:
        case KEY_ICON_5:
          OPEN_MENU(menuSetMpcParam);
          break;

        case KEY_ICON_6:
          popupDialog(DIALOG_TYPE_QUESTION, mpcItems.title.index, LABEL_TUNE_START_INFO, LABEL_CONFIRM, LABEL_CANCEL, mpcStart, NULL, NULL);
          break;

        case KEY_ICON_7:
          // set (neopixel) LED light to current color or to OFF according to infoSettings.led_always_on and
          // restore infoSettings.knob_led_idle and knob LED color to their default values
          LED_SetPostEventColor();

          CLOSE_MENU();
          break;

        default:
          break;
      }
    }
    else if (mpcTuning.status == STARTED)
    {
      mpcTuning.status = ONGOING;

      LED_SetEventColor(&ledRed, false);  // set (neopixel) LED light to RED
      LCD_SET_KNOB_LED_IDLE(false);       // set infoSettings.knob_led_idle temporary to OFF

      popupSplash(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);
    }
    else if (mpcTuning.status == ONGOING)
    {
      if (mpcTuning.result != NO_RESULT)
      {
        mpcTuning.status = DONE;

        LED_SetEventColor(&ledGreen, false);  // set (neopixel) LED light to GREEN

        switch (mpcTuning.result)
        {
          case FINISHED:
          {
            LABELCHAR(tempMsg, LABEL_PROCESS_COMPLETED);

            BUZZER_PLAY(SOUND_SUCCESS);

            if (infoMachineSettings.EEPROM == 1)
            {
              sprintf(strchr(tempMsg, '\0'), "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

              popupDialog(DIALOG_TYPE_SUCCESS, LABEL_MPC_TITLE, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
            }
            else
            {
              popupReminder(DIALOG_TYPE_SUCCESS, LABEL_MPC_TITLE, (uint8_t *) tempMsg);
            }

            break;
          }

          case INTERRUPTED:
          {
            BUZZER_PLAY(SOUND_ERROR);

            popupReminder(DIALOG_TYPE_ERROR, LABEL_MPC_TITLE, LABEL_PROCESS_ABORTED);

            break;
          }

          default:
            break;
        }
      }
    }

    loopProcess();
  }
}
