#include "Parametersetting.h"
#include "includes.h"

const GUI_RECT rect_of_numkey[KEY_NUM]={
  {0*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//1
  {1*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//2
  {2*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//3
  {3*SKEYWIDTH, ICON_START_Y+0*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT},//OK

  {0*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//4
  {1*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//5
  {2*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//6
  {3*SKEYWIDTH, ICON_START_Y+1*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT},//Del

  {0*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//7
  {1*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//8
  {2*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//9
  {3*SKEYWIDTH, ICON_START_Y+2*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT},//

  {0*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 1*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//.
  {1*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 2*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//0
  {2*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 3*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//
  {3*SKEYWIDTH, ICON_START_Y+3*SKEYHEIGHT, 4*SKEYWIDTH, ICON_START_Y+4*SKEYHEIGHT},//
};

const char *const parameterCurrent[TOTAL_AXIS] = {
    "X Axis Current(mA)",
    "Y Axis Current(mA)",
    "Z Axis Current(mA)",
    "E Current(mA)"
};

const char *const parameterSteps[TOTAL_AXIS] = {
    "X Axis Steps/MM",
    "Y Axis Steps/MM",
    "Z Axis Steps/MM",
    "E Steps/MM"
};

const char *const parameter_currentCmd[TOTAL_AXIS] = {
    "M906 X%.1f\n","M906 Y%.1f\n","M906 Z%.1f\n","M906 E%.1f\n",
};

const char *const parameter_stepsCmd[TOTAL_AXIS] = {
    "M92 X%.1f\n","M92 Y%.1f\n","M92 Z%.1f\n","M92 E%.1f\n",
};

const char *const Key_Num[KEY_NUM] = {
  "1","2","3","\u0894",
  "4","5","6","\u0899",
  "7","8","9","\u0895",
  ".","0","",""
};

const LISTITEMS parameterMainItems = {
// title
LABEL_SETTING_PARAMETER,
// icon                     ItemType         Item Title                item value text(only for custom value)
{
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_STEPS_SETTING,    LABEL_BACKGROUND},
    {ICONCHAR_SETTING1,     LIST_MOREBUTTON,    LABEL_CURRENT_SETTING,  LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,         LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,         LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,         LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,         LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,     LIST_LABEL,         LABEL_BACKGROUND,       LABEL_BACKGROUND},
    {ICONCHAR_BACK,           LIST_LABEL,         LABEL_BACKGROUND,       LABEL_BACKGROUND},}
};

float parameter_current_value[TOTAL_AXIS];
float parameter_steps_value[TOTAL_AXIS];

float getParameterCurrent(AXIS index){
    return parameter_current_value[index];
}

float getParameterSteps(AXIS index){
    return parameter_steps_value[index];
}

void setParameterCurrent(AXIS index, float val){
    parameter_current_value[index] = val;
}

void setParameterSteps(AXIS index, float val){
    parameter_steps_value[index] = val;
}

NUM_KEY_VALUES NumKeyGetValue(void)
{
  return (NUM_KEY_VALUES)KEY_GetValue(sizeof(rect_of_numkey)/sizeof(rect_of_numkey[0]), rect_of_numkey);
}

void Send_Settingcmd(void)
{
    storeCmd("M906\n");
    storeCmd("M92\n");
    return ;
}


void setCurrentMenuItems(){

    KEY_VALUES key_num = KEY_IDLE;

    LISTITEMS currentmenuitems ={
    // title
    LABEL_SETTING_PARAMETER,
    // icon                     ItemType        Item Title               item value text(only for custom value)
    {{ICONCHAR_BLANK,           LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BLANK,            LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BLANK,            LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BLANK,            LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACK,             LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},}
    };

    for(int i = 0; i < TOTAL_AXIS; i++){
        setDynamicLabel(i,(char*)parameterCurrent[i]);
        setDynamicValue(i,parameter_current_value[i]);
    }

    menuDrawListPage(&currentmenuitems);

    while (infoMenu.menu[infoMenu.cur] == setCurrentMenuItems)
    {
        key_num = menuKeyGetValue();

        switch (key_num)
        {
        case KEY_ICON_7:
            storeCmd("M500\n");
            infoMenu.cur--;
            break;

        default:
            if(key_num < (KEY_VALUES)TOTAL_AXIS){
                float v = num_pad(getParameterCurrent((AXIS)key_num));
                if (v != getParameterCurrent((AXIS)key_num))
                {
                    storeCmd(parameter_currentCmd[key_num],v);
                    setDynamicValue(key_num,v);
                    setParameterCurrent((AXIS)key_num,v);
                }
                menuDrawListPage(&currentmenuitems);
            }
            break;

        }
        for(AXIS i = X_AXIS; i < TOTAL_AXIS; i++){
            if(getDynamicValue(i) != getParameterCurrent(i)){
                setDynamicValue(i,parameter_current_value[i]);
                menuDrawListItem(&currentmenuitems.items[i],i);
            }
        }
        loopProcess();
    }

}
void setStepsMenuItems(){

    KEY_VALUES key_num = KEY_IDLE;

    LISTITEMS stepsmenuitems ={
    // title
    LABEL_SETTING_PARAMETER,
    // icon                     ItemType        Item Title               item value text(only for custom value)
    {{ICONCHAR_BLANK,           LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BLANK,            LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BLANK,            LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BLANK,            LIST_CUSTOMVALUE,     LABEL_DYNAMIC,     LABEL_DYNAMIC},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACKGROUND,       LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},
    {ICONCHAR_BACK,             LIST_LABEL,           LABEL_BACKGROUND,     LABEL_BACKGROUND},}
    };

    for(int i = 0; i < TOTAL_AXIS; i++){
        setDynamicLabel(i,(char*)parameterSteps[i]);
        setDynamicValue(i,parameter_steps_value[i]);
    }

    menuDrawListPage(&stepsmenuitems);
    while (infoMenu.menu[infoMenu.cur] == setStepsMenuItems)
    {
        key_num = menuKeyGetValue();

        switch (key_num)
        {

        case KEY_ICON_7:
            storeCmd("M500\n");
            infoMenu.cur--;
            break;

        default:
            if(key_num < (KEY_VALUES)TOTAL_AXIS){
                float v = num_pad(getParameterSteps((AXIS)key_num));
                if (v != getParameterSteps((AXIS)key_num)){
                    storeCmd(parameter_stepsCmd[key_num],v);
                    setDynamicValue((AXIS)key_num,v);
                    setParameterSteps((AXIS)key_num,v);
                }
                menuDrawListPage(&stepsmenuitems);
            }
            break;

        }
        for(AXIS i = X_AXIS; i < TOTAL_AXIS; i++){
            if(getDynamicValue(i) != getParameterSteps(i)){
                setDynamicValue(i,parameter_steps_value[i]);
                menuDrawListItem(&stepsmenuitems.items[i],i);
            }
        }
        loopProcess();
    }

}

void parametersetting(){
    KEY_VALUES key_num = KEY_IDLE;
    Send_Settingcmd();
    menuDrawListPage(&parameterMainItems);
    while (infoMenu.menu[infoMenu.cur] == parametersetting)
    {
        key_num = menuKeyGetValue();
        switch (key_num)
        {
        case KEY_ICON_0:
            infoMenu.menu[++infoMenu.cur] = setStepsMenuItems;
            break;

        case KEY_ICON_1:
            infoMenu.menu[++infoMenu.cur] = setCurrentMenuItems;
            break;

        case KEY_ICON_7:
            infoMenu.cur--;
            break;

        default:
            break;

        }

    loopProcess();
    }

}


void Draw_keyboard(void)
{
    TSC_ReDrawIcon = NULL;
    GUI_SetBkColor(BLACK);
    GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_numkey[0].y0);
    GUI_SetBkColor(WHITE);
    GUI_ClearRect(0, rect_of_numkey[0].y0, LCD_WIDTH, LCD_HEIGHT);
    GUI_SetColor(BLACK);

    for (int i = 0;i<3;i++){
        GUI_DrawLine(rect_of_numkey[i].x1,rect_of_numkey[i].y0,rect_of_numkey[12+i].x1,rect_of_numkey[12+i].y1);
        GUI_DrawLine(rect_of_numkey[i*4].x0,rect_of_numkey[i*4].y1,rect_of_numkey[3+i*4].x1,rect_of_numkey[3+i*4].y1);
    }

    for(uint8_t i=0 ;i<KEY_NUM ;i++)
    {

        //GUI_DrawPrect(&rect_of_numkey[i]);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
        GUI_SetColor(BLACK);
        GUI_DispStringInPrect(&rect_of_numkey[i],(u8*)Key_Num[i]);
        GUI_SetTextMode(GUI_TEXTMODE_NORMAL);

    }
    GUI_RestoreColorDefault();
}


float num_pad(float old_val)
{
    //bool exit = false;
    GUI_RECT oldParameterRect = {0, 0, LCD_WIDTH/2 - BYTE_WIDTH, rect_of_numkey[0].y0};
    GUI_RECT newParameterRect = {LCD_WIDTH/2 + BYTE_WIDTH, 0, LCD_WIDTH, rect_of_numkey[0].y0};
    GUI_RECT arrowRect = {LCD_WIDTH/2 - BYTE_WIDTH, 0, LCD_WIDTH/2 + BYTE_WIDTH, rect_of_numkey[0].y0};
    uint8_t nowIndex = 0,lastIndex = 0;
    char ParameterBuf[BUFLONG] = {0};
    NUM_KEY_VALUES key_num = NUM_KEY_IDLE;
    my_sprintf(ParameterBuf,"%f",old_val);
    Draw_keyboard();
    GUI_DispStringInPrect(&arrowRect,(u8 *)">");
    char tempstr[BUFLONG];
    my_sprintf(tempstr, "%.2f", old_val);
    GUI_SetColor(WHITE);
    GUI_DispStringInPrect(&oldParameterRect,(u8*)tempstr);
    while (1)
    {
        key_num = NumKeyGetValue();
        switch (key_num)
        {
        case NUM_KEY_EXIT:
            Send_Settingcmd();
            return old_val;

        case NUM_KEY_DEL:
            if(nowIndex)
            ParameterBuf[--nowIndex] = 0;
            break;

        case NUM_KEY_1:
        case NUM_KEY_2:
        case NUM_KEY_3:
        case NUM_KEY_4:
        case NUM_KEY_5:
        case NUM_KEY_6:
        case NUM_KEY_7:
        case NUM_KEY_8:
        case NUM_KEY_9:
        case NUM_KEY_0:
            if(nowIndex < BUFLONG-1)
            {
                ParameterBuf[nowIndex++] = Key_Num[key_num][0];
                ParameterBuf[nowIndex] = 0;
            }
            break;
        case NUM_KEY_DEC:
            if(!strchr(&ParameterBuf, Key_Num[key_num][0])){
                if(nowIndex < BUFLONG-1)
                {
                    ParameterBuf[nowIndex++] = Key_Num[key_num][0];
                    ParameterBuf[nowIndex] = 0;
                }
            }
            break;

        case NUM_KEY_OK:
           /*  if(nowIndex)
            {
             ParameterBuf[nowIndex++] = '\n';
             ParameterBuf[nowIndex] = 0;

             cmd_long = strlen(para_cmd);
             cmd_buff = malloc(cmd_long + strlen(ParameterBuf));
             if(cmd_buff)
             {
                strcpy(cmd_buff, para_cmd);
                storeCmd(strcat(cmd_buff, ParameterBuf));
                free(cmd_buff);
             }

             ParameterBuf[nowIndex = 0] = 0;

            } */

            //Send_Settingcmd();
            return strtof(ParameterBuf,NULL);

            default:
            break;
        }

        if(lastIndex != nowIndex)
        {
            lastIndex = nowIndex;
            GUI_SetBkColor(BLACK);
            GUI_SetColor(WHITE);
            GUI_ClearPrect(&newParameterRect);
            GUI_DispStringInPrect(&newParameterRect, (u8 *)ParameterBuf);
            GUI_RestoreColorDefault();
        }

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

    GUI_ClearRect(LCD_WIDTH/3, 0, LCD_WIDTH, BYTE_HEIGHT);

    //global nozzle
    lcd_frame_display(ICON_NOZZLE_X, 0, 2*BYTE_WIDTH, BYTE_HEIGHT, ICON_ADDR(ICON_GLOBAL_NOZZLE0));
    my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(NOZZLE0), heatGetTargetTemp(NOZZLE0));
    GUI_DispStringInRect(VALUE_NOZZLE_X,0,VALUE_NOZZLE_X+8*BYTE_WIDTH,BYTE_HEIGHT, (u8 *)tempstr);

    //global bed
    lcd_frame_display(ICON_BED_X, 0, 2*BYTE_WIDTH, BYTE_HEIGHT, ICON_ADDR(ICON_GLOBAL_BED));
    my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
    GUI_DispStringInRect(VALUE_BED_X,0,VALUE_BED_X+8*BYTE_WIDTH,BYTE_HEIGHT, (u8 *)tempstr);
}
