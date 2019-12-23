#include "Parametersetting.h"
#include "includes.h"

SKEY_VALUES Select_Paranmeter;
bool getsetparameter,rev_success;
int cmd_getparameter_num = 0;
char * Get_value[VALUE_NUM];

const GUI_RECT rect_of_Skey[PARANMETER_NUM]={
  {BUTTON_WIDTH/2, 0, LCD_WIDTH-BUTTON_WIDTH/2,TITLE_SY+BYTE_HEIGHT}, //TITLE
  {3*START_X, TITLE_END_Y-BYTE_HEIGHT/2, LCD_WIDTH-BUTTON_WIDTH,TITLE_END_Y+BYTE_HEIGHT/2},//M906
  {3*START_X, TITLE_END_Y-BYTE_HEIGHT/2+4*BYTE_HEIGHT+7*BUTTON_SPACE, LCD_WIDTH-BUTTON_WIDTH,TITLE_END_Y-BYTE_HEIGHT/2+5*BYTE_HEIGHT+7*BUTTON_SPACE},//M92

  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y-BYTE_HEIGHT/2, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2},//M906_X
  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+0*BYTE_HEIGHT+1*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+1*BYTE_HEIGHT+1*BUTTON_SPACE},//M906_Y
  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+1*BYTE_HEIGHT+2*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+2*BYTE_HEIGHT+2*BUTTON_SPACE},//M906_Z
  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+2*BYTE_HEIGHT+3*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+3*BYTE_HEIGHT+3*BUTTON_SPACE},//M906_E

  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+3*BYTE_HEIGHT+7*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+4*BYTE_HEIGHT+7*BUTTON_SPACE},//M92_X
  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+4*BYTE_HEIGHT+8*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+5*BYTE_HEIGHT+8*BUTTON_SPACE},//M92_Y
  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+5*BYTE_HEIGHT+9*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+6*BYTE_HEIGHT+9*BUTTON_SPACE},//M92_Z
  {LCD_WIDTH-1*BUTTON_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+6*BYTE_HEIGHT+10*BUTTON_SPACE, LCD_WIDTH, TITLE_END_Y+BYTE_HEIGHT/2+7*BYTE_HEIGHT+10*BUTTON_SPACE},//M92_E
  
  // Back
  //{0, 0, BUTTON_WIDTH/2, LCD_HEIGHT/8},
  {LCD_WIDTH-BUTTON_WIDTH/2, 0, LCD_WIDTH, LCD_HEIGHT/8},
};

const GUI_RECT rect_of_numkey[KEY_NUM]={
  {0*SkeyWIDTH, P_height+0*Key_height, 1*SkeyWIDTH, P_height+1*Key_height},//1
  {1*SkeyWIDTH, P_height+0*Key_height, 2*SkeyWIDTH, P_height+1*Key_height},//2
  {2*SkeyWIDTH, P_height+0*Key_height, 3*SkeyWIDTH, P_height+1*Key_height},//3
  {3*SkeyWIDTH, P_height+0*Key_height, 4*SkeyWIDTH, P_height+1*Key_height},//del

  {0*SkeyWIDTH, P_height+1*Key_height, 1*SkeyWIDTH, P_height+2*Key_height},//4
  {1*SkeyWIDTH, P_height+1*Key_height, 2*SkeyWIDTH, P_height+2*Key_height},//5
  {2*SkeyWIDTH, P_height+1*Key_height, 3*SkeyWIDTH, P_height+2*Key_height},//6
  {3*SkeyWIDTH, P_height+1*Key_height, 4*SkeyWIDTH, P_height+2*Key_height},//.

  {0*SkeyWIDTH, P_height+2*Key_height, 1*SkeyWIDTH, P_height+3*Key_height},//7
  {1*SkeyWIDTH, P_height+2*Key_height, 2*SkeyWIDTH, P_height+3*Key_height},//8
  {2*SkeyWIDTH, P_height+2*Key_height, 3*SkeyWIDTH, P_height+3*Key_height},//9
  {3*SkeyWIDTH, P_height+2*Key_height, 4*SkeyWIDTH, P_height+3*Key_height},//0

  {0*SkeyWIDTH, P_height+3*Key_height, 2*SkeyWIDTH-BUTTON_SPACE, LCD_HEIGHT},//X
  {2*SkeyWIDTH+BUTTON_SPACE, P_height+3*Key_height, 4*SkeyWIDTH, LCD_HEIGHT},//OK
};

