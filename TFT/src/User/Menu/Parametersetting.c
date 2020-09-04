#include "Parametersetting.h"
#include "includes.h"

static u8 ps_cur_page = 0;
static u8 cur_parameter = 0;
static u8 total_pages = 1;

bool parametersChanged = false;

const LISTITEM parametertypes[P_ITEMSCOUNT] = {
  // icon               ItemType          Item Title              item value text(only for custom value)
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_STEPS_SETTING,    LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_CURRENT_SETTING,  LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_MAXFEEDRATE,      LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_MAXACCELERATION,  LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_ACCELERATION,     LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_JERK,             LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_PROBE_OFFSET,     LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_BUMP_SENSITIVITY, LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_FWRETRACT,        LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_FWRECOVER,        LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_RETRACT_AUTO,     LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_LIN_ADVANCE,      LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_ABL,              LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_OFFSET_TOOL,      LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_HYBRID_THRESHOLD, LABEL_BACKGROUND},
  //Keep below items always at the end
  {ICONCHAR_SAVE,       LIST_LABEL,       LABEL_SETTING_SAVE,     LABEL_BACKGROUND},
  {ICONCHAR_UNDO,       LIST_LABEL,       LABEL_SETTING_RESTORE,  LABEL_BACKGROUND},
  {ICONCHAR_RESET,      LIST_LABEL,       LABEL_SETTING_RESET,    LABEL_BACKGROUND},
};

LISTITEMS parameterMainItems = {
// title
LABEL_PARAMETER_SETTING,
// icon           ItemType     Item Title        item value text(only for custom value)
{
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,   LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},
  {ICONCHAR_BACK,         LIST_LABEL,     LABEL_BACKGROUND,     LABEL_BACKGROUND},}
};

