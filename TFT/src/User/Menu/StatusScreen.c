#include "StatusScreen.h"
#include "includes.h"
#include "GUI.h"
//1 title, ITEM_PER_PAGE items (icon + label) 
MENUITEMS StatusItems = {
// title
LABEL_READY,
// icon                       label
 {{ICON_STATUSNOZZLE,         LABEL_BACKGROUND},
  {ICON_STATUSBED,            LABEL_BACKGROUND},
  {ICON_STATUSFAN,            LABEL_BACKGROUND},
  {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
  {ICON_MAINMENU,             LABEL_MAINMENU},  
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_BACKGROUND,           LABEL_BACKGROUND}, //Reserved for gantry position to be added later
  {ICON_PRINT,                LABEL_PRINT},}
};

const ITEM ToolItems[3] = {
// icon                       label
  {ICON_STATUSNOZZLE,         LABEL_BACKGROUND},
  {ICON_STATUSBED,            LABEL_BACKGROUND},
  {ICON_STATUSFAN,            LABEL_BACKGROUND},
//{ICON_HEAT_STATUS,          LABEL_BACKGROUND},
//{ICON_BED_STATUS,           LABEL_BACKGROUND},
//{ICON_FAN_STATUS,           LABEL_BACKGROUND},
};
const ITEM SpeedItems[2] = {
// icon                       label
  {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
  {ICON_STATUS_FLOW,          LABEL_BACKGROUND},
};

static u32 nowTime = 0;
static u32 update_time = 200; // 1 seconds is 100
SCROLL     msgScroll;
static int lastConnection_status = -1;

static char msgtitle[20];
static char msgbody[512];

//static char msgxyz[512];
static float xaxis;
static float yaxis;
static float zaxis;
static bool gantryCmdWait = false;

TOOL current_Ext = NOZZLE0;
int current_fan = 0;
int current_speedID = 0;
const char* SpeedID[2] = SPEED_ID;
// text position rectangles for Live icons 
//icon 0
const GUI_POINT pointID[4] = {
  {1*ICON_WIDTH+0*SPACE_X+START_X - BYTE_WIDTH/2,    ICON_START_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
  {2*ICON_WIDTH+1*SPACE_X+START_X - BYTE_WIDTH/2,    ICON_START_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
  {3*ICON_WIDTH+2*SPACE_X+START_X - BYTE_WIDTH/2,    ICON_START_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
  {4*ICON_WIDTH+3*SPACE_X+START_X - BYTE_WIDTH/2,    ICON_START_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_NAME_Y0},
};
const GUI_RECT rectB[4] = {
  {START_X + 0 * ICON_WIDTH + 0 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 1 * ICON_WIDTH + 0 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
  
  {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 2 * ICON_WIDTH + 1 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
  
  {START_X + 2 * ICON_WIDTH + 2 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 3 * ICON_WIDTH + 2 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
  
  {START_X + 3 * ICON_WIDTH + 3 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0,
   START_X + 4 * ICON_WIDTH + 3 * SPACE_X,  ICON_START_Y +  0 * ICON_HEIGHT + 0 * SPACE_Y + SSICON_VAL_Y0 + BYTE_HEIGHT},
};

//info rectangle          
const GUI_RECT RectInfo = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,  ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X,  ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y};

const  GUI_RECT msgRect ={START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2,   ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                          START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2,   ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

const GUI_RECT RecGantry = {START_X,                        1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            4*ICON_WIDTH+3*SPACE_X+START_X, 1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y - STATUS_GANTRY_YOFFSET};
                                

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
    
  char tempstr[100];
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&ToolItems[0],0);                                                                //Ext icon
  GUI_DispStringRight(pointID[0].x, pointID[0].y, (u8 *)heatDisplayID[current_Ext]);                           //Ext label
  
  GUI_SetColor(VAL_COLOR);
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(current_Ext), heatGetTargetTemp(current_Ext)); 
  GUI_DispStringInPrect(&rectB[0], (u8 *)tempstr);                                                //Ext value

  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&ToolItems[1],1);                                          //Bed icon
  GUI_DispStringRight(pointID[1].x, pointID[1].y, (u8 *)heatDisplayID[BED]);             //Bed label
  GUI_SetColor(VAL_COLOR);
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED)); 
  GUI_DispStringInPrect(&rectB[1], (u8 *)tempstr);                        //Bed value

  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&ToolItems[2],2);                                          //Fan icon
  GUI_DispStringRight(pointID[2].x, pointID[2].y, (u8 *)fanID[current_fan]);              //Fan label
  GUI_SetColor(VAL_COLOR);
  
  u8 fs;
  #ifdef SHOW_FAN_PERCENTAGE
    fs = (fanGetSpeed(current_fan)*100)/255;
    my_sprintf(tempstr, "%d%%", fs); 
  #else
    fs = fanSpeed[curIndex];
    my_sprintf(tempstr, "%d", fs);
  #endif 
  GUI_DispStringInPrect(&rectB[2], (u8 *)tempstr);                        //Fan value

  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&SpeedItems[current_speedID],3);                           //Speed / flow icon
  GUI_DispStringRight(pointID[3].x, pointID[3].y, (u8 *)SpeedID[current_speedID]);       //Speed / flow label
  GUI_SetColor(VAL_COLOR);
  my_sprintf(tempstr, "%d%s", speedGetPercent(current_speedID),"%"); 
  GUI_DispStringInPrect(&rectB[3], (u8 *)tempstr);                        //Speed / Flow value

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(GANTRYLBL_BKCOLOR);
  my_sprintf(tempstr, "   X: %.2f   Y: %.2f   Z: %.2f   ", xaxis, yaxis, zaxis);
  GUI_DispStringInPrect(&RecGantry,(u8 *)tempstr);
  
  GUI_RestoreColorDefault();
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
  gantryCmdWait = false;
}

