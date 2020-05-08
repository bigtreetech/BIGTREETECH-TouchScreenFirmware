#include "Parametersetting.h"
#include "includes.h"

const char *const parameter_Cmd[PARAMETERS_COUNT][STEPPER_COUNT] = {
    {"M92 X%.2f\n",   "M92 Y%.2f\n",  "M92 Z%.2f\n",  "M92 T0 E%.2f\n",  "M92 T1 E%.2f\n"}, //Steps/mm
    {"M906 X%.0f\n", "M906 Y%.0f\n", "M906 Z%.0f\n", "M906 T0 E%.0f\n", "M906 I1 E%.0f\n"}, //Current
    {"M203 X%.0f\n", "M203 Y%.0f\n", "M203 Z%.0f\n", "M203 T0 E%.0f\n", "M203 T1 E%.0f\n"}, //MaxFeedrate
    {"M201 X%.0f\n", "M201 Y%.0f\n", "M201 Z%.0f\n", "M201 T0 E%.0f\n", "M201 T1 E%.0f\n"}, //MaxAcceleration
    {"M204 P%.0f\n", "M204 R%.0f\n", "M204 T%.0f\n", NULL, NULL}, //Acceleration
    {"M851 X%.2f\n", "M851 Y%.2f\n", "M851 Z%.2f\n", NULL, NULL}, //Probe offset
    {"M914 X%.2f\n", "M914 Y%.2f\n", "M914 Z%.2f\n", NULL, NULL}, //bump Sensitivity
};

//Extra teppers current gcode command
const char *const dualstepper_current_Cmd[3] = {"M906 I1 X%.1f\n", "M906 I1 Y%.1f\n", "M906 I1 Z%.1f\n"};
const char *const dualstepper_bump_Cmd[3] = {"M914 I1 X%.1f\n", "M914 I1 Y%.1f\n", "M914 I1 Z%.1f\n"};

const uint32_t parameter_names[PARAMETERS_COUNT] = {LABEL_STEPS_SETTING,
                                                    LABEL_CURRENT_SETTING,
                                                    LABEL_MAXFEEDRATE,
                                                    LABEL_MAXACCELERATION,
                                                    LABEL_ACCELERATION,
                                                    LABEL_PROBE_OFFSET,
                                                    LABEL_BUMP_SENSITIVITY
                                                    };

int ps_cur_page = 0;
int cur_parameter = 0;
bool dualstepper[TOTAL_AXIS] = {false,false,false,false};
char *const axisDisplayID[STEPPER_COUNT] = AXIS_DISPLAY_ID;
const LABEL Accel_disp_ID[3] = {LABEL_PRINT_ACCELERATION,LABEL_RETRACT_ACCELERATION, LABEL_TRAVEL_ACCELERATION};

const LISTITEM parametertypes[PARAMETERS_COUNT] = {
    // icon                 ItemType            Item Title                  item value text(only for custom value)
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_STEPS_SETTING,        LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_CURRENT_SETTING,      LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_MAXFEEDRATE,          LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_MAXACCELERATION,      LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_ACCELERATION,         LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_PROBE_OFFSET,         LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_BUMP_SENSITIVITY,     LABEL_BACKGROUND},
};

LISTITEMS parameterMainItems = {
// title
LABEL_PARAMETER_SETTING,
// icon                     ItemType         Item Title                item value text(only for custom value)
{
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACK,           LIST_LABEL,       LABEL_BACKGROUND,       LABEL_BACKGROUND},}
};

PARAMETERS infoParameters;

float getParameter(PARAMETER_NAME name, int index){
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
            if(index > Z_AXIS){ return 0.0f;}
            else{ return infoParameters.ProbeOffset[index]; }
        case P_BUMPSENSITIVITY:
            if(index > Z_AXIS){ return 0.0f;}
            else{ return infoParameters.BumpSensitivity[index]; }
        default:
            return 0.0f;
        }
}

