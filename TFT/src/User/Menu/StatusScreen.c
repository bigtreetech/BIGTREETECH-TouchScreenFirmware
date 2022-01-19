//TG MODIFIED BY T.GIOIOSA
#include "StatusScreen.h"

#ifdef TFT70_V3_0
#define KEY_SPEEDMENU         KEY_ICON_3
#define KEY_FLOWMENU          (KEY_SPEEDMENU + 1)
#define KEY_MAINMENU          (KEY_FLOWMENU + 1)
#define SET_SPEEDMENUINDEX(x) setSpeedItemIndex(x)
#else
#define KEY_SPEEDMENU         KEY_ICON_3
#define KEY_MAINMENU          (KEY_SPEEDMENU + 1)
#define SET_SPEEDMENUINDEX(x)
#endif

MENUITEMS StatusItems = {         //TG 1/12/20 removed const so this menu can be dynamically modified
  // title
  LABEL_READY,
  // icon                          label
  {
	{ICON_STATUS_SPINDLE,          LABEL_BACKGROUND},    //TG 1/14/20 changed
    {ICON_STATUS_VACUUM,           LABEL_BACKGROUND},    //TG 1/14/20 changed
    {ICON_STATUS_FAN,              LABEL_BACKGROUND},
    {ICON_STATUS_SPEED,            LABEL_BACKGROUND},
    #ifdef TFT70_V3_0
      {ICON_STATUS_FLOW,             LABEL_BACKGROUND},
      {ICON_MAINMENU,                LABEL_MAINMENU},
    #else
      {ICON_MAINMENU,                LABEL_MAINMENU},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
    #endif
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_PRINT,                   LABEL_PRINT},
  }
};

const ITEM SpeedItems[2] = {
  // icon                        label
  {ICON_STATUS_SPEED,            LABEL_BACKGROUND},
  {ICON_STATUS_FLOW,             LABEL_BACKGROUND},
};

#define UPDATE_TOOL_TIME  2000 // 1 seconds is 1000  //TG 2/21/21 changed, was 2000
static u32 nextToolTime = 0;
SCROLL     msgScroll;
static int8_t lastConnection_status = -1;
static bool msgNeedRefresh = false;

static char msgtitle[20];
static char msgbody[MAX_MSG_LENGTH];
SCROLL msgScroll;

const char *const SpeedID[2] = SPEED_ID;
// text position rectangles for Live icons
//icon 0
const GUI_POINT ss_title_point = {SSICON_WIDTH - BYTE_WIDTH/2, SSICON_NAME_Y0}; // 95-24/2, 7 = 89,7
const GUI_POINT ss_val_point   = {SSICON_WIDTH/2, SSICON_VAL_Y0};  // icon x center, y 75 = 47.5,75
const GUI_POINT ss_val2_point = {SSICON_WIDTH - 3*BYTE_WIDTH-BYTE_WIDTH/4, SSICON_NAME_Y0 + BYTE_HEIGHT};

#ifdef TFT70_V3_0
  const GUI_POINT ss_val2_point = {SSICON_WIDTH/2, SSICON_VAL2_Y0};
#endif

//info box msg area
const  GUI_RECT msgRect = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2,   ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2,   ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

const GUI_RECT RecGantry = {START_X, SSICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            START_X + 4 * ICON_WIDTH + 3 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};

