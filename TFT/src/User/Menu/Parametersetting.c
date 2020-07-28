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
          v = numPadFloat(NULL, v, negative_val); // parameter is a decimal number
        else
          v = (float)numPadInt(NULL, v, negative_val); // parameter is an integer

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
        ps_cur_page = 0;
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

  //global nozzle
  lcd_frame_display(ICON_NOZZLE_X, 0, GLOBALICON_WIDTH, GLOBALICON_HEIGHT, ICON_ADDR(ICON_GLOBAL_NOZZLE));
  sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(NOZZLE0), heatGetTargetTemp(NOZZLE0));
  GUI_DispString(VALUE_NOZZLE_X, 0, (u8 *)tempstr);

  //global bed
  lcd_frame_display(ICON_BED_X, 0, GLOBALICON_WIDTH, GLOBALICON_HEIGHT, ICON_ADDR(ICON_GLOBAL_BED));
  sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
  GUI_DispString(VALUE_BED_X, 0, (u8 *)tempstr);

  GUI_SetBkColor(infoSettings.bg_color);
}