void setParameter(PARAMETER_NAME name, int index, float val){
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
            if(index < E_AXIS){infoParameters.ProbeOffset[index] = val; }
            break;
        case P_BUMPSENSITIVITY:
            if(index < E_AXIS){infoParameters.BumpSensitivity[index] = val; }
            break;
        default:
            break;
        }
}

//show menu for selected parameter type
void menuShowParameter(void){
    KEY_VALUES key_num = KEY_IDLE;

    LISTITEMS parameter_menuitems ={
    // title
    parameter_names[cur_parameter],
    // icon                     ItemType                Item Title            item value text(only for custom value)
    {{ICONCHAR_EDIT,            LIST_CUSTOMVALUE,       LABEL_DYNAMIC,        LABEL_CUSTOM_VALUE},
    {ICONCHAR_EDIT,             LIST_CUSTOMVALUE,       LABEL_DYNAMIC,        LABEL_CUSTOM_VALUE},
    {ICONCHAR_EDIT,             LIST_CUSTOMVALUE,       LABEL_DYNAMIC,        LABEL_CUSTOM_VALUE},
    {ICONCHAR_EDIT,             LIST_CUSTOMVALUE,       LABEL_DYNAMIC,        LABEL_CUSTOM_VALUE},
    {ICONCHAR_BACKGROUND,       LIST_CUSTOMVALUE,       LABEL_DYNAMIC,        LABEL_CUSTOM_VALUE},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,             LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,             LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACK,             LIST_LABEL,             LABEL_BACKGROUND,     LABEL_BACKGROUND},}
    };

    for (int i = 0; i < STEPPER_COUNT; i++) {
        setDynamicLabel(i, axisDisplayID[i]);
        setDynamicValue(i, getParameter(cur_parameter,i));
        //prevent Extruder icon from showing when probe offset setting is opened
        switch (cur_parameter)
        {
        case P_PROBE_OFFSET:
        case P_BUMPSENSITIVITY:
            if(i > (Z_STEPPER)) parameter_menuitems.items[i].icon = ICONCHAR_BACKGROUND;
            break;

        case P_ACCELERATION:
            if(i > (Z_STEPPER)){ parameter_menuitems.items[i].icon = ICONCHAR_BACKGROUND;}
            else { parameter_menuitems.items[i].titlelabel = Accel_disp_ID[i];}
            break;

        default:
            if(dualstepper[E_STEPPER] == true && i == E2_STEPPER)
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
            infoMenu.cur--;
            break;

        default:
            if(key_num < (KEY_VALUES)STEPPER_COUNT){
                if(parameter_menuitems.items[key_num].icon == ICONCHAR_BACKGROUND)
                {
                    break;
                }
             //accept negative values only for probe offset
                bool negative_val = false;
                if(cur_parameter == P_PROBE_OFFSET || cur_parameter == P_BUMPSENSITIVITY) negative_val = true;
                float v = numPadFloat(getParameter(cur_parameter,key_num),negative_val);
                if (v != getParameter(cur_parameter,key_num))
                {
                    storeCmd(parameter_Cmd[cur_parameter][key_num],v);
                //send current setting gcode for dual steppers(x,y & z only) if they exists
                    if(cur_parameter == P_CURRENT && key_num < (KEY_VALUES)E_STEPPER)
                    {
                        if(dualstepper[key_num] == true){
                            storeCmd(dualstepper_current_Cmd[key_num],v);
                        }
                    }
                    else if(cur_parameter == P_BUMPSENSITIVITY && key_num < (KEY_VALUES)E_STEPPER)
                    {
                        if(dualstepper[key_num] == true){
                            storeCmd(dualstepper_bump_Cmd[key_num],v);
                        }
                    }
                    setDynamicValue(key_num,v);
                }
                 menuDrawListPage(&parameter_menuitems);
            }
            break;
        }

    for(int i = 0; i < TOTAL_AXIS; i++){
        if(getDynamicValue(i) != getParameter(cur_parameter,i)){
            setDynamicValue(i,getParameter(cur_parameter,i));
            menuDrawListItem(&parameter_menuitems.items[i],i);
        }
    }
    loopProcess();
    }

}

