#include "StatusScreen.h"

//1 title, ITEM_PER_PAGE items (icon + label)
const MENUITEMS StatusItems = {
// title
LABEL_READY,
// icon                       label
 {{ICON_STATUS_NOZZLE,         LABEL_BACKGROUND},
  {ICON_STATUS_BED,            LABEL_BACKGROUND},
  {ICON_STATUS_FAN,            LABEL_BACKGROUND},
  {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
  {ICON_MAINMENU,             LABEL_MAINMENU},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_PRINT,                LABEL_PRINT},}
};

const ITEM ToolItems[3] = {
// icon                       label
  {ICON_STATUS_NOZZLE,         LABEL_BACKGROUND},
  {ICON_STATUS_BED,            LABEL_BACKGROUND},
  {ICON_STATUS_FAN,            LABEL_BACKGROUND},
};
const ITEM SpeedItems[2] = {
// icon                       label
  {ICON_STATUS_SPEED,         LABEL_BACKGROUND},
  {ICON_STATUS_FLOW,          LABEL_BACKGROUND},
};

static u32 nextTime = 0;
static u32 update_time = 2000; // 1 seconds is 1000
SCROLL     msgScroll;
static int lastConnection_status = -1;
static bool booted = false;

static char msgtitle[20];
static char msgbody[512];

static float xaxis;
static float yaxis;
static float zaxis;
static bool gantryCmdWait = false;

TOOL current_tool = NOZZLE0;
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
  menuDrawIconOnly(&ToolItems[0],0);                                                   //Ext icon
  GUI_DispStringRight(pointID[0].x, pointID[0].y, (u8 *)heatDisplayID[current_tool]);   //Ext label

  GUI_SetColor(VAL_COLOR);
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(current_tool), heatGetTargetTemp(current_tool));
  GUI_DispStringInPrect(&rectB[0], (u8 *)tempstr);                            //Ext value

  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&ToolItems[1],1);                                          //Bed icon
  GUI_DispStringRight(pointID[1].x, pointID[1].y, (u8 *)heatDisplayID[BED]);  //Bed label
  GUI_SetColor(VAL_COLOR);
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
  GUI_DispStringInPrect(&rectB[1], (u8 *)tempstr);                            //Bed value

  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&ToolItems[2],2);                                          //Fan icon
  GUI_DispStringRight(pointID[2].x, pointID[2].y, (u8 *)fanID[current_fan]);  //Fan label
  GUI_SetColor(VAL_COLOR);

  u8 fs;
  if (infoSettings.fan_percentage == 1)
  {
    fs = (fanGetSpeed(current_fan) * 100) / 255;
    my_sprintf(tempstr, "%d%%", fs);
  }
  else
  {
    fs = fanGetSpeed(current_fan);
    my_sprintf(tempstr, "%d", fs);
  }

  GUI_DispStringInPrect(&rectB[2], (u8 *)tempstr);                                //Fan value

  GUI_SetColor(HEADING_COLOR);
  menuDrawIconOnly(&SpeedItems[current_speedID],3);                               //Speed / flow icon
  GUI_DispStringRight(pointID[3].x, pointID[3].y, (u8 *)SpeedID[current_speedID]);//Speed / flow label
  GUI_SetColor(VAL_COLOR);
  my_sprintf(tempstr, "%d%s", speedGetPercent(current_speedID),"%");
  GUI_DispStringInPrect(&rectB[3], (u8 *)tempstr);                                //Speed / Flow value

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(lcd_colors[infoSettings.status_xyz_bg_color]);
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
    if ( xaxis > infoSettings.machine_size_max[X_AXIS]){
      xaxis = infoSettings.machine_size_max[X_AXIS];
    }
    break;
  case 1:
    yaxis += val;
    if ( yaxis > infoSettings.machine_size_max[Y_AXIS]){
      yaxis = infoSettings.machine_size_max[Y_AXIS];
    }
    break;
  case 2:
    zaxis += val;
    if ( zaxis > infoSettings.machine_size_max[Z_AXIS]){
      zaxis = infoSettings.machine_size_max[Z_AXIS];
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
    if ( xaxis < infoSettings.machine_size_min[X_AXIS]){
      xaxis = infoSettings.machine_size_min[X_AXIS];
    }
    break;
  case 1:
    yaxis -= val;
    if ( yaxis < infoSettings.machine_size_min[Y_AXIS]){
      yaxis = infoSettings.machine_size_min[Y_AXIS];
    }
    break;
  case 2:
    zaxis -= val;
    if ( zaxis < infoSettings.machine_size_min[Z_AXIS]){
      zaxis = infoSettings.machine_size_min[Z_AXIS];
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
  strncpy(msgtitle, (char *)title, sizeof(msgtitle));
  strncpy(msgbody, (char *)msg, sizeof(msgbody));

  if (infoMenu.menu[infoMenu.cur] == menuStatus && booted == true)
  {
    drawStatusScreenMsg();
  }
}

void drawStatusScreenMsg(void)
{
//GUI_ClearRect(RectInfo.x0,RectInfo.y0,RectInfo.x1,RectInfo.y1);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(RectInfo.x0,RectInfo.y0,INFOBOX_WIDTH,INFOBOX_HEIGHT,INFOBOX_ADDR);
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
  if (OS_GetTimeMs() > nextTime)
  {
    if (infoSettings.tool_count > 1)
    {
      current_tool = (TOOL)((current_tool+1) % HEATER_COUNT);
      current_tool = (current_tool == 0) ? 1 : current_tool;
    }
    if (infoSettings.fan_count > 1)
    {
      current_fan = (current_fan + 1) % infoSettings.fan_count;
    }
    current_speedID = (current_speedID + 1) % 2;
    nextTime = OS_GetTimeMs() + update_time;
    drawTemperature();

    if (infoHost.connected == true)
    {
      if (gantryCmdWait != true)
      {
        gantryCmdWait = true;
        storeCmd("M114\n");
        storeCmd("M220\n");
        storeCmd("M221\n");
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
  booted = true;
  KEY_VALUES key_num = KEY_IDLE;
  GUI_SetBkColor(lcd_colors[infoSettings.bg_color]);
  //set_status_icon();
  menuDrawPage(&StatusItems);
  GUI_SetColor(lcd_colors[infoSettings.status_xyz_bg_color]);
      //GUI_ClearPrect(&RecGantry);
  GUI_FillPrect(&RecGantry);
  drawTemperature();
  drawStatusScreenMsg();
  while (infoMenu.menu[infoMenu.cur] == menuStatus)
  {
    if(infoHost.connected != lastConnection_status){
      if(infoHost.connected == false){
        statusScreen_setMsg(textSelect(LABEL_STATUS), textSelect(LABEL_UNCONNECTED));
      }
      else{
        statusScreen_setMsg(textSelect(LABEL_STATUS), textSelect(LABEL_READY));
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
        infoMenu.menu[++infoMenu.cur] = unifiedMenu;
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