//TG code to update a single lvIcon text line on an icon. Up to three lines can be drawn on any icon, indexes 0,1,2
void drawSingleLiveIconLine(uint8_t currentToggleID) {
  //icons and their values are updated one by one to reduce flicker/clipping
  if(infoMenu.menu[infoMenu.cur] != menuStatus)
    return;
  
  char tempstr[45];
  LIVE_INFO lvIcon;
  lvIcon.enabled[1] = true;                         // single item
  lvIcon.lines[1].h_align = CENTER;                 // center justified
  lvIcon.lines[1].v_align = CENTER;                 // center aligned
  lvIcon.lines[1].fn_color = SSICON_VAL_COLOR;      // black
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;   // no background
  lvIcon.lines[1].pos = ss_val_point;               // value location
  lvIcon.lines[1].large_font = VAL_LARGE_FONT;      // false
  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;                      // no 3rd item
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].fn_color = SSICON_VAL2_COLOR;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
    lvIcon.lines[2].pos = ss_val2_point;
    lvIcon.lines[2].large_font = VAL2_LARGE_FONT;  
  #endif
  #ifdef TFT70_V3_0
    char tempstr2[45];
    TOOL / EXT
    lvIcon.lines[0].text = (u8 *)heatDisplayID[currentTool];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = (u8 *)tempstr2;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);
    //BED
    lvIcon.lines[0].text = (u8 *)heatDisplayID[BED];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(BED));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = (u8 *)tempstr2;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);
    lvIcon.enabled[2] = false;
  #else
    //TOOL / SPINDLE    //TG 2/17/21 updated for spindle, was for EXT, now alternates between Set/Cur speed every UPDATE_TOOL_TIME
    lvIcon.lines[0].text = (u8 *)spindleDisplayID[currentTool];
    sprintf(tempstr, infoSettings.cutter_disp_unit == MPCT ? "%d%%" : "%d", currentToggleID ? 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetSetSpeed(currentTool)) : 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetCurSpeed(currentTool))
           );
    lvIcon.lines[1].text = (u8 *)tempstr;
    // icon index(0-7), address of live icon data, address of current icon image, live icon line(0-2)
    showSingleLiveIconLine(0, &lvIcon, &StatusItems.items[0],1);
 #endif
}

//TG draws all the icons on the statusScreen
void drawAllLiveIconData(void)
{
  //icons and their values are updated one by one to reduce flicker/clipping
  char tempstr[45];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;                         // 1st item
  lvIcon.lines[0].h_align = RIGHT;                  // right-justified
  lvIcon.lines[0].v_align = TOP;                    // top aligned
  lvIcon.lines[0].fn_color = SSICON_NAME_COLOR;     // white
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;   // no background
  lvIcon.lines[0].pos = ss_title_point;             // name location
  lvIcon.lines[0].large_font = NAME_LARGE_FONT;     // false

  lvIcon.enabled[1] = true;                         // 2nd item
  lvIcon.lines[1].h_align = CENTER;                 // center justified
  lvIcon.lines[1].v_align = CENTER;                 // center aligned
  lvIcon.lines[1].fn_color = SSICON_VAL_COLOR;      // black
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;   // no background
  lvIcon.lines[1].pos = ss_val_point;               // value location
  lvIcon.lines[1].large_font = VAL_LARGE_FONT;      // false

  lvIcon.lines[2].h_align = LEFT;                   //TG 2/22/21 for "Set/Act" text on spindle speed
  lvIcon.lines[2].v_align = TOP;
  lvIcon.lines[2].fn_color = SSICON_NAME_COLOR;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = ss_val2_point;
  lvIcon.lines[2].large_font = NAME_LARGE_FONT;

  #ifdef TFT70_V3_0
    char tempstr2[45];

  //TOOL / EXT
    lvIcon.lines[0].text = (u8 *)heatDisplayID[currentTool];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = (u8 *)tempstr2;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);

    //BED
    lvIcon.lines[0].text = (u8 *)heatDisplayID[BED];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(BED));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = (u8 *)tempstr2;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);

    lvIcon.enabled[2] = false;
  #else

    //TOOL / SPINDLE    //TG 2/17/21 updated for spindle, was for EXT
    //the speed line[1] is also shown every 1000ms from Marlin RPM gate interval which calls drawSingleLiveIconLine()
    //the order of showing Set/Act is reversed with ref to currentSpeedID so that Speed/Flow icon will display actual
    //spindle Act rotation speed while the spindle icon is showing Set spindle speed.            
    lvIcon.lines[0].text = (u8 *)spindleDisplayID[currentTool];
    //sprintf(tempstr, "%d", currentSpeedID ? spindleGetCurSpeed(currentTool) : spindleGetSetSpeed(currentTool));
    sprintf(tempstr, infoSettings.cutter_disp_unit == MPCT ? "%d%%" : "%d", currentSpindleSpeedID ? 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetSetSpeed(currentTool)) : 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetCurSpeed(currentTool)) 
           );
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = currentSpindleSpeedID ? (u8*)"Set" : (u8*)"Act";
    
    lvIcon.enabled[2] = true; // only show line[2] for spindle/laser icon (0)
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);
    lvIcon.enabled[2] = false;// turn off for other icons

    //VACUUM //TG 2/17/21 updated for Vacuum, was for BED
    lvIcon.lines[0].text = (u8 *)vacuumDisplayID[0];
    sprintf(tempstr, "%s %s", (vacuumState & 2) == 2 ? (char*)"Auto" : (char*)"", (vacuumState & 1) == 1 ? (char*)"On" : (char*)"Off");
    lvIcon.lines[1].text = (u8 *)tempstr;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);

  #endif

  //FAN
  lvIcon.lines[0].text = (u8 *)fanID[currentFan];
  if (infoSettings.fan_percentage == 1)
  {
    sprintf(tempstr, "%d%%", fanGetCurPercent(currentFan));
  }
  else
  {
    sprintf(tempstr, "%d", fanGetCurSpeed(currentFan));
  }
  lvIcon.lines[1].text = (u8 *)tempstr;
  showLiveInfo(2, &lvIcon, &StatusItems.items[2]);

  #ifdef TFT70_V3_0
    //SPEED
    lvIcon.lines[0].text = (u8 *)SpeedID[0];
    sprintf(tempstr, "%d%%", speedGetCurPercent(0));
    lvIcon.lines[1].text = (u8 *)tempstr;
    showLiveInfo(3, &lvIcon, &SpeedItems[0]);

    //FLOW
    lvIcon.lines[0].text = (u8 *)SpeedID[1];
    sprintf(tempstr, "%d%%", speedGetCurPercent(1));
    lvIcon.lines[1].text = (u8 *)tempstr;
    showLiveInfo(4, &lvIcon, &SpeedItems[1]);

  #else
    //SPEED / flow  //TG 2/24/21 (flow no longer applies for CNC, so we put spindle speed/laser power in it's place)
    //TG TODO need to update this for Laser Power
    lvIcon.lines[0].text = (u8 *)SpeedID[currentSpeedID];
    if(currentSpeedID == 0) 
    { // show plotting speed
      sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedID));  
    }
    //TG 2/24/21 don't alternate plot/spindle speed by keeping currentSpeedID=0 in toggleTool(), makes the display confusing
    else
    { // show spindle speed
      sprintf(tempstr, infoSettings.cutter_disp_unit == MPCT ? "%d%%" : "%d", 
              convertSpeed_Marlin_2_LCD(currentTool,spindleGetCurSpeed(currentTool))
             );  
    }
    
    lvIcon.lines[1].text = (u8 *)tempstr;
    showLiveInfo(3, &lvIcon, &SpeedItems[currentSpeedID]);
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(infoSettings.status_xyz_bg_color);
  sprintf(tempstr, "   X: %.2f   Y: %.2f   Z: %.2f   ", coordinateGetAxisActual(X_AXIS), coordinateGetAxisActual(Y_AXIS),
          coordinateGetAxisActual(Z_AXIS));
  GUI_DispStringInPrect(&RecGantry, (u8 *)tempstr);

  GUI_RestoreColorDefault();
}