const char *const parameter[PARANMETER_NUM] = {
    "Parametersetting",
    "Set TMC Motor Current:","Set Axis Steps-per-unit:",
    "X","Y","Z","E",
    "X","Y","Z","E",
    "back",
};

const char *const parameter_cmd[PARANMETER_NUM] = {
    "0","0","0",
    "M906 X","M906 Y","M906 Z","M906 E",
    "M92 X","M92 Y","M92 Z","M92 E",
};

const char *const Key_Num[KEY_NUM] = {
  "1","2","3","del",
  "4","5","6",".",
  "7","8","9","0",
  "X","OK",
};

float Get_parameter_value[VALUE_NUM];

SKEY_VALUES SKeyGetValue(void)
{    
  return (SKEY_VALUES)KEY_GetValue(sizeof(rect_of_Skey)/sizeof(rect_of_Skey[0]), rect_of_Skey);    
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

void Draw_parameterbutton(void)
{
    TSC_ReDrawIcon = NULL;
    GUI_SetBkColor(SET_BK_COLOR);
    GUI_ClearRect(0, 0, LCD_WIDTH, LCD_HEIGHT);

    for (uint8_t k = 3; k < PARANMETER_NUM-1; k++)
    {   
        GUI_SetBkColor(SET_BK_COLOR);
        GUI_ClearRect(rect_of_Skey[k].x0,rect_of_Skey[k].y0,rect_of_Skey[k].x1,rect_of_Skey[k].y1);
    }
    for(uint8_t i = 0;i<PARANMETER_NUM;i++)
    {
        GUI_SetColor(BLACK);
        if(i<1)
        {
        GUI_DispStringInRect(rect_of_Skey[i].x0,rect_of_Skey[i].y0,rect_of_Skey[i].x1,rect_of_Skey[i].y1,(u8*)parameter[i]);
        GUI_DrawLine(0,rect_of_Skey[i].y1,LCD_WIDTH,rect_of_Skey[i].y1);
        GUI_DrawLine(0,rect_of_Skey[i].y1+1,LCD_WIDTH,rect_of_Skey[i].y1+1);
        }
        else if(i>=1 && i<3)
        {
        GUI_SetColor(BLACK); 
        GUI_DrawLine(0,(rect_of_Skey[2].y0+rect_of_Skey[6].y1)/2,LCD_WIDTH,(rect_of_Skey[2].y0+rect_of_Skey[6].y1)/2);
        GUI_DrawLine(0,(rect_of_Skey[2].y0+rect_of_Skey[6].y1)/2+1,LCD_WIDTH,(rect_of_Skey[2].y0+rect_of_Skey[6].y1)/2+1);

        GUI_DispLenString(rect_of_Skey[i].x0,rect_of_Skey[i].y0,(u8*)parameter[i],LCD_WIDTH-BUTTON_WIDTH);
        GUI_SetColor(WHITE);
        }
        else if(i>=3 && i<PARANMETER_NUM-1)
        GUI_DispLenString(rect_of_Skey[i].x0,rect_of_Skey[i].y0,(u8*)parameter[i],rect_of_Skey[i].x1-rect_of_Skey[i].x0);
        else
        {
        GUI_SetColor(WHITE);
        GUI_SetBkColor(WHITE);
        GUI_FillCircle((rect_of_Skey[i].x0+rect_of_Skey[i].x1)/2,(rect_of_Skey[i].y0+rect_of_Skey[i].y1)/2,BUTTON_WIDTH/6);
        GUI_SetColor(BLACK);
        GUI_DispStringInPrect(&rect_of_Skey[i],(u8*)"X");
        //GUI_DrawCircle((rect_of_Skey[i].x0+rect_of_Skey[i].x1)/2,(rect_of_Skey[i].y0+rect_of_Skey[i].y1)/2,BUTTON_WIDTH/6);
        GUI_SetBkColor(SET_BK_COLOR);
        GUI_SetColor(BLACK);
        //GUI_DispStringInPrect(&rect_of_Skey[i],(u8*)parameter[i]);//GUI_FillCircle()  GUI_DrawCircle(RecIinfo.x0 + STATUS_MSG_ICON_XCENTER, RecIinfo.y0 + STATUS_MSG_ICON_YCENTER, BYTE_WIDTH);
        }

        if(i>0 && i<3)
        {
        GUI_SetColor(GREEN);
        GUI_FillCircle(rect_of_Skey[i].x0-BYTE_WIDTH, rect_of_Skey[i].y0+BYTE_HEIGHT/2, BYTE_WIDTH/2);
        GUI_SetColor(YELLOW);
        GUI_DrawCircle(rect_of_Skey[i].x0-BYTE_WIDTH, rect_of_Skey[i].y0+BYTE_HEIGHT/2, BYTE_WIDTH/2);
        }
    }  
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetBkColor(BLACK);

    return ;
}

void Draw_keyboard(void)
{
    GUI_SetBkColor(WHITE);
    GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_numkey[0].y0);
    GUI_SetBkColor(BLUE);
    GUI_ClearRect(0, rect_of_numkey[0].y0, LCD_WIDTH, LCD_HEIGHT);
    for(uint8_t i=0 ;i<KEY_NUM ;i++)
    {
      if(i>=KEY_NUM-2)//return & sure
      {
        GUI_SetColor(GRAY);
        GUI_FillPrect(&rect_of_numkey[i]);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
        GUI_SetColor(BLACK);
        GUI_DispStringInPrect(&rect_of_numkey[i],(u8*)Key_Num[i]);
        GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
      }
      else
      {
        GUI_SetColor(BLACK);
        GUI_DispStringInPrect(&rect_of_numkey[i],(u8*)Key_Num[i]);
      }
    }
    return ;
}

