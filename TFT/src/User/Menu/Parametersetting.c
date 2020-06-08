#include "Parametersetting.h"
#include "includes.h"

const u8 parameter_element_count[PARAMETERS_COUNT] = {5, 5, 5, 5, 3, 3, 3, 4, 4, 1};

const char *const parameter_Cmd[PARAMETERS_COUNT][STEPPER_COUNT] = {
  {"M92 X%.2f\n",   "M92 Y%.2f\n",  "M92 Z%.2f\n",  "M92 T0 E%.2f\n",  "M92 T1 E%.2f\n"}, //Steps/mm
  {"M906 X%.0f\n", "M906 Y%.0f\n", "M906 Z%.0f\n", "M906 T0 E%.0f\n", "M906 T1 E%.0f\n"}, //Current
  {"M203 X%.0f\n", "M203 Y%.0f\n", "M203 Z%.0f\n", "M203 T0 E%.0f\n", "M203 T1 E%.0f\n"}, //MaxFeedrate
  {"M201 X%.0f\n", "M201 Y%.0f\n", "M201 Z%.0f\n", "M201 T0 E%.0f\n", "M201 T1 E%.0f\n"}, //MaxAcceleration
  {"M204 P%.0f\n", "M204 R%.0f\n", "M204 T%.0f\n",              NULL,              NULL}, //Acceleration
  {"M851 X%.2f\n", "M851 Y%.2f\n", "M851 Z%.2f\n",              NULL,              NULL}, //Probe offset
  {"M914 X%.2f\n", "M914 Y%.2f\n", "M914 Z%.2f\n",              NULL,              NULL}, //bump Sensitivity
  {"M207 S%.0f\n", "M207 W%.2f\n", "M207 F%.2f\n",    "M207 Z%.2f\n",              NULL}, //FW retract
  {"M208 S%.0f\n", "M208 W%.0f\n", "M208 F%.2f\n",    "M208 R%.2f\n",              NULL}, //FW retract recover
  {"M900 K%.2f\n",           NULL,            NULL,             NULL,              NULL}  //Linear Advance
};

const VAL_TYPE parameter_val_type[PARAMETERS_COUNT][STEPPER_COUNT] = {
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT,       VAL_TYPE_FLOAT,       VAL_TYPE_FLOAT},  //Steps/mm
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT,         VAL_TYPE_INT},    //Current
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT,         VAL_TYPE_INT},    //MaxFeedrate
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT,         VAL_TYPE_INT},    //MaxAcceleration
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT},                          //Acceleration
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                          //Probe offset
  {VAL_TYPE_NEG_INT,    VAL_TYPE_NEG_INT,   VAL_TYPE_NEG_INT},                                            //bump Sensitivity
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT,       VAL_TYPE_FLOAT},                        //FW retract
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_NEG_FLOAT,   VAL_TYPE_NEG_FLOAT},                    //FW retract recover
  {VAL_TYPE_FLOAT}                                                                                        //Linear Advance
};

//Extra teppers current gcode command
const char *const dualstepper_current_Cmd[3] = {"M906 I1 X%.1f\n", "M906 I1 Y%.1f\n", "M906 I1 Z%.1f\n"};
const char *const dualstepper_bump_Cmd[3] = {"M914 I1 X%.1f\n", "M914 I1 Y%.1f\n", "M914 I1 Z%.1f\n"};

static u8 ps_cur_page = 0;
static u8 cur_parameter = 0;
static u8 total_pages = 1;

bool parametersChanged = false;
bool dualstepper[TOTAL_AXIS] = {false,false,false,false};
char *const axisDisplayID[STEPPER_COUNT] = AXIS_DISPLAY_ID;

const LABEL accel_disp_ID[] = {LABEL_PRINT_ACCELERATION, LABEL_RETRACT_ACCELERATION, LABEL_TRAVEL_ACCELERATION};
const LABEL retract_disp_ID[] = {LABEL_RETRACT_LENGTH, LABEL_RETRACT_SWAP_LENGTH, LABEL_RETRACT_FEEDRATE, LABEL_RETRACT_Z_LIFT};
const LABEL recover_disp_ID[] = {LABEL_RECOVER_LENGTH, LABEL_SWAP_RECOVER_LENGTH, LABEL_RECOVER_FEEDRATE, LABEL_SWAP_RECOVER_FEEDRATE};