//TG displays messages in the statusText box
void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  strncpy(msgtitle, (char *)title, sizeof(msgtitle));
  strncpy(msgbody, (char *)msg, sizeof(msgbody));
  msgNeedRefresh = true;
}

//TG displays "Printer not connected" or "Ready" msg in the statusText box
void statusScreen_setReady(void)
{
  strncpy(msgtitle, (char *)textSelect(LABEL_STATUS), sizeof(msgtitle));
  if (infoHost.connected == false)
  {
    strncpy(msgbody, (char *)textSelect(LABEL_UNCONNECTED), sizeof(msgbody));
  }
  else
  {
    strncpy(msgbody, (char *)machine_type, sizeof(msgbody));
    strcat(msgbody, " ");
    strcat(msgbody, (char *)textSelect(LABEL_READY));
  }
  msgNeedRefresh = true;
}

//TG draws the statusText rectangle and puts in the scrolling message
void drawStatusScreenMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(rect_of_keySS[17].x0, rect_of_keySS[17].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[17].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(ICONCHAR_INFO));

  GUI_DispString(rect_of_keySS[17].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 (uint8_t *)msgtitle);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);

  Scroll_CreatePara(&msgScroll, (uint8_t *)msgbody, &msgRect);

  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
}

static inline void scrollMsg(void)
{
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&msgScroll,CENTER);
  GUI_RestoreColorDefault();
}