void gantry_inc(int n, float val){
    //float* px = &val;
  switch (n)
  {
  case 0:
    xaxis += val;
    if ( xaxis > X_MAX_POS){
      xaxis = X_MAX_POS;
    }
    break;
  case 1:
    yaxis += val;
    if ( yaxis > Y_MAX_POS){
      yaxis = Y_MAX_POS;
    }
    break;
  case 2:
    zaxis += val;
    if ( zaxis > Z_MAX_POS){
      zaxis = Z_MAX_POS;
    }
    break;   
  default:
    break;
  }
}
void gantry_dec(int n, float val){
    //float* px = &val;
  switch (n)
  {
  case 0:
    xaxis -= val;
    if ( xaxis < X_MIN_POS){
      xaxis = X_MIN_POS;
    }
    break;
  case 1:
    yaxis -= val;
    if ( yaxis < Y_MIN_POS){
      yaxis = Y_MIN_POS;
    }
    break;
  case 2:
    zaxis -= val;
    if ( zaxis < Z_MIN_POS){
      zaxis = Z_MIN_POS;
    }
    break;   
  default:
    break;
  }
}

float getAxisLocation(u8 n){
  switch (n)
  {
  case 0:
    return xaxis;
  case 1:
    return yaxis;
  case 2:
    return zaxis;
  default:
    return xaxis;
  }
}


void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  memcpy(msgtitle, (char *)title, sizeof(msgtitle));
  memcpy(msgbody, (char *)msg, sizeof(msgbody));

  if (infoMenu.menu[infoMenu.cur] == menuStatus)
  {
    drawStatusScreenMsg();
  }
}

void drawStatusScreenMsg(void)
{
//GUI_ClearRect(RectInfo.x0,RectInfo.y0,RectInfo.x1,RectInfo.y1);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
 
  GUI_SetColor(INFOBOX_BKCOLOR);
  GUI_FillPrect(&RectInfo);

  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(RectInfo.x0 + STATUS_MSG_ICON_XOFFSET, RectInfo.y0 + STATUS_MSG_ICON_YOFFSET,IconCharSelect(ICONCHAR_INFO));

  GUI_DispString(RectInfo.x0 + BYTE_HEIGHT+ STATUS_MSG_TITLE_XOFFSET,RectInfo.y0 + STATUS_MSG_ICON_YOFFSET,(u8*)msgtitle); 
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);
  
  Scroll_CreatePara(&msgScroll, (u8 *)msgbody, &msgRect);

  GUI_RestoreColorDefault();
}

void scrollMsg(void){
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&msgScroll,CENTER);
  GUI_RestoreColorDefault();
}

void toggleTool(void)
{
  if (OS_GetTime() > nowTime + update_time)
  {
    if (EXTRUDER_NUM > 1)
    {
      current_Ext = (TOOL)((current_Ext + 1) % HEATER_NUM);
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
    
    if (infoHost.connected == true)
    {
      if (gantryCmdWait != true)
      {
        gantryCmdWait = true;
        storeCmd("M114\n");
      }
    }
    else
    {
      gantryCmdWait = false;
    }
  }
}

void menuStatus(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  GUI_SetBkColor(BACKGROUND_COLOR);
  //set_status_icon();
  menuDrawPage(&StatusItems);
  GUI_SetColor(GANTRYLBL_BKCOLOR);
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