const LISTITEM parametertypes[P_ITEMSCOUNT] = {
  // icon               ItemType          Item Title              item value text(only for custom value)
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_STEPS_SETTING,    LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_CURRENT_SETTING,  LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_MAXFEEDRATE,      LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_MAXACCELERATION,  LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_ACCELERATION,     LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_PROBE_OFFSET,     LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_BUMP_SENSITIVITY, LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_FWRETRACT,        LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_FWRECOVER,        LABEL_BACKGROUND},
  {ICONCHAR_SETTING1,   LIST_MOREBUTTON,  LABEL_LIN_ADVANCE,      LABEL_BACKGROUND},
  //Keep below items always at the end
  {ICONCHAR_RESET,      LIST_LABEL,       LABEL_SETTING_RESET,    LABEL_BACKGROUND},
  {ICONCHAR_UNDO,       LIST_LABEL,       LABEL_SETTING_RESTORE,  LABEL_BACKGROUND},
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

PARAMETERS infoParameters;

float getParameter(PARAMETER_NAME name, int index)
{
  if(index >= parameter_element_count[name]) return 0.0f;
  switch (name)
  {
  case P_STEPS_PER_MM:
    return infoParameters.StepsPerMM[index];
  case P_CURRENT:
    return infoParameters.Current[index];
  case P_MAX_FEED_RATE:
    return infoParameters.MaxFeedRate[index];
  case P_MAX_ACCELERATION:
    return infoParameters.MaxAcceleration[index];
  case P_ACCELERATION:
    return infoParameters.Acceleration[index];
  case P_PROBE_OFFSET:
    return infoParameters.ProbeOffset[index];
  case P_BUMPSENSITIVITY:
    return infoParameters.BumpSensitivity[index];
  case P_FWRETRACT:
    return infoParameters.FwRetract[index];
  case P_FWRECOVER:
    return infoParameters.FwRecover[index];
  case P_LIN_ADV:
    return infoParameters.LinAdvance[index];
  default:
    return 0.0f;
  }
}

void setParameter(PARAMETER_NAME name, int index, float val){
  if(index >= parameter_element_count[name]) return;
    switch (name)
    {
    case P_STEPS_PER_MM:
      infoParameters.StepsPerMM[index] = val;
      break;
    case P_CURRENT:
      infoParameters.Current[index] = val;
      break;
    case P_MAX_FEED_RATE:
      infoParameters.MaxFeedRate[index]  = val;
      break;
    case P_MAX_ACCELERATION:
      infoParameters.MaxAcceleration[index] = val;
      break;
    case P_ACCELERATION:
      infoParameters.Acceleration[index] = val;
      break;
    case P_PROBE_OFFSET:
      infoParameters.ProbeOffset[index] = val;
      break;
    case P_BUMPSENSITIVITY:
      infoParameters.BumpSensitivity[index] = val;
      break;
    case P_FWRETRACT:
      infoParameters.FwRetract[index] = val;
      break;
    case P_FWRECOVER:
      infoParameters.FwRecover[index] = val;
      break;
    case P_LIN_ADV:
      infoParameters.LinAdvance[index] = val;
      break;
    default:
      break;
    }
}

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

  for (int i = 0; i < parameter_element_count[cur_parameter]; i++) {
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
    case P_FWRETRACT:
      parameter_menuitems.items[i].titlelabel = retract_disp_ID[i];
      break;
    case P_FWRECOVER:
      parameter_menuitems.items[i].titlelabel = recover_disp_ID[i];
      break;
    case P_LIN_ADV:
      setDynamicLabel(i, "K");
      break;

    default:
      if (dualstepper[E_STEPPER] == true && i == E2_STEPPER)
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
        //accept negative values only for probe offset
        bool negative_val = parameter_val_type[cur_parameter][key_num] % 2;

        float v = getParameter(cur_parameter, key_num);

        if (parameter_val_type[cur_parameter][key_num] == VAL_TYPE_FLOAT || parameter_val_type[cur_parameter][key_num] == VAL_TYPE_NEG_FLOAT)
          v = numPadFloat(NULL, v, negative_val); // parameter is a decimal number
        else
          v = (float)numPadInt(NULL, v, negative_val); // parameter is an integer

        if (v != getParameter(cur_parameter, key_num))
        {
          storeCmd(parameter_Cmd[cur_parameter][key_num], v);
          //send gcode for dual steppers(x,y & z only) if they exists
          if (dualstepper[key_num] == true)
          {
            if (cur_parameter == P_CURRENT && key_num < (KEY_VALUES)E_STEPPER)
                storeCmd(dualstepper_current_Cmd[key_num], v);
            else if (cur_parameter == P_BUMPSENSITIVITY && key_num < (KEY_VALUES)E_STEPPER)
                storeCmd(dualstepper_bump_Cmd[key_num], v);
          }
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
      if (infoMachineSettings.EEPROM != 1 && (item_index == P_RESET_SETTINGS || item_index == P_RESTORE_SETTINGS))
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
  ps_cur_page = 0;

  if (infoMachineSettings.EEPROM != 1)
    total_pages = (P_RESET_SETTINGS+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE;
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
        infoMenu.cur--;
      }
      break;

    default:
    {
      int cp = ps_cur_page * LISTITEM_PER_PAGE + key_num;

      if (infoMachineSettings.EEPROM == 1)
      {
        if (cp == P_RESET_SETTINGS)
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
        infoMenu.menu[++infoMenu.cur] = menuShowParameter;
        break;
      }
    }
    break;
    }
  loopProcess();
  }

}



