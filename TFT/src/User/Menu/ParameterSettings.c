#include "ParameterSettings.h"
#include "includes.h"

static uint16_t psCurPage = 0;
static uint8_t curParameter = 0;
bool parametersChanged = false;

const LABEL parameterTypes[PARAMETERS_COUNT] = {
  LABEL_STEPS_SETTING,
  LABEL_FILAMENT_SETTING,
  LABEL_MAXACCELERATION,
  LABEL_MAXFEEDRATE,
  LABEL_ACCELERATION,
  LABEL_JERK,
  LABEL_JUNCTION_DEVIATION,
  LABEL_HOME_OFFSET,
  LABEL_FWRETRACT,
  LABEL_FWRECOVER,
  LABEL_RETRACT_AUTO,
  LABEL_HOTEND_OFFSET,
  LABEL_HOTEND_PID,
  LABEL_BED_PID,
  LABEL_ABL,
  LABEL_STEALTH_CHOP,
  LABEL_DELTA_CONFIGURATION,
  LABEL_DELTA_TOWER_ANGLE,
  LABEL_DELTA_DIAGONAL_ROD,
  LABEL_DELTA_ENDSTOP,
  LABEL_PROBE_OFFSET,
  LABEL_LIN_ADVANCE,
  LABEL_CURRENT_SETTING,
  LABEL_HYBRID_THRESHOLD,
  LABEL_BUMP_SENSITIVITY,
  LABEL_MBL_OFFSET,
};

const LISTITEM eepromItems[P_SETTINGS_COUNT] = {
// icon            ItemType    Item Title              item value text(only for custom value)
  {CHARICON_SAVE,  LIST_LABEL, LABEL_SETTINGS_SAVE,    LABEL_NULL},
  {CHARICON_UNDO,  LIST_LABEL, LABEL_SETTINGS_RESTORE, LABEL_NULL},
  {CHARICON_RESET, LIST_LABEL, LABEL_SETTINGS_RESET,   LABEL_NULL},
};

// Load elements for selected parameter
void loadElements(LISTITEM * parameterMainItem, uint16_t index, uint8_t itemPos)
{
  uint8_t enabledElementCount = getEnabledElementCount(curParameter);

  if (index < enabledElementCount)
  {
    uint8_t elementIndex = getEnabledElement(curParameter, index);

    if (elementIndex < getElementCount(curParameter))
    {
      parameterMainItem->icon = CHARICON_EDIT;
      parameterMainItem->itemType = LIST_CUSTOMVALUE;
      parameterMainItem->titlelabel.index = LABEL_DYNAMIC;
      parameterMainItem->valueLabel.index = LABEL_CUSTOM_VALUE;
      parameterMainItem->titlelabel.address = axisDisplayID[elementIndex];

      // load value
      setDynamicValue(itemPos, getParameter(curParameter, elementIndex));

      // load parameter specific labels
      switch (curParameter)
      {
        case P_FILAMENT_DIAMETER:
          parameterMainItem->titlelabel.address = filamentDiaDisplayID[elementIndex];
          break;

        case P_ACCELERATION:
          parameterMainItem->titlelabel = accelDisplayID[elementIndex];
          break;

        case P_JUNCTION_DEVIATION:
          parameterMainItem->titlelabel = junctionDeviationDisplayID[elementIndex];
          break;

        case P_HOTEND_PID:
          parameterMainItem->titlelabel.address = hotendPidDisplayID[elementIndex];
          break;

        case P_BED_PID:
          parameterMainItem->titlelabel.address = bedPidDisplayID[elementIndex];
          break;

        case P_FWRETRACT:
          parameterMainItem->titlelabel = retractDisplayID[elementIndex];
          break;

        case P_FWRECOVER:
          parameterMainItem->titlelabel = recoverDisplayID[elementIndex];
          break;

        case P_AUTO_RETRACT:
          parameterMainItem->titlelabel.address = autoRetractDisplayID[elementIndex];
          break;

        case P_ABL_STATE:
          parameterMainItem->titlelabel.address = ablStateDisplayID[elementIndex];
          break;

        case P_STEALTH_CHOP:
          parameterMainItem->titlelabel.address = stealthChopDisplayID[elementIndex];
          break;

        case P_DELTA_CONFIGURATION:
          parameterMainItem->titlelabel.address = deltaConfigurationDisplayID[elementIndex];
          break;

        case P_DELTA_TOWER_ANGLE:
          parameterMainItem->titlelabel.address = deltaTowerAngleDisplayID[elementIndex];
          break;

        case P_DELTA_DIAGONAL_ROD:
          parameterMainItem->titlelabel.address = deltaDiagonalRodDisplayID[elementIndex];
          break;

        case P_DELTA_ENDSTOP:
          parameterMainItem->titlelabel.address = deltaEndstopDisplayID[elementIndex];
          break;

        case P_LIN_ADV:
          parameterMainItem->titlelabel.address = linAdvDisplayID[elementIndex];
          break;

        case P_CURRENT:
        case P_HYBRID_THRESHOLD:
        case P_BUMPSENSITIVITY:
          parameterMainItem->titlelabel.address = stepperDisplayID[elementIndex];
          break;

        case P_MBL_OFFSET:
          parameterMainItem->titlelabel.address = axisDisplayID[Z_AXIS];
          break;

        default:
          break;
      }
    }
    else
    {
      parameterMainItem->icon = CHARICON_NULL;
    }
  }
  else
  {
    parameterMainItem->icon = CHARICON_NULL;
  }
}

