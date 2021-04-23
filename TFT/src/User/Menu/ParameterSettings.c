#include "ParameterSettings.h"
#include "includes.h"

static uint16_t ps_cur_page = 0;
static uint8_t cur_parameter = 0;

bool parametersChanged = false;

const LABEL parameterTypes[PARAMETERS_COUNT] = {
  LABEL_STEPS_SETTING,
  LABEL_MAXFEEDRATE,
  LABEL_MAXACCELERATION,
  LABEL_ACCELERATION,
  LABEL_JERK,
  LABEL_JUNCTION_DEVIATION,
  LABEL_HOME_OFFSET,
  LABEL_FWRETRACT,
  LABEL_FWRECOVER,
  LABEL_RETRACT_AUTO,
  LABEL_HOTEND_OFFSET,
  LABEL_ABL,
  LABEL_PROBE_OFFSET,
  LABEL_LIN_ADVANCE,
  LABEL_FILAMENT_SETTING,
  LABEL_CURRENT_SETTING,
  LABEL_BUMP_SENSITIVITY,
  LABEL_HYBRID_THRESHOLD,
  LABEL_STEALTH_CHOP,
  LABEL_MBL_OFFSET,
};

const LISTITEM eepromItems[P_SETTINGS_COUNT] = {
  // icon           ItemType   Item Title              item value text(only for custom value)
  {CHARICON_SAVE,  LIST_LABEL, LABEL_SETTINGS_SAVE,    LABEL_BACKGROUND},
  {CHARICON_UNDO,  LIST_LABEL, LABEL_SETTINGS_RESTORE, LABEL_BACKGROUND},
  {CHARICON_RESET, LIST_LABEL, LABEL_SETTINGS_RESET,   LABEL_BACKGROUND},
};