void temp_Change(void)
{
  if(infoSettings.persistent_info != 1) return;
  //static FP_MENU NUM[MAX_MENU_DEPTH];
  static int16_t compare [2];

  if(infoHost.connected == false || infoMenu.menu[infoMenu.cur] == menuPrinting)  return;
  if(infoMenu.menu[infoMenu.cur] == menuMove || infoMenu.menu[infoMenu.cur] == menuStatus) return;

  if(heatGetCurrentTemp(NOZZLE0) != compare[0] || heatGetCurrentTemp(BED) != compare[1] )
  //|| strcmp((char *)infoMenu.menu[infoMenu.cur],(char *)NUM)!=0)
  {
    //strcpy((char *)NUM ,(char *)infoMenu.menu[infoMenu.cur]);
    compare[0] = heatGetCurrentTemp(NOZZLE0);
    compare[1] = heatGetCurrentTemp(BED);

    drawGlobalInfo();
  }

  return ;
}

void show_GlobalInfo(void)
{
  if(infoSettings.persistent_info != 1) return;
  if(infoHost.connected == false || infoMenu.menu[infoMenu.cur] == menuPrinting)  return;
  if(infoMenu.menu[infoMenu.cur] == menuMove || infoMenu.menu[infoMenu.cur] == menuStatus) return;
  drawGlobalInfo();

  return;
}

void drawGlobalInfo(void){
  char tempstr[10];
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_ClearRect(LCD_WIDTH/3, 0, LCD_WIDTH, BYTE_HEIGHT);

  //global nozzle
  lcd_frame_display(ICON_NOZZLE_X, 0, 2*BYTE_WIDTH, BYTE_HEIGHT, ICON_ADDR(ICON_GLOBAL_NOZZLE));
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(NOZZLE0), heatGetTargetTemp(NOZZLE0));
  GUI_DispStringInRect(VALUE_NOZZLE_X,0,VALUE_NOZZLE_X+8*BYTE_WIDTH,BYTE_HEIGHT, (u8 *)tempstr);

  //global bed
  lcd_frame_display(ICON_BED_X, 0, 2*BYTE_WIDTH, BYTE_HEIGHT, ICON_ADDR(ICON_GLOBAL_BED));
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
  GUI_DispStringInRect(VALUE_BED_X,0,VALUE_BED_X+8*BYTE_WIDTH,BYTE_HEIGHT, (u8 *)tempstr);
  GUI_SetBkColor(infoSettings.bg_color);
}