// Show menu for selected parameter type
void menuShowParameter(void)
{
  uint8_t enabledElementCount = getEnabledElementCount(curParameter);
  float oldval[enabledElementCount];
  uint16_t curIndex = KEY_IDLE;
  PARAMETERS now = infoParameters;

  for (uint8_t i = 0; i < enabledElementCount; i++)
  {
    oldval[i] = getParameter(curParameter, getEnabledElement(curParameter, i));
  }

  listViewCreate(parameterTypes[curParameter], NULL, enabledElementCount, NULL, false, NULL, loadElements);

  while (MENU_IS(menuShowParameter))
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case KEY_BACK:
        if (memcmp(&now, &infoParameters, sizeof(PARAMETERS)))
        {
          parametersChanged = true;
        }
        CLOSE_MENU();
        break;

      default:
      {
        uint8_t elementIndex = getEnabledElement(curParameter, curIndex);

        if (elementIndex < getElementCount(curParameter))
        {
          VAL_TYPE val_type = getParameterValType(curParameter, elementIndex);
          bool negative_val = val_type % 2;  // accept negative values only for val_types with negetive
          float val = getParameter(curParameter, elementIndex);

          if (val_type == VAL_TYPE_FLOAT || val_type == VAL_TYPE_NEG_FLOAT)
            val = numPadFloat(NULL, val, val, negative_val);  // parameter is a decimal number
          else
            val = (float)numPadInt(NULL, val, val, negative_val);  // parameter is an integer

          if (val != getParameter(curParameter, elementIndex))
            sendParameterCmd(curParameter, elementIndex, val);

          listViewRefreshMenu();
        }
        break;
      }
    }

    for (uint8_t i = 0; i < enabledElementCount; i++)
    {
      float newVal = getParameter(curParameter, getEnabledElement(curParameter, i));

      if (oldval[i] != newVal)
      {
        oldval[i] = newVal;
        listViewRefreshItem(i);
      }
    }

    loopProcess();
  }
}

// Load main parameter list page
void loadParameters(LISTITEM * parameterMainItem, uint16_t index, uint8_t itemPos)
{
  uint8_t enabledParameterCount = getEnabledParameterCount();
  uint8_t totalItems = (infoMachineSettings.EEPROM == 1) ? (enabledParameterCount + P_SETTINGS_COUNT) : enabledParameterCount;

  if (index < enabledParameterCount)
  {
    uint8_t parameterIndex = getEnabledParameter(index);

    if (parameterIndex < PARAMETERS_COUNT)
    {
      parameterMainItem->icon = CHARICON_SETTING1;
      parameterMainItem->itemType = LIST_MOREBUTTON;
      parameterMainItem->titlelabel = parameterTypes[parameterIndex];
    }
    else
    {
      parameterMainItem->icon = CHARICON_NULL;
    }
  }
  else
  {
    if (infoMachineSettings.EEPROM == 1 && index < totalItems)
      *parameterMainItem = eepromItems[(index - enabledParameterCount)];
    else
      parameterMainItem->icon = CHARICON_NULL;
  }
}

// Main parameter menu
void menuParameterSettings(void)
{
  uint8_t enabledParameterCount = getEnabledParameterCount();
  uint8_t totalItems = (infoMachineSettings.EEPROM == 1) ? (enabledParameterCount + P_SETTINGS_COUNT) : enabledParameterCount;
  uint16_t curIndex = KEY_IDLE;
  LABEL title = {LABEL_PARAMETER_SETTINGS};

  listViewCreate(title, NULL, totalItems, &psCurPage, false, NULL, loadParameters);

  while (MENU_IS(menuParameterSettings))
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case KEY_BACK:
        if (parametersChanged && infoMachineSettings.EEPROM == 1)
        {
          setDialogText(title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
          parametersChanged = false;
        }
        else
        {
          psCurPage = 0;
        }

        CLOSE_MENU();
        break;

      default:
        // open selected parameter
        if (curIndex < enabledParameterCount)
        {
          curParameter = getEnabledParameter(curIndex);
          if (curParameter < PARAMETERS_COUNT)
          {
            mustStoreCmd("M503 S0\n");
            OPEN_MENU(menuShowParameter);
          }
          break;
        }
        // perform EEPROM task
        else if (infoMachineSettings.EEPROM == 1 && curIndex < totalItems)
        {
          uint8_t curIndex_e = (curIndex - enabledParameterCount);
          if (curIndex_e == P_SAVE_SETTINGS)
          {
            setDialogText(title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, saveEepromSettings, NULL, NULL);
            parametersChanged = false;
            break;
          }
          else if (curIndex_e == P_RESET_SETTINGS)
          {
            setDialogText(LABEL_SETTINGS_RESET, LABEL_SETTINGS_RESET_INFO, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, resetEepromSettings, NULL, NULL);
            break;
          }
          else if (curIndex_e == P_RESTORE_SETTINGS)
          {
            setDialogText(LABEL_SETTINGS_RESTORE, LABEL_EEPROM_RESTORE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, restoreEepromSettings, NULL, NULL);
            break;
          }
        }
    }

    loopProcess();
  }
}