//show menu for selected parameter type
void menuShowParameter(void)
{
  uint16_t curIndex = KEY_IDLE;
  PARAMETERS now = infoParameters;
  float oldval[LISTITEM_PER_PAGE];

  LISTITEM parameter_menuitems[] = {
  // icon                   ItemType            Item Title          item value text(only for custom value)
    {CHARICON_BACKGROUND,   LIST_CUSTOMVALUE,   LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
    {CHARICON_BACKGROUND,   LIST_CUSTOMVALUE,   LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
    {CHARICON_BACKGROUND,   LIST_CUSTOMVALUE,   LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
    {CHARICON_BACKGROUND,   LIST_CUSTOMVALUE,   LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
    {CHARICON_BACKGROUND,   LIST_CUSTOMVALUE,   LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
  };

  for (uint8_t i = 0; i < getParameterElementCount(cur_parameter); i++)
  {
    parameter_menuitems[i].titlelabel.address = axisDisplayID[i];
    setDynamicValue(i, getParameter(cur_parameter, i));

    if (i < E2_STEPPER)
      parameter_menuitems[i].icon = CHARICON_EDIT;

    //load parameter specific labels
    switch (cur_parameter)
    {
      case P_ACCELERATION:
        parameter_menuitems[i].titlelabel = accelDisplayID[i];
        break;

      case P_JERK:
        parameter_menuitems[X_AXIS].titlelabel.address = "X";
        parameter_menuitems[Y_AXIS].titlelabel.address = "Y";
        parameter_menuitems[Z_AXIS].titlelabel.address = "Z";
        parameter_menuitems[E_AXIS].titlelabel.address = "E";
        break;

      case P_JUNCTION_DEVIATION:
        parameter_menuitems[i].titlelabel = junctionDeviationDisplayID[i];
        break;

      case P_FWRETRACT:
        parameter_menuitems[i].titlelabel = retractDisplayID[i];
        break;

      case P_FWRECOVER:
        parameter_menuitems[i].titlelabel = recoverDisplayID[i];
        break;

      case P_AUTO_RETRACT:
        parameter_menuitems[i].titlelabel.index = LABEL_RETRACT_AUTO;
        break;

      case P_HOTEND_OFFSET:
        parameter_menuitems[X_AXIS].titlelabel.address = "X";
        parameter_menuitems[Y_AXIS].titlelabel.address = "Y";
        parameter_menuitems[Z_AXIS].titlelabel.address = "Z";
        break;

      case P_ABL_STATE:
        parameter_menuitems[0].titlelabel.address = "S 1=ON 0=OFF";
        parameter_menuitems[1].titlelabel.address = "Z fade";
        break;

      case P_LIN_ADV:
        parameter_menuitems[0].titlelabel.address = "K-E";
        parameter_menuitems[1].titlelabel.address = "K-E2";
        break;

      case P_FILAMENT_SETTING:
        parameter_menuitems[0].titlelabel.address = "S 1=ON 0=OFF";
        parameter_menuitems[1].titlelabel.address = "T0 Ø Filament";
        parameter_menuitems[2].titlelabel.address = "T1 Ø Filament";
        break;

      case P_STEALTH_CHOP:
        parameter_menuitems[X_STEPPER].titlelabel.address = "X 1=ON 0=OFF";
        parameter_menuitems[Y_STEPPER].titlelabel.address = "Y 1=ON 0=OFF";
        parameter_menuitems[Z_STEPPER].titlelabel.address = "Z 1=ON 0=OFF";
        parameter_menuitems[E_STEPPER].titlelabel.address = "E 1=ON 0=OFF";
        parameter_menuitems[E2_STEPPER].titlelabel.address = "E2 1=ON 0=OFF";
        break;

      case P_MBL_OFFSET:
        parameter_menuitems[i].titlelabel.address = "Z";
        break;

      default:
        if (getDualStepperStatus(E_STEPPER) && i == E2_STEPPER)
        {
          parameter_menuitems[i].icon = CHARICON_EDIT;
        }
        break;
    }
  }

  listViewCreate(parameterTypes[cur_parameter], parameter_menuitems, getParameterElementCount(cur_parameter), NULL,
                 false, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuShowParameter)
  {
    curIndex = listViewGetSelectedIndex();

    switch (curIndex)
    {
      case KEY_BACK:
        if (memcmp(&now, &infoParameters, sizeof(PARAMETERS)))
        {
          parametersChanged = true;
        }
        infoMenu.cur--;
        break;

      default:
        if (curIndex < (KEY_VALUES)STEPPER_COUNT)
        {
          VAL_TYPE val_type = getParameterValType(cur_parameter, curIndex);
          bool negative_val = val_type % 2;  //accept negative values only for probe offset
          float val = getParameter(cur_parameter, curIndex);

          if (val_type == VAL_TYPE_FLOAT || val_type == VAL_TYPE_NEG_FLOAT)
            val = numPadFloat(NULL, val, val, negative_val);  // parameter is a decimal number
          else
            val = (float)numPadInt(NULL, val, val, negative_val);  // parameter is an integer

          if (val != getParameter(cur_parameter, curIndex))
          {
            sendParameterCmd(cur_parameter, curIndex, val);
          }

          setDynamicValue(curIndex, val);
          listViewRefreshMenu();
        }
        break;
    }

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
      if (oldval[i] != getParameter(cur_parameter, i))
      {
        oldval[i] = getParameter(cur_parameter, i);
        setDynamicValue(i, getParameter(cur_parameter, i));
        listViewRefreshItem(curIndex);
      }
    }

    loopProcess();
  }
}

//Load main parameter list page
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
      parameterMainItem->icon = CHARICON_BACKGROUND;
    }
  }
  else
  {
    if (infoMachineSettings.EEPROM == 1 && index < totalItems)
      *parameterMainItem = eepromItems[(index - enabledParameterCount)];
    else
      parameterMainItem->icon = CHARICON_BACKGROUND;
  }
}

void menuParameterSettings(void)
{
  uint8_t enabledParameterCount = getEnabledParameterCount();
  uint8_t totalItems = (infoMachineSettings.EEPROM == 1) ? (enabledParameterCount + P_SETTINGS_COUNT): enabledParameterCount;

  LABEL title = {LABEL_PARAMETER_SETTING};
  //LISTITEM parameterMainItems[totalItems];
  uint16_t curIndex = KEY_IDLE;

  listViewCreate(title, NULL, totalItems, &ps_cur_page, false, NULL, loadParameters);

  while (infoMenu.menu[infoMenu.cur] == menuParameterSettings)
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
          ps_cur_page = 0;
          infoMenu.cur--;
        }
        break;

      default:

        if (curIndex < enabledParameterCount)
        {
          cur_parameter = getEnabledParameter(curIndex);
          if (cur_parameter < PARAMETERS_COUNT)
          {
            mustStoreCmd("M503 S0\n");
            infoMenu.menu[++infoMenu.cur] = menuShowParameter;
          }
          break;
        }
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

bool temperatureStatusValid(void)
{
  if (infoSettings.persistent_info != 1) return false;
  if (infoHost.connected == false) return false;
  if (toastRunning()) return false;

  if (infoMenu.menu[infoMenu.cur] == menuPrinting) return false;
  if (infoMenu.menu[infoMenu.cur] == menuStatus) return false;
  if (infoMenu.menu[infoMenu.cur] == menuMove) return false;
  if (infoMenu.menu[infoMenu.cur] == menuInfo) return false;
  if (infoMenu.menu[infoMenu.cur] == menuNotification) return false;

  return true;
}

void loopTemperatureStatus(void)
{
  if (getMenuType() == MENU_TYPE_FULLSCREEN) return;
  if (!temperatureStatusValid()) return;

  uint8_t tmpHeater[3];  // chamber, bed, hotend
  uint8_t tmpIndex = 0;

  if (infoSettings.hotend_count)  // global hotend
    tmpHeater[tmpIndex++] = heatGetCurrentHotend();

  if (infoSettings.bed_en)  // global bed
    tmpHeater[tmpIndex++] = BED;

  if (infoSettings.chamber_en)  // global chamber
    tmpHeater[tmpIndex++] = CHAMBER;

  bool update = false;
  static int16_t lastCurrent[3];
  static int16_t lastTarget[3];

  for (int8_t i = tmpIndex - 1; i >= 0; i--)
  {
    int16_t actCurrent = heatGetCurrentTemp(tmpHeater[i]);
    int16_t actTarget = heatGetTargetTemp(tmpHeater[i]);

    if (lastCurrent[i] != actCurrent || lastTarget[i] != actTarget)
    {
      lastCurrent[i] = actCurrent;
      lastTarget[i] = actTarget;
      update = true;
    }
  }

  if (update) menuReDrawCurTitle();
}

int16_t drawTemperatureStatus(void)
{
  int16_t x_offset = LCD_WIDTH - 10;

  if (!temperatureStatusValid()) return x_offset;

  uint8_t tmpHeater[3];  // chamber, bed, 1-2hotend
  uint16_t tmpIcon[3];
  uint8_t tmpIndex = 0;

  if (infoSettings.hotend_count)
  { // global hotend
    if (infoSettings.hotend_count == 2 && !infoSettings.chamber_en )  // dual hotend
    {
      tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
      tmpHeater[tmpIndex++] = NOZZLE0;
      tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
      tmpHeater[tmpIndex++] = NOZZLE1;
    }
    else  // singl or mixing hotend
    {
      tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
      tmpHeater[tmpIndex++] = heatGetCurrentHotend();
    }
  }

  if (infoSettings.bed_en)
  { // global bed
    tmpIcon[tmpIndex] = ICON_GLOBAL_BED;
    tmpHeater[tmpIndex++] = BED;
  }

  if (infoSettings.chamber_en)
  { // global chamber
    tmpIcon[tmpIndex] = ICON_GLOBAL_CHAMBER;
    tmpHeater[tmpIndex++] = CHAMBER;
  }

  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;

  GUI_SetBkColor(infoSettings.title_bg_color);

  for (int8_t i = tmpIndex - 1; i >= 0; i--)
  {
    char tempstr[10];

    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);
    sprintf(tempstr, "%d/%d", heatGetCurrentTemp(tmpHeater[i]), heatGetTargetTemp(tmpHeater[i]));

    x_offset -= GUI_StrPixelWidth((uint8_t *)tempstr);
    GUI_StrPixelWidth(LABEL_10_PERCENT);

    GUI_DispString(x_offset, start_y, (uint8_t *)tempstr);  // value
    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);
    x_offset -= GLOBALICON_WIDTH;
    ICON_ReadDisplay(x_offset, start_y, tmpIcon[i]);  // icon
  }

  GUI_SetBkColor(infoSettings.bg_color);

  return x_offset;
}