//show menu for selected parameter type
void menuShowParameter(void){
  KEY_VALUES key_num = KEY_IDLE;
  PARAMETERS now = infoParameters;

  LISTITEMS parameter_menuitems ={
  // title
  parametertypes[cur_parameter].titlelabel,
  // icon           ItemType        Item Title      item value text(only for custom value)
  {{ICONCHAR_BACKGROUND,      LIST_CUSTOMVALUE,     LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
  {ICONCHAR_BACKGROUND,       LIST_CUSTOMVALUE,     LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
  {ICONCHAR_BACKGROUND,       LIST_CUSTOMVALUE,     LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
  {ICONCHAR_BACKGROUND,       LIST_CUSTOMVALUE,     LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
  {ICONCHAR_BACKGROUND,       LIST_CUSTOMVALUE,     LABEL_DYNAMIC,      LABEL_CUSTOM_VALUE},
  {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,   LABEL_BACKGROUND},
  {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,   LABEL_BACKGROUND},
  {ICONCHAR_BACK,             LIST_LABEL,           LABEL_BACKGROUND,   LABEL_BACKGROUND},}
  };

  for (int i = 0; i < getParameterElementCount(cur_parameter); i++) {
    setDynamicLabel(i, axisDisplayID[i]);
    setDynamicValue(i, getParameter(cur_parameter,i));

    if (i < E2_STEPPER)
      parameter_menuitems.items[i].icon = ICONCHAR_EDIT;

    //load parameter specific labels
    switch (cur_parameter)
    {
    case P_ACCELERATION:
      parameter_menuitems.items[i].titlelabel = accel_disp_ID[i];
      break;
    case P_JERK:
      setDynamicLabel(X_AXIS, "X");
      setDynamicLabel(Y_AXIS, "Y");
      setDynamicLabel(Z_AXIS, "Z");
      setDynamicLabel(E_AXIS, "E");
      break;
    case P_FWRETRACT:
      parameter_menuitems.items[i].titlelabel = retract_disp_ID[i];
      break;
    case P_FWRECOVER:
      parameter_menuitems.items[i].titlelabel = recover_disp_ID[i];
      break;
    case P_AUTO_RETRACT:
      parameter_menuitems.items[i].titlelabel = retract_auto_ID[i];
      break;
    case P_LIN_ADV:
      setDynamicLabel(0, "K-E");
      setDynamicLabel(1, "K-E2");
      break;
    case P_ABL_STATE:
      setDynamicLabel(0, "S 1=ON 0=OFF");
      setDynamicLabel(1, "Z fade");
      break;
    case P_OFFSET_TOOL:
      setDynamicLabel(0, "X");
      setDynamicLabel(1, "Y");
      setDynamicLabel(2, "Z");
      break;
    default:
      if (getDualstepperStatus(E_STEPPER) && i == E2_STEPPER)
      {
        parameter_menuitems.items[i].icon = ICONCHAR_EDIT;
      }
      break;
    }
  }

  menuDrawListPage(&parameter_menuitems);

  while (infoMenu.menu[infoMenu.cur] == menuShowParameter)
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
    case KEY_ICON_7:
      if (memcmp(&now, &infoParameters, sizeof(PARAMETERS)))
      {
        parametersChanged = true;
      }
      infoMenu.cur--;
      break;

    default:
      if (key_num < (KEY_VALUES)STEPPER_COUNT)
      {
        if (parameter_menuitems.items[key_num].icon == ICONCHAR_BACKGROUND)
          break;

        VAL_TYPE v_type = getParameterValType(cur_parameter, key_num);
        //accept negative values only for probe offset
        bool negative_val = v_type % 2;

        float v = getParameter(cur_parameter, key_num);

        if (v_type == VAL_TYPE_FLOAT || v_type == VAL_TYPE_NEG_FLOAT)
          v = numPadFloat(NULL, v, v, negative_val); // parameter is a decimal number
        else
          v = (float)numPadInt(NULL, v, v, negative_val); // parameter is an integer

        if (v != getParameter(cur_parameter, key_num))
        {
          sendParameterCmd(cur_parameter,key_num, v);
        }
        setDynamicValue(key_num, v);
        menuDrawListPage(&parameter_menuitems);
      }
      break;
    }

    for (int i = 0; i < STEPPER_COUNT; i++)
    {
      if (getDynamicValue(i) != getParameter(cur_parameter, i))
      {
        setDynamicValue(i, getParameter(cur_parameter, i));
        menuDrawListItem(&parameter_menuitems.items[i], i);
      }
    }
  loopProcess();
  }

}

//Load main parameter list page
void loadParameterPage(void){
for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++)
  {
    uint8_t item_index = ps_cur_page * LISTITEM_PER_PAGE + i;
    if (item_index < P_ITEMSCOUNT)
    {
      if (infoMachineSettings.EEPROM != 1 && (item_index == P_RESET_SETTINGS || item_index == P_RESTORE_SETTINGS || item_index == P_SAVE_SETTINGS))
        parameterMainItems.items[i].icon = ICONCHAR_BACKGROUND;
      else
        parameterMainItems.items[i] = parametertypes[item_index];
    }
    else
    {
      parameterMainItems.items[i].icon = ICONCHAR_BACKGROUND;
    }
  }
// set page up down button according to page count and current page
  if (P_ITEMSCOUNT <= LISTITEM_PER_PAGE)
  {
    parameterMainItems.items[5].icon = ICONCHAR_BACKGROUND;
    parameterMainItems.items[6].icon = ICONCHAR_BACKGROUND;
  }
  else
  {
    if(ps_cur_page == 0){
      parameterMainItems.items[5].icon = ICONCHAR_BACKGROUND;
      parameterMainItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
    else if(ps_cur_page == (total_pages-1)){
      parameterMainItems.items[5].icon = ICONCHAR_PAGEUP;
      parameterMainItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      parameterMainItems.items[5].icon = ICONCHAR_PAGEUP;
      parameterMainItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
}

void menuParameterSettings(void){
  KEY_VALUES key_num = KEY_IDLE;

  if (infoMachineSettings.EEPROM != 1)
    total_pages = (P_SAVE_SETTINGS+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE;
  else
    total_pages = (P_ITEMSCOUNT+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE;

  loadParameterPage();
  menuDrawListPage(&parameterMainItems);
  while (infoMenu.menu[infoMenu.cur] == menuParameterSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_5:
      if(total_pages > 1)
      {
        if (ps_cur_page > 0)
        {
          ps_cur_page--;
          loadParameterPage();
          menuRefreshListPage();
        }
      }
      break;

    case KEY_ICON_6:
      if(total_pages > 1)
      {
        if (ps_cur_page < total_pages - 1)
        {
          ps_cur_page++;
          loadParameterPage();
          menuRefreshListPage();
        }
      }
      break;

    case KEY_ICON_7:
      if (parametersChanged && infoMachineSettings.EEPROM == 1)
      {
        showDialog(DIALOG_TYPE_QUESTION, textSelect(parameterMainItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL) , saveEepromSettings, NULL, NULL);
        parametersChanged = false;
      }
      else
      {
        ps_cur_page = 0;
        infoMenu.cur--;
      }
      break;

    default:
    {
      int cp = ps_cur_page * LISTITEM_PER_PAGE + key_num;

      if (infoMachineSettings.EEPROM == 1)
      {
        if (cp == P_SAVE_SETTINGS)
        {
          showDialog(DIALOG_TYPE_ALERT, textSelect(parameterMainItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL) , saveEepromSettings, NULL, NULL);
          parametersChanged = false;
          break;
        }
        else if (cp == P_RESET_SETTINGS)
        {
          showDialog(DIALOG_TYPE_ALERT, textSelect(LABEL_SETTING_RESET), textSelect(LABEL_RESET_SETTINGS_INFO),
                      textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), resetEepromSettings, NULL, NULL);
          break;
        }
        else if (cp == P_RESTORE_SETTINGS)
        {
          showDialog(DIALOG_TYPE_ALERT, textSelect(LABEL_SETTING_RESTORE), textSelect(LABEL_EEPROM_RESTORE_INFO),
                      textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), restoreEepromSettings, NULL, NULL);
          break;
        }
      }
      if (key_num < LISTITEM_PER_PAGE && cp < PARAMETERS_COUNT)
      {
        cur_parameter = cp;
        mustStoreCmd("M503 S0\n");
        infoMenu.menu[++infoMenu.cur] = menuShowParameter;
        break;
      }
    }
    break;
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

  return true;
}

void loopTemperatureStatus(void)
{
  if (!temperatureStatusValid()) return;

  uint8_t tmpHeater[3]; // chamber, bed, hotend
  uint8_t tmpIndex = 0;
  if (infoSettings.hotend_count) { // global hotend
    tmpHeater[tmpIndex++] = heatGetCurrentHotend();
  }
  if (infoSettings.bed_en) { // global bed
    tmpHeater[tmpIndex++] = BED;
  }
  if (infoSettings.chamber_en) { // global chamber
    tmpHeater[tmpIndex++] = CHAMBER;
  }

  bool update = false;
  static int16_t lastCurrent[3];
  static int16_t lastTarget[3];
  for(int8_t i = tmpIndex - 1; i >= 0; i--) {
    int16_t actCurrent = heatGetCurrentTemp(tmpHeater[i]);
    int16_t actTarget = heatGetTargetTemp(tmpHeater[i]);
    if (lastCurrent[i] != actCurrent || lastTarget[i] != actTarget) {
      lastCurrent[i] = actCurrent;
      lastTarget[i] = actTarget;
      update = true;
    }
  }
  if (update) menuReDrawCurTitle();
}

int16_t drawTemperatureStatus(void){

  int16_t x_offset = LCD_WIDTH - 10;
  if (!temperatureStatusValid()) return x_offset;

  uint8_t tmpHeater[3]; // chamber, bed, hotend
  uint16_t tmpIcon[3];
  uint8_t tmpIndex = 0;
  if (infoSettings.hotend_count) { // global hotend
    tmpIcon[tmpIndex] = ICON_GLOBAL_NOZZLE;
    tmpHeater[tmpIndex++] = heatGetCurrentHotend();
  }
  if (infoSettings.bed_en) { // global bed
    tmpIcon[tmpIndex] = ICON_GLOBAL_BED;
    tmpHeater[tmpIndex++] = BED;
  }
  if (infoSettings.chamber_en) { // global chamber
    tmpIcon[tmpIndex] = ICON_GLOBAL_CHAMBER;
    tmpHeater[tmpIndex++] = CHAMBER;
  }

  uint16_t start_y = (TITLE_END_Y - BYTE_HEIGHT) / 2;
  GUI_SetBkColor(infoSettings.title_bg_color);
  for(int8_t i = tmpIndex - 1; i >= 0; i--) {
    char tempstr[10];
    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);
    sprintf(tempstr, "%d/%d", heatGetCurrentTemp(tmpHeater[i]), heatGetTargetTemp(tmpHeater[i]));
    x_offset -= GUI_StrPixelWidth((uint8_t *)tempstr);
    GUI_DispString(x_offset, start_y, (u8 *)tempstr); // value
    x_offset -= GLOBALICON_INTERVAL;
    GUI_ClearRect(x_offset, start_y, x_offset + GLOBALICON_INTERVAL, start_y + GLOBALICON_HEIGHT);
    x_offset -= GLOBALICON_WIDTH;
    lcd_frame_display(x_offset, start_y, GLOBALICON_WIDTH, GLOBALICON_HEIGHT, ICON_ADDR(tmpIcon[i])); // icon
  }
  GUI_SetBkColor(infoSettings.bg_color);
  return x_offset;
}