static inline void toggleTool(void)
{
  if (nextScreenUpdate(UPDATE_TOOL_TIME))
  { //TG commented this out not needed
    //if (infoSettings.hotend_count > 1)
    //{
    //  currentTool = (currentTool + 1) % infoSettings.hotend_count;
    //}
    if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    {
      currentFan = (currentFan + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
    }
    //currentSpeedID = (currentSpeedID + 1) % 2;  //TG 2/24/21 don't toggle plot speed / spindle speed(old flow)
    currentSpindleSpeedID = (currentSpindleSpeedID + 1) % 2;    //TG toggle spindle icon data
    nextToolTime = OS_GetTimeMs() + UPDATE_TOOL_TIME;
    drawAllLiveIconData();

    // gcode queries must be call after drawTemperature
    coordinateQuery();
    speedQuery();
    fanSpeedQuery();
  }
}

void menuStatus(void)   // shows the status screen menu, this is the first menu that comes up after reset unless in Classic Menu mode
{
   //TG 1/12/20 added this code to modify menu for laser option
  StatusItems.items[0].icon = (infoSettings.laser_mode == 1) ? ICON_STATUS_LASER : ICON_STATUS_SPINDLE;
  StatusItems.items[0].label.index = (infoSettings.laser_mode == 1) ? LABEL_BACKGROUND : LABEL_BACKGROUND;
  
  // so we can track when speed changes happen and display them quickly
  uint16_t lastCurrent = spindleGetCurSpeed(currentTool);
  uint16_t lastTarget = spindleGetSetSpeed(currentTool);
   
  KEY_VALUES key_num = KEY_IDLE;
  GUI_SetBkColor(infoSettings.bg_color);
  menuDrawPage(&StatusItems);                       // redraw and update the display
  GUI_SetColor(infoSettings.status_xyz_bg_color);
  GUI_FillPrect(&RecGantry);                        // draws gray rect across screen center for x,y,z coords status
  drawAllLiveIconData();                            // fills in speeds and coords on status screen
  drawStatusScreenMsg();                            // draws the status screen message center
  while (infoMenu.menu[infoMenu.cur] == menuStatus) // loop here until the current menu changes
  {
    if (infoHost.connected != lastConnection_status)
    {
      statusScreen_setReady();                      // prints the no printer attached or ready status
      lastConnection_status = infoHost.connected;
    }
    if (msgNeedRefresh)                             // time to update the status area?
    {
      drawStatusScreenMsg();                        // draws "Status" on message center
    }
    scrollMsg();                                    // scrolls current message
    key_num = menuKeyGetValue();                    // look for a key press (KEY_IDLE = no press), this just
    switch (key_num)                                // out on KEY_IDLE and runs backEnd/frontEnd loops
    {
      case KEY_ICON_0:
        if(infoSettings.laser_mode == 0)            //TG 1/12/20 modified this code to call tool menu for spindle/laser
        {
          toolSetCurrentIndex(currentTool);         //set index to current tool (this can cycle if more than 1 tool)
          infoMenu.menu[++infoMenu.cur] = menuSpindle;
        }
        else
          infoMenu.menu[++infoMenu.cur] = menuLaser;
        break;
      case KEY_ICON_1:
        toolSetCurrentIndex(BED);                   //TODO NEED TO CHECK THIS
        infoMenu.menu[++infoMenu.cur] = menuVacuum;
        break;
      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;
      case KEY_SPEEDMENU:
        SET_SPEEDMENUINDEX(0);
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;
      #ifdef TFT70_V3_0
        case KEY_FLOWMENU:
          SET_SPEEDMENUINDEX(1);
          infoMenu.menu[++infoMenu.cur] = menuSpeed;
          break;
      #endif
      case KEY_MAINMENU:
        infoMenu.menu[++infoMenu.cur] = cncMenu;
        break;
      case KEY_ICON_7:
        infoMenu.menu[++infoMenu.cur] = menuPrint;
        break;
      case KEY_INFOBOX:
        infoMenu.menu[++infoMenu.cur] = menuNotification;
      default:
        break;
    }

    // speed changes will come from keypad(target) or from "S0:" msg from Marlin, which updates CurSpeed
    // if there was a speed change(actual or target), capture it and update speeds in spindle icon
    if(lastCurrent != spindleGetCurSpeed(currentTool) || lastTarget != spindleGetSetSpeed(currentTool))
    {     
      lastCurrent = 0;
      drawSingleLiveIconLine(currentSpindleSpeedID);
      lastCurrent = spindleGetCurSpeed(currentTool);
      lastTarget = spindleGetSetSpeed(currentTool);
    }
    
    toggleTool();     // step through all hotends/fans if there are more than 1 
    loopProcess();    // handle the backEnd/frontEnd tasks


  }
}