void show_value(void)
{
    //8 value
const GUI_POINT value_xy[VALUE_NUM]={
  {rect_of_Skey[3].x0+2*BYTE_WIDTH,  (rect_of_Skey[3].y1-rect_of_Skey[3].y0-BYTE_HEIGHT)/2+rect_of_Skey[3].y0},
  {rect_of_Skey[4].x0+2*BYTE_WIDTH,  (rect_of_Skey[4].y1-rect_of_Skey[4].y0-BYTE_HEIGHT)/2+rect_of_Skey[4].y0},
  {rect_of_Skey[5].x0+2*BYTE_WIDTH,  (rect_of_Skey[5].y1-rect_of_Skey[5].y0-BYTE_HEIGHT)/2+rect_of_Skey[5].y0},
  {rect_of_Skey[6].x0+2*BYTE_WIDTH,  (rect_of_Skey[6].y1-rect_of_Skey[6].y0-BYTE_HEIGHT)/2+rect_of_Skey[6].y0},
  {rect_of_Skey[7].x0+2*BYTE_WIDTH,  (rect_of_Skey[7].y1-rect_of_Skey[7].y0-BYTE_HEIGHT)/2+rect_of_Skey[7].y0},
  {rect_of_Skey[8].x0+2*BYTE_WIDTH,  (rect_of_Skey[8].y1-rect_of_Skey[8].y0-BYTE_HEIGHT)/2+rect_of_Skey[8].y0},
  {rect_of_Skey[9].x0+2*BYTE_WIDTH,  (rect_of_Skey[9].y1-rect_of_Skey[9].y0-BYTE_HEIGHT)/2+rect_of_Skey[9].y0},
  {rect_of_Skey[10].x0+2*BYTE_WIDTH, (rect_of_Skey[10].y1-rect_of_Skey[10].y0-BYTE_HEIGHT)/2+rect_of_Skey[10].y0},
};

    GUI_SetBkColor(WHITE);
    GUI_SetColor(BLACK);
    for(int i = 0;i<VALUE_NUM;i++)
    {
        GUI_DispFloat(value_xy[i].x, value_xy[i].y, Get_parameter_value[i], 4, 1, CENTER);
    }
    GUI_SetColor(WHITE);
    GUI_SetBkColor(BLACK);

    return ;
}
void Setting_parameter(void)
{   
    GUI_RECT ParameterRect = {0, 0, LCD_WIDTH, rect_of_numkey[0].y0};
    uint8_t nowIndex = 0,lastIndex = 0;
    char ParameterBuf[BUFLONG] = {0};
    int cmd_long;
    char * cmd_buff;
    NUM_KEY_VALUES key_num = NUM_KEY_IDLE;

    Draw_keyboard();
    while (infoMenu.menu[infoMenu.cur] == Setting_parameter)
    {
        key_num = NumKeyGetValue();
        switch (key_num)
        {
        case NUM_KEY_IDLE:
            break;

        case NUM_KEY_12:
            Send_Settingcmd();
            infoMenu.cur--;
            break;

        case NUM_KEY_13:
            if(nowIndex)
            {
             ParameterBuf[nowIndex++] = '\n'; 
             ParameterBuf[nowIndex] = 0;
             #if 1
             cmd_long = strlen(parameter_cmd[Select_Paranmeter]);
             cmd_buff = malloc(cmd_long + strlen(ParameterBuf));
             if(cmd_buff)
             {
                strcpy(cmd_buff, parameter_cmd[Select_Paranmeter]);
                storeCmd(strcat(cmd_buff, ParameterBuf));
                free(cmd_buff);
             }
             #endif
             ParameterBuf[nowIndex = 0] = 0;

            }
            Send_Settingcmd();
            infoMenu.cur--;
            break;

        case NUM_KEY_DEL:
            if(nowIndex)
            ParameterBuf[--nowIndex] = 0;
        break;

        default:
            if(nowIndex < BUFLONG-1)
            {
            ParameterBuf[nowIndex++] = Key_Num[key_num][0];
            ParameterBuf[nowIndex] = 0;
            }
            break;
        }

        if(lastIndex != nowIndex)
        {
            lastIndex = nowIndex;
            GUI_SetBkColor(WHITE);
            GUI_ClearRect(0, 0, LCD_WIDTH, rect_of_numkey[0].y0);
            GUI_DispStringInPrect(&ParameterRect, (u8 *)ParameterBuf);
        }
    }
    return ;
}