//Load main parameter list page
void loadParameterPage(void){
for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++)
    {
        uint8_t item_index = ps_cur_page*LISTITEM_PER_PAGE + i;
        if(item_index < PARAMETERS_COUNT)
        {
            parameterMainItems.items[i] = parametertypes[item_index];
        }
        else{
            parameterMainItems.items[i].icon = ICONCHAR_BACKGROUND;
        }
    }
// set page up down button according to page count and current page
  if (PARAMETERS_COUNT <= LISTITEM_PER_PAGE)
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
    else if(ps_cur_page == (PS_PAGE_COUNT-1)){
      parameterMainItems.items[5].icon = ICONCHAR_PAGEUP;
      parameterMainItems.items[6].icon = ICONCHAR_BACKGROUND;
    }
    else
    {
      parameterMainItems.items[5].icon = ICONCHAR_PAGEUP;
      parameterMainItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
};

void menuParameterSettings(void){
    KEY_VALUES key_num = KEY_IDLE;
    PARAMETERS now = infoParameters;
    ps_cur_page = 0;
    loadParameterPage();
    menuDrawListPage(&parameterMainItems);
    while (infoMenu.menu[infoMenu.cur] == menuParameterSettings)
    {
        key_num = menuKeyGetValue();
        switch (key_num)
        {
        case KEY_ICON_5:
            if(PS_PAGE_COUNT > 1)
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
            if(PS_PAGE_COUNT > 1)
            {
                if (ps_cur_page < PS_PAGE_COUNT - 1)
                {
                    ps_cur_page++;
                    loadParameterPage();
                    menuRefreshListPage();
                }
            }
            break;
        case KEY_ICON_7:
            if(memcmp(&now, &infoParameters, sizeof(PARAMETERS)))
                {
                    storeCmd("M500\n");
                }
            infoMenu.cur--;
            break;

        default:
            {
                int cp = ps_cur_page*LISTITEM_PER_PAGE + key_num;
                if(key_num < LISTITEM_PER_PAGE && cp < PARAMETERS_COUNT){
                    cur_parameter = cp;
                    infoMenu.menu[++infoMenu.cur] = menuShowParameter;  break;
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

    if(infoHost.connected == false || infoMenu.menu[infoMenu.cur] == menuPrinting)    return;
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
    if(infoHost.connected == false || infoMenu.menu[infoMenu.cur] == menuPrinting)    return;
    if(infoMenu.menu[infoMenu.cur] == menuMove || infoMenu.menu[infoMenu.cur] == menuStatus) return;
    drawGlobalInfo();

    return;
}

void drawGlobalInfo(void){
    char tempstr[10];
    GUI_SetBkColor(TITLE_BACKGROUND_COLOR);
    GUI_ClearRect(LCD_WIDTH/3, 0, LCD_WIDTH, BYTE_HEIGHT);

    //global nozzle
    lcd_frame_display(ICON_NOZZLE_X, 0, 2*BYTE_WIDTH, BYTE_HEIGHT, ICON_ADDR(ICON_GLOBAL_NOZZLE));
    my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(NOZZLE0), heatGetTargetTemp(NOZZLE0));
    GUI_DispStringInRect(VALUE_NOZZLE_X,0,VALUE_NOZZLE_X+8*BYTE_WIDTH,BYTE_HEIGHT, (u8 *)tempstr);

    //global bed
    lcd_frame_display(ICON_BED_X, 0, 2*BYTE_WIDTH, BYTE_HEIGHT, ICON_ADDR(ICON_GLOBAL_BED));
    my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
    GUI_DispStringInRect(VALUE_BED_X,0,VALUE_BED_X+8*BYTE_WIDTH,BYTE_HEIGHT, (u8 *)tempstr);
    GUI_SetBkColor(BACKGROUND_COLOR);
}
