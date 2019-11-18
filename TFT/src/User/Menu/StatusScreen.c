#include "StatusScreen.h"
#include "includes.h"
#include "GUI.h"
//1 title, ITEM_PER_PAGE items (icon + label) 
MENUITEMS StatusItems = {
// title
LABEL_READY,
// icon                       label

 {{ICON_STATUSNOZZLE,          LABEL_BACKGROUND},
  {ICON_STATUSBED,          LABEL_BACKGROUND},
  {ICON_STATUSFAN,          LABEL_BACKGROUND},
  {ICON_STATUS_SPEED,          LABEL_BACKGROUND},
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
SCROLL   msgScroll;
static int lastConnection_status = -1;

static char* msgtitle[20];
static char* msgbody[512];

static char* msgxyz[512];
static float xaxis;
static float yaxis;
static float zaxis;

#define matt_gray  0x2187

int current_Ext = 1;
int current_fan = 0;
int current_speedID = 0;
const char* SpeedID[2] = SPEED_ID;
// text position rectangles for Live icons 
//icon 0
const GUI_RECT rectA[8] = {
    {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                    SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    
    {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                    SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    
    {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                    SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    
    {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                    SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    /*---- rect for label in icons in second row
    {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                    SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    
    {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                    SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    
    {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                    SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT},
    
    {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
     START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                    SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT}
    */
};
const GUI_RECT rectB[8] = {
    {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    
    {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,                        SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    
    {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,                        SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    
    {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,                        SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    /*---- rect for values in icons in second row
    {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    
    {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,                        SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    
    {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,                        SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
    
    {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,                        SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT}
    */
};
/* const GUI_RECT RecA1 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + SSICON_NAME_X0,    SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecA2 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 1
const GUI_RECT RecB1 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + SSICON_NAME_X0,    SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecB2 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,                        SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 2
const GUI_RECT RecC1 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecC2 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,                        SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 3
const GUI_RECT RecD1 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         SS_TITLE_END_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecD2 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,                        SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         SS_TITLE_END_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 4
const GUI_RECT RecE1 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X + SSICON_NAME_X0,    SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecE2 = {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 1 * ICON_WIDTH + 0 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 5
const GUI_RECT RecF1 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + SSICON_NAME_X0,    SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecF2 = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,                        SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 2 * ICON_WIDTH + 1 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 6
const GUI_RECT RecG1 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecG2 = {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,                        SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 3 * ICON_WIDTH + 2 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT};
//icon 7
const GUI_RECT RecH1 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X + SSICON_NAME_X0,   SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         SS_TITLE_END_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_NAME_Y0 + BYTE_HEIGHT};

const GUI_RECT RecH2 = {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,                        SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0,
                        START_X + 4 * ICON_WIDTH + 3 * SPACE_X,                         SS_TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT}; */

//info rectangle          
const GUI_RECT RecIinfo = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,  TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X,  TITLE_END_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y};

const  GUI_RECT msgRect ={START_X + 1 * ICON_WIDTH + 1 * SPACE_X,   TITLE_END_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X,  TITLE_END_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

const GUI_RECT RecGantry = {START_X,  1*ICON_HEIGHT+0*SPACE_Y+SS_TITLE_END_Y+STATUS_GANTRY_YOFFSET,   4*ICON_WIDTH+3*SPACE_X+START_X,  1*ICON_HEIGHT+1*SPACE_Y+TITLE_END_Y-STATUS_GANTRY_YOFFSET};
                                




/*set status icons */
/* void set_status_icon(void)
{
        StatusItems.items[0] = ToolItems[0];
        StatusItems.items[1] = ToolItems[1];
        StatusItems.items[2] = ToolItems[2];
        StatusItems.items[3] = SpeedItems[0];

} */

void drawTemperature(void)
{
        //icons and their values are updated one by one to reduce flicker/clipping
    
        char *tempstr[100];
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[0],0);                                                                  //Ext icon
        GUI_DispStringInPrect(&rectA[0], (u8 *)heatDisplayID[current_Ext]);                           //Ext label
        
        GUI_SetColor(BLACK);
        my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(current_Ext), heatGetTargetTemp(current_Ext)); 
        GUI_DispStringInPrect(&rectB[0], (u8 *)tempstr);                                                //Ext value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[1],1);                                          //bed icon
        GUI_DispStringInPrect(&rectA[1], (u8 *)heatDisplayID[0]);             //Bed label
        GUI_SetColor(BLACK);
        my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(0), heatGetTargetTemp(0)); 
        GUI_DispStringInPrect(&rectB[1], (u8 *)tempstr);                        //Bed value

        GUI_SetColor(WHITE);
        menuDrawItem(&ToolItems[2],2);                                          //fan icon
        GUI_DispStringInPrect(&rectA[2],(u8 *)fanID[current_fan]);             //Fan label
        GUI_SetColor(BLACK);
        my_sprintf(tempstr, "%d", fanGetSpeed(current_fan)); 
        GUI_DispStringInPrect(&rectB[2], (u8 *)tempstr);                        //Fan value

        GUI_SetColor(WHITE);
        menuDrawItem(&SpeedItems[current_speedID],3);                           //Speed / flow icon
        GUI_DispStringInPrect(&rectA[3], (u8 *)SpeedID[current_speedID]);       //Speed / flow label
        GUI_SetColor(BLACK);
        my_sprintf(tempstr, "%d", speedGetPercent(current_speedID)); 
        GUI_DispStringInPrect(&rectB[3], (u8 *)tempstr);                        //speed / Flow value

        GUI_SetColor(matt_gray);
        
        GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
        GUI_SetColor(FK_COLOR);
        GUI_SetBkColor(matt_gray);
        my_sprintf(tempstr, "X: %.2f   Y: %.2f   Z: %.2f", xaxis, yaxis, zaxis);
        GUI_DispStringInPrect(&RecGantry,(u8 *)tempstr);
        
        GUI_SetBkColor(BK_COLOR);
        GUI_SetColor(FK_COLOR);
        //GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void storegantry(int n, float val){
    //float* px = &val;
    switch (n)
    {
    case 0:
        xaxis = val;
        break;
    case 1:
        yaxis = val;
        break;
    case 2:
        zaxis = val;
        break;   
    default:
        break;
    }
}


void statusScreen_setMsg(const uint8_t *title,const uint8_t *msg)
{
    memcpy(msgtitle,(char*)title,sizeof(msgtitle));
    memcpy(msgbody,(char*)msg,sizeof(msgbody));
    drawStatusScreenMsg();
    Scroll_CreatePara(&msgScroll,(u8*)msgbody,&msgRect);
}
void drawStatusScreenMsg(void)
{
//GUI_ClearRect(RecIinfo.x0,RecIinfo.y0,RecIinfo.x1,RecIinfo.y1);
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    u8* iconChar = (u8 *)"i";

    GUI_SetColor(0x4b0d);
    GUI_FillRect(RecIinfo.x0, RecIinfo.y0, RecIinfo.x1, RecIinfo.y1);

    GUI_SetColor(0x03BF);
    GUI_FillCircle(RecIinfo.x0 + STATUS_MSG_ICON_XCENTER, RecIinfo.y0 + STATUS_MSG_ICON_YCENTER, BYTE_WIDTH); //icon circle

    GUI_SetColor(WHITE);
    GUI_DrawCircle(RecIinfo.x0 + STATUS_MSG_ICON_XCENTER, RecIinfo.y0 + STATUS_MSG_ICON_YCENTER, BYTE_WIDTH); //circle outline
    GUI_FillRect(msgRect.x0, msgRect.y0, msgRect.x1, msgRect.y1); //rectangle for msg scroller
    
    GUI_DispString(RecIinfo.x0 + STATUS_MSG_ICON_XCENTER-(BYTE_WIDTH/2), RecIinfo.y0 + STATUS_MSG_ICON_YCENTER - (BYTE_HEIGHT/2), iconChar);

    GUI_DispString(RecIinfo.x0 + STATUS_MSG_TITLE_XOFFSET,RecIinfo.y0 + STATUS_MSG_ICON_YCENTER - (BYTE_HEIGHT/2),(u8*)msgtitle); 
    GUI_SetBkColor(0x03BF);
    GUI_FillRect(msgRect.x0, msgRect.y0, msgRect.x1, msgRect.y1);
    GUI_SetColor(BLACK);
    
    GUI_SetBkColor(BK_COLOR);
    GUI_SetColor(FK_COLOR);
    
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void scrollMsg(void){
    GUI_SetBkColor(WHITE);
    GUI_SetColor(BLACK);
    Scroll_DispString(&msgScroll,CENTER); 
    GUI_SetBkColor(BK_COLOR);
    GUI_SetColor(FK_COLOR);
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
        if(infoHost.connected == true){  
        storeCmd("M114 \n");
        }
        
        }
    }


void menuStatus(void)
{
    KEY_VALUES key_num = KEY_IDLE;
    GUI_SetBkColor(BK_COLOR);
    //set_status_icon();
    menuDrawPage(&StatusItems);
    GUI_SetColor(matt_gray);
        //GUI_ClearPrect(&RecGantry);
    GUI_FillPrect(&RecGantry);
    drawTemperature();
    drawStatusScreenMsg();

    while (infoMenu.menu[infoMenu.cur] == menuStatus)
    {
        if(infoHost.connected != lastConnection_status){
            if(infoHost.connected == false){
            statusScreen_setMsg(textSelect(LABEL_SCREEN_INFO), textSelect(LABEL_UNCONNECTED));
            }
            else{
                statusScreen_setMsg(textSelect(LABEL_SCREEN_INFO), textSelect(LABEL_READY));
            }
            lastConnection_status = infoHost.connected;
        } 
        scrollMsg();
        key_num = menuKeyGetValue();
        switch (key_num)
        {

        case KEY_ICON_0:
            infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
            break;
        case KEY_ICON_1:
            infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
            break;
        case KEY_ICON_2:
            infoMenu.menu[++infoMenu.cur] = menuFan;
            break;
        case KEY_ICON_3:
            infoMenu.menu[++infoMenu.cur] = menuSpeed;
            break;
        case KEY_ICON_4:
            infoMenu.menu[++infoMenu.cur] = menuMain;
            break;
        case KEY_ICON_7:
            infoMenu.menu[++infoMenu.cur] = menuPrint;
            break;

        default:break;
    }
    toggleTool();
    loopProcess();
  }
  
}






