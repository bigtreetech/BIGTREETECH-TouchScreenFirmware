#include "StatusScreen.h"
#include "includes.h"
#include "Heat.h"

//1 title, ITEM_PER_PAGE items (icon + label) 
MENUITEMS StatusItems = {
// title
LABEL_READY,
// icon                       label

 {{ICON_BACKGROUND,          LABEL_BACKGROUND},
  {ICON_BACKGROUND,          LABEL_BACKGROUND},
  {ICON_BACKGROUND,          LABEL_BACKGROUND},
  #if EXTRUDER_NUM == 2
  {ICON_BACKGROUND,          LABEL_BACKGROUND},
#else
    #ifdef UNIFIED_MENU
    {ICON_HOME_MOVE,           LABEL_BACKGROUND}, //Label is removed to make the status screen look symetrical
    #else
    {ICON_LEVELING,            LABEL_BACKGROUND}, //Label is removed to make the status screen look symetrical
    #endif
#endif
  {ICON_MAINMENU,              LABEL_MAINMENU},  
  {ICON_BACKGROUND,          LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,          LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_PRINT,                    LABEL_PRINT},}
};

const ITEM ToolItems[3] = {
// icon                       label
  {ICON_STATUSNOZZLE,             LABEL_BACKGROUND},
  {ICON_STATUSBED,                LABEL_BACKGROUND},
  {ICON_STATUSFAN,                LABEL_BACKGROUND},
//{ICON_HEAT_STATUS,          LABEL_BACKGROUND},
//{ICON_BED_STATUS,           LABEL_BACKGROUND},
//{ICON_FAN_STATUS,           LABEL_BACKGROUND},
};

static u32       nowTime = 0;
static u32   update_time = 200; // 1 seconds is 100




int current_Ext = 1;
int current_fan = 0;

// text position rectangles for Live icons 

const GUI_RECT RecA1 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const GUI_RECT RecA2 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};

const GUI_RECT RecB1 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecB2 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};

const  GUI_RECT RecC1 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecC2 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};

const  GUI_RECT RecD1 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecD2 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,     TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};

/*set status icons */
void set_status_icon(void)
{
    //int last_index = 0; 
   if (EXTRUDER_NUM == 2){
        StatusItems.items[0] = ToolItems[0];
        StatusItems.items[1] = ToolItems[0];
        StatusItems.items[2] = ToolItems[1];
        StatusItems.items[3] = ToolItems[2];
        //last_index += 2;
   }
    else
    {
        StatusItems.items[0] = ToolItems[0];
        StatusItems.items[1] = ToolItems[1];
        StatusItems.items[2] = ToolItems[2];
        //last_index += 1;        
    }
}

void drawTemperature(void)
{
        //icons and their values are updated one by one to reduce flicker/clipping
    
    char *tempstr[7];

   #if EXTRUDER_NUM == 2
        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[0],0);                                          //T0 icon
        GUI_DispStringInPrect(&RecA1, (u8 *)heatDisplayID[1],   0);             //T0 label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d/%d", heatGetCurrentTemp(1), heatGetTargetTemp(1)); 
        GUI_DispStringInPrect(&RecA2, (u8 *)tempstr, 0);                        //T0 value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[0],1);                                          //T1 icon
        GUI_DispStringInPrect(&RecB1, (u8 *)heatDisplayID[2],   0);             //T1 label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d/%d", heatGetCurrentTemp(2), heatGetTargetTemp(2)); 
        GUI_DispStringInPrect(&RecB2, (u8 *)tempstr, 0);                        //T1 value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[1],2);                                          //bed icon
        GUI_DispStringInPrect(&RecC1, (u8 *)heatDisplayID[0],   0);             //Bed label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d/%d", heatGetCurrentTemp(0), heatGetTargetTemp(0)); 
        GUI_DispStringInPrect(&RecC2, (u8 *)tempstr, 0);                        //Bed value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[2],3);                                          //fan icon
        GUI_DispStringInPrect(&RecD1, (u8 *)fanID[current_fan], 0);             //Fan label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d", fanGetSpeed(current_fan)); 
        GUI_DispStringInPrect(&RecD2, (u8 *)tempstr, 0);                        //Fan value


    #else

     GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[0],0);                                                                  //T0 icon
        GUI_DispStringInPrect(&RecA1, (u8 *)heatDisplayID[current_Ext],   0);                           //T0 label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d/%d", heatGetCurrentTemp(current_Ext), heatGetTargetTemp(current_Ext)); 
        GUI_DispStringInPrect(&RecA2, (u8 *)tempstr, 0);                                                //T0 value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[1],1);                                          //bed icon
        GUI_DispStringInPrect(&RecB1, (u8 *)heatDisplayID[0],   0);             //Bed label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d/%d", heatGetCurrentTemp(0), heatGetTargetTemp(0)); 
        GUI_DispStringInPrect(&RecB2, (u8 *)tempstr, 0);                        //Bed value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[2],2);                                          //fan icon
        GUI_DispStringInPrect(&RecC1, (u8 *)fanID[current_fan], 0);             //Fan label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d", fanGetSpeed(current_fan)); 
        GUI_DispStringInPrect(&RecC2, (u8 *)tempstr, 0);                        //Fan value

    #endif
}

void toggleTool(void)
{
    if (OS_GetTime() > nowTime + update_time)
    {
        if (EXTRUDER_NUM > 2)
        {
            current_Ext = (current_Ext + 1) % HEATER_NUM;
            if (current_Ext == 0)
            {
                current_Ext += 1;
            }
        }
        if (FAN_NUM > 1)
        {
            current_fan = (current_fan + 1) % FAN_NUM;
        }
        nowTime = OS_GetTime();
        drawTemperature();
    }
}

void menuStatus(void)
{
  KEY_VALUES  key_num = KEY_IDLE;
  GUI_SetBkColor(ST7920_BKCOLOR);
  set_status_icon();
  menuDrawPage(&StatusItems);
     drawTemperature();
  while(infoMenu.menu[infoMenu.cur] == menuStatus)
  {
   
    key_num = menuKeyGetValue();
    switch(key_num)
    {
    #if EXTRUDER_NUM == 2
    case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuFan;           break;
    case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuMain;          break;
    case KEY_ICON_7: infoMenu.menu[++infoMenu.cur] = menuPrint;         break;
     // case KEY_ICON_7:
     //   infoMenu.cur--;
     //   break;

    #else
    #ifdef UNIFIED_MENU
    case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuFan;           break;
    case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuUnifiedMove;   break;
    case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuMain;          break;
    case KEY_ICON_7: infoMenu.menu[++infoMenu.cur] = menuPrint;         break;
    #else
    case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHeat;   break;
    case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuHeat;   break;
    case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuFan;   break;
    case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuAutoLeveling;   break;
    case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuMain;   break;
    case KEY_ICON_7: infoMenu.menu[++infoMenu.cur] = menuPrint;   break;
    
    #endif
    #endif

    default:break;
    }
    toggleTool();
    loopProcess();
  }
  
}






