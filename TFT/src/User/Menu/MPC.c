#include "MPC.h"
#include "includes.h"

#define DEF_HEATER_POWER      40
#define DEF_FIL_HEAT_CAPACITY 0.0046
#define MPC_PARAM_ITEMS_COUNT 2

typedef struct
{
  uint8_t heater_power;
  float fil_heat_capcity;
} MPC_PARAM;


typedef struct
{
  MPC_STATUS status;
  MPC_RESULT result;
} MPC_TUNING;

static uint8_t curTool_index = NOZZLE0;
static MPC_TUNING mpcTuning = {DONE, NO_RESULT};
static MPC_PARAM mpcParameter[MAX_HOTEND_COUNT] = {0};

void displayValues()
{
  char tmpStr[15];

  setFontSize(FONT_SIZE_LARGE);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tool_change[curTool_index]);
  setFontSize(FONT_SIZE_NORMAL);

  snprintf(tmpStr, 10, "P: %d W  ", mpcParameter[curTool_index].heater_power);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0 + 2 * BYTE_HEIGHT, (uint8_t *)tmpStr);

  sprintf(tmpStr, "H: %.5f J/mm", mpcParameter[curTool_index].fil_heat_capcity);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0 + 3 * BYTE_HEIGHT, (uint8_t *)tmpStr);
}

void menuSetMpcParam(void)
{
  LABEL title = {LABEL_PARAMETER_SETTINGS};
  uint16_t curIndex = KEY_IDLE;

  LISTITEM paramItems[MPC_PARAM_ITEMS_COUNT];

  for (uint8_t i = 0; i < MPC_PARAM_ITEMS_COUNT; i++)
  {
    paramItems[i].icon = CHARICON_EDIT;
    paramItems[i].itemType = LIST_CUSTOMVALUE;
    paramItems[i].titlelabel.address = (uint8_t*)(i == 0 ? "P: " : "H: ");
    paramItems[i].valueLabel.index = LABEL_CUSTOM_VALUE;
    setDynamicValue(i, (i == 0 ? mpcParameter[curTool_index].heater_power : mpcParameter[curTool_index].fil_heat_capcity));
  }

  listViewCreate(title, paramItems, MPC_PARAM_ITEMS_COUNT, NULL, false, NULL, NULL);

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
          tmpVal = numPadInt((uint8_t *)"1 ~ 255", mpcParameter[curTool_index].heater_power, DEF_HEATER_POWER, false);
          if(tmpVal < 1 || tmpVal > 255)
            BUZZER_PLAY(SOUND_ERROR);
          else
           break;
        } while (true);

        mpcParameter[curTool_index].heater_power = tmpVal;
        setDynamicValue(0, mpcParameter[curTool_index].heater_power);
        listViewRefreshMenu();
        break;
      }

      case KEY_ICON_1:
      {
        float tmpVal;

        do
        {
          tmpVal = numPadFloat((uint8_t *)"0.001 ~ 0.1", mpcParameter[curTool_index].fil_heat_capcity, DEF_FIL_HEAT_CAPACITY, false);
          if (tmpVal < 0.001 || tmpVal > 0.1)
            BUZZER_PLAY(SOUND_ERROR);
          else
            break;
        } while (true);

        mpcParameter[curTool_index].fil_heat_capcity = tmpVal;
        setDynamicValue(1, mpcParameter[curTool_index].fil_heat_capcity);
        listViewRefreshMenu();
        break;
      }

      case KEY_BACK:
        mustStoreCmd("M306 E%d P%d H%.5f\n", curTool_index, mpcParameter[curTool_index].heater_power, mpcParameter[curTool_index].fil_heat_capcity);
        CLOSE_MENU();
        break;

    }

    loopProcess();
  }
}

void mpcStart(void)
{
  if(storeCmd("%s\n", tool_change[curTool_index]))
  {
    if (storeCmd("M306 T\n"))
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

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&mpcItems);
  displayValues();

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
          displayValues();
          break;

        case KEY_INFOBOX:
        case KEY_ICON_5:
          OPEN_MENU(menuSetMpcParam);
          break;

        case KEY_ICON_6:
          setDialogText(mpcItems.title.index, LABEL_TUNE_START_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, mpcStart, NULL, NULL);
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
      LED_SetEventColor(&ledRed, false);  // set (neopixel) LED light to RED
      LCD_SET_KNOB_LED_IDLE(false);       // set infoSettings.knob_led_idle temporary to OFF
      mpcTuning.status = ONGOING;
    }
    else if (mpcTuning.status == ONGOING)
    {
      if (getMenuType() != MENU_TYPE_SPLASH)
      {
       setDialogText(LABEL_SCREEN_INFO, LABEL_BUSY, LABEL_NULL, LABEL_NULL);
       showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      }

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
              sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

              setDialogText(LABEL_MPC_TITLE, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
              showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
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