void parametersetting(void)
{

    TSC_ReDrawIcon = NULL;
    Draw_parameterbutton();
    Send_Settingcmd();
    SKEY_VALUES key_num = SKEY_IDLE;

    while (infoMenu.menu[infoMenu.cur] == parametersetting)
    {
    key_num = SKeyGetValue();
    switch (key_num)
    {
    case SKEY_3:
    case SKEY_4:
    case SKEY_5:
    case SKEY_6:
    case SKEY_7:   
    case SKEY_8:
    case SKEY_9:
    case SKEY_10:
        Select_Paranmeter = key_num;
        infoMenu.menu[++infoMenu.cur] = Setting_parameter;
        break;

    case SKEY_11:
        infoMenu.cur--;
        break;

    default:
        break;
    }

    show_value();
    loopBackEnd();
    }
    return ;
}

void Temp_change(void)
{
    //static FP_MENU NUM[MAX_MENU_DEPTH];
    char tempstr[10];
    static int16_t compare [2] ;
    #ifndef STATUS_SCREEN
    return;
    #endif 
    if(infoHost.connected == false)    return;
    
    if(heatGetCurrentTemp(NOZZLE0) != compare[0] || heatGetCurrentTemp(BED) != compare[1] )
    //|| strcmp((char *)infoMenu.menu[infoMenu.cur],(char *)NUM)!=0)
    {
        //strcpy((char *)NUM ,(char *)infoMenu.menu[infoMenu.cur]);
        compare[0] = heatGetCurrentTemp(NOZZLE0);
        compare[1] = heatGetCurrentTemp(BED);
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
    
    return ;
}

void show_globalinfo(void)
{
    if(infoHost.connected == false)    return;
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

    return;
}