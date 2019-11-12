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
  {ICON_BACKGROUND,          LABEL_BACKGROUND},
  {ICON_MAINMENU,              LABEL_MAINMENU},  
  {ICON_BACKGROUND,          LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,          LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_PRINT,                    LABEL_PRINT},}
};

const ITEM ToolItems[5] = {
// icon                       label
  {ICON_STATUSNOZZLE,             LABEL_BACKGROUND},
  {ICON_STATUSBED,                LABEL_BACKGROUND},
  {ICON_STATUSFAN,                LABEL_BACKGROUND},
//{ICON_HEAT_STATUS,          LABEL_BACKGROUND},
//{ICON_BED_STATUS,           LABEL_BACKGROUND},
//{ICON_FAN_STATUS,           LABEL_BACKGROUND},
};
const ITEM SpeedItems[2] = {
// icon                       label
  {ICON_STATUS_SPEED,               LABEL_BACKGROUND},
  {ICON_STATUS_FLOW,                LABEL_BACKGROUND},
};

static u32       nowTime = 0;
static u32   update_time = 200; // 1 seconds is 100

int current_Ext = 1;
int current_fan = 0;
int current_speedID = 0;
const char* SpeedID[] = SPEED_ID;
// text position rectangles for Live icons 
//icon 0
const GUI_RECT RecA1 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const GUI_RECT RecA2 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 1
const GUI_RECT RecB1 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecB2 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,                        TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 2
const  GUI_RECT RecC1 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + statusicon_name_x0,   TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecC2 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,                        TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 3
const  GUI_RECT RecD1 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + statusicon_name_x0,   TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecD2 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,                        TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 4
const GUI_RECT RecE1 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const GUI_RECT RecE2 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 5
const GUI_RECT RecF1 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + statusicon_name_x0,    TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecF2 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,                        TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 6
const  GUI_RECT RecG1 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + statusicon_name_x0,   TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecG2 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,                        TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};
//icon 7
const  GUI_RECT RecH1 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + statusicon_name_x0,   TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_name_y0 + BYTE_HEIGHT};

const  GUI_RECT RecH2 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,                        TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + statusicon_val_y0 + BYTE_HEIGHT};


/*set status icons */
void set_status_icon(void)
{
        StatusItems.items[0] = ToolItems[0];
        StatusItems.items[1] = ToolItems[1];
        StatusItems.items[2] = ToolItems[2];
        StatusItems.items[3] = SpeedItems[0];

}

void drawTemperature(void)
{
        //icons and their values are updated one by one to reduce flicker/clipping
    
    char *tempstr[7];

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[0],0);                                                                  //Ext icon
        GUI_DispStringInPrect(&RecA1, (u8 *)heatDisplayID[current_Ext],   0);                           //Ext label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d/%d", heatGetCurrentTemp(current_Ext), heatGetTargetTemp(current_Ext)); 
        GUI_DispStringInPrect(&RecA2, (u8 *)tempstr, 0);                                                //Ext value

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

        GUI_SetColor(WHITE);
        menuDrawItem(&SpeedItems[current_speedID],3);                           //Speed / flow icon
        GUI_DispStringInPrect(&RecD1, (u8 *)SpeedID[current_speedID], 0);       //Speed / flow label
        GUI_SetColor(BLACK);
        sprintf(tempstr, "%d", speedGetPercent(current_speedID)); 
        GUI_DispStringInPrect(&RecD2, (u8 *)tempstr, 0);                        //speed / Flow value
        GUI_SetColor(WHITE);
}

void toggleTool(void)
{
    if (OS_GetTime() > nowTime + update_time)
    {
        if (EXTRUDER_NUM > 1)
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
        current_speedID = (current_speedID + 1) % 2;
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

    case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;   break;
    case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuFan;           break;
    case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuSpeed;         break;
    case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuMain;          break;
    case KEY_ICON_7: infoMenu.menu[++infoMenu.cur] = menuPrint;         break;

    default:break;
    }
    toggleTool();
    loopProcess();
  }
  
}






