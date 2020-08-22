#include "StatusScreen.h"

//1 title, ITEM_PER_PAGE items (icon + label)
const MENUITEMS StatusItems = {
// title
LABEL_READY,
// icon                       label
 {{ICON_STATUS_NOZZLE,        LABEL_BACKGROUND},
  {ICON_STATUS_BED,           LABEL_BACKGROUND},
  {ICON_STATUS_FAN,           LABEL_BACKGROUND},
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
static bool msgNeedRefresh = false;

static char msgtitle[20];
static char msgbody[MAX_MSG_LENGTH];

uint8_t current_tool = NOZZLE0;
int current_fan = 0;
int current_speedID = 0;
const char* SpeedID[2] = SPEED_ID;
// text position rectangles for Live icons
//icon 0
const GUI_POINT ss_title_point = {ICON_WIDTH - BYTE_WIDTH/2, SSICON_NAME_Y0};
const GUI_POINT ss_val_point   = {ICON_WIDTH/2, SSICON_VAL_Y0};

//info rectangle
const GUI_RECT RectInfo = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X,  ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X,  ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y};

const  GUI_RECT msgRect ={START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2,   ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                          START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2,   ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

const GUI_RECT RecGantry = {START_X,                        1*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            4*ICON_WIDTH+3*SPACE_X+START_X, 1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y - STATUS_GANTRY_YOFFSET};


void drawTemperature(void)
{
  //icons and their values are updated one by one to reduce flicker/clipping
  char tempstr[20];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;

  lvIcon.lines[0].h_align = RIGHT;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = LCD_WHITE;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = ss_title_point;

  lvIcon.lines[1].h_align = CENTER;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = LCD_BLACK;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = ss_val_point;

  //TOOL / EXT
  lvIcon.lines[0].text =  (u8 *)heatDisplayID[current_tool];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(current_tool), heatGetTargetTemp(current_tool));
  lvIcon.lines[1].text = (u8 *)tempstr;
  showLiveInfo(0,&lvIcon,&StatusItems.items[0]);

  //BED
  lvIcon.lines[0].text =  (u8 *)heatDisplayID[BED];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
  lvIcon.lines[1].text = (u8 *)tempstr;
  showLiveInfo(1,&lvIcon,&StatusItems.items[1]);

  //FAN
  lvIcon.lines[0].text = (u8 *)fanID[current_fan];

  if (infoSettings.fan_percentage == 1)
  {
    sprintf(tempstr, "%d%%", fanGetSpeedPercent(current_fan));
  }
  else
  {
    sprintf(tempstr, "%d", fanGetSpeed(current_fan));
  }
  lvIcon.lines[1].text = (u8 *)tempstr;
  showLiveInfo(2,&lvIcon,&StatusItems.items[2]);

  //SPEED / flow
  lvIcon.lines[0].text =  (u8 *)SpeedID[current_speedID];
  sprintf(tempstr, "%d%%", speedGetPercent(current_speedID));
  lvIcon.lines[1].text = (u8 *)tempstr;
  showLiveInfo(3,&lvIcon,&SpeedItems[current_speedID]);


  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(infoSettings.status_xyz_bg_color);
  sprintf(tempstr, "   X: %.2f   Y: %.2f   Z: %.2f   ", coordinateGetAxisActual(X_AXIS), coordinateGetAxisActual(Y_AXIS), coordinateGetAxisActual(Z_AXIS));
  GUI_DispStringInPrect(&RecGantry,(u8 *)tempstr);

  GUI_RestoreColorDefault();
}

void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  strncpy(msgtitle, (char *)title, sizeof(msgtitle));
  strncpy(msgbody, (char *)msg, sizeof(msgbody));
  msgNeedRefresh = true;
}

void statusScreen_setReady(void)
{
  strncpy(msgtitle, (char *)textSelect(LABEL_STATUS), sizeof(msgtitle));
  if(infoHost.connected == false){
    strncpy(msgbody, (char *)textSelect(LABEL_UNCONNECTED), sizeof(msgbody));
  }
  else{
    strncpy(msgbody, (char *)machine_type, sizeof(msgbody));
    strcat(msgbody, " ");
    strcat(msgbody, (char *)textSelect(LABEL_READY));
  }

  msgNeedRefresh = true;
}

void drawStatusScreenMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(RectInfo.x0,RectInfo.y0,INFOBOX_WIDTH,INFOBOX_HEIGHT,INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(RectInfo.x0 + STATUS_MSG_ICON_XOFFSET, RectInfo.y0 + STATUS_MSG_ICON_YOFFSET,IconCharSelect(ICONCHAR_INFO));

  GUI_DispString(RectInfo.x0 + BYTE_HEIGHT+ STATUS_MSG_TITLE_XOFFSET,RectInfo.y0 + STATUS_MSG_ICON_YOFFSET,(u8*)msgtitle);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);

  Scroll_CreatePara(&msgScroll, (u8 *)msgbody, &msgRect);

  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
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
    if (infoSettings.hotend_count > 1)
    {
      current_tool = (current_tool+1) % infoSettings.hotend_count;
    }
    if (infoSettings.fan_count > 1)
    {
      current_fan = (current_fan + 1) % infoSettings.fan_count;
    }
    current_speedID = (current_speedID + 1) % 2;
    nextTime = OS_GetTimeMs() + update_time;
    drawTemperature();

    coordinateQuery();
    speedQuery();
  }
}

void menuStatus(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  GUI_SetBkColor(infoSettings.bg_color);
  menuDrawPage(&StatusItems);
  GUI_SetColor(infoSettings.status_xyz_bg_color);
  GUI_FillPrect(&RecGantry);
  drawTemperature();
  drawStatusScreenMsg();
  while (infoMenu.menu[infoMenu.cur] == menuStatus)
  {
    if(infoHost.connected != lastConnection_status){
      statusScreen_setReady();
      lastConnection_status = infoHost.connected;
    }
    if (msgNeedRefresh) {
      drawStatusScreenMsg();
    }
    scrollMsg();
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetCurrentIndex(current_tool);
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;
      case KEY_ICON_1:
        heatSetCurrentIndex(BED);
        infoMenu.menu[++infoMenu.cur] = menuHeat;
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
