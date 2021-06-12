#include "StatusScreen.h"
#include "includes.h"

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

const MENUITEMS StatusItems = {
  // title
  LABEL_READY,
  // icon                          label
  {
    {ICON_STATUS_NOZZLE,           LABEL_BACKGROUND},
    {ICON_STATUS_BED,              LABEL_BACKGROUND},
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

#define UPDATE_TOOL_TIME 2000  // 1 seconds is 1000
static int8_t lastConnection_status = -1;
static bool msgNeedRefresh = false;

static char msgtitle[20];
static char msgbody[MAX_MSG_LENGTH];

const char *const SpeedID[2] = SPEED_ID;
// text position rectangles for Live icons
// icon 0
const GUI_POINT ss_title_point = {SSICON_WIDTH - BYTE_WIDTH / 2, SSICON_NAME_Y0};
const GUI_POINT ss_val_point   = {SSICON_WIDTH / 2, SSICON_VAL_Y0};
#ifdef TFT70_V3_0
  const GUI_POINT ss_val2_point = {SSICON_WIDTH/2, SSICON_VAL2_Y0};
#endif

// info box msg area
const  GUI_RECT msgRect = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2,   ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2,   ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

const GUI_RECT RecGantry = {START_X,                                SSICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            START_X + 4 * ICON_WIDTH + 3 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};

void drawTemperature(void)
{
  // icons and their values are updated one by one to reduce flicker/clipping
  char tempstr[45];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.lines[0].h_align = RIGHT;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = SSICON_NAME_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = ss_title_point;
  lvIcon.lines[0].font = NAME_LARGE_FONT;

  lvIcon.enabled[1] = true;
  lvIcon.lines[1].h_align = CENTER;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = SSICON_VAL_COLOR;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = ss_val_point;
  lvIcon.lines[1].font = VAL_LARGE_FONT;

  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].fn_color = SSICON_VAL2_COLOR;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
    lvIcon.lines[2].pos = ss_val2_point;
    lvIcon.lines[2].font = VAL2_LARGE_FONT;
  #endif

  #ifdef TFT70_V3_0
    char tempstr2[45];

    // TOOL / EXT
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[currentTool];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);

    // BED
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[BED];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(BED));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);

    lvIcon.enabled[2] = false;
  #else
    // TOOL / EXT
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[currentTool];
    sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);

    // BED
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[BED];
    sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);
  #endif

  // FAN
  lvIcon.lines[0].text = (uint8_t *)fanID[currentFan];

  if (infoSettings.fan_percentage == 1)
  {
    sprintf(tempstr, "%3d%%", fanGetCurPercent(currentFan));
  }
  else
  {
    sprintf(tempstr, "%3d", fanGetCurSpeed(currentFan));
  }
  lvIcon.lines[1].text = (uint8_t *)tempstr;
  showLiveInfo(2, &lvIcon, &StatusItems.items[2]);

  #ifdef TFT70_V3_0
    // SPEED
    lvIcon.lines[0].text = (uint8_t *)SpeedID[0];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(0));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, &SpeedItems[0]);

    // FLOW
    lvIcon.lines[0].text = (uint8_t *)SpeedID[1];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(1));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(4, &lvIcon, &SpeedItems[1]);
  #else
    // SPEED / FLOW
    lvIcon.lines[0].text = (uint8_t *)SpeedID[currentSpeedID];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(currentSpeedID));
    lvIcon.lines[1].text = (uint8_t *)tempstr;

    menuDrawIconOnly(&SpeedItems[currentSpeedID], 3);
    showLiveInfo(3, &lvIcon, &SpeedItems[currentSpeedID]);
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(infoSettings.status_xyz_bg_color);
  sprintf(tempstr, "   X: %.2f   Y: %.2f   Z: %.2f   ", coordinateGetAxisActual(X_AXIS), coordinateGetAxisActual(Y_AXIS),
          coordinateGetAxisActual(Z_AXIS));
  GUI_DispStringInPrect(&RecGantry, (uint8_t *)tempstr);

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

void drawStatusScreenMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  IMAGE_ReadDisplay(rect_of_keySS[17].x0, rect_of_keySS[17].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[17].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));

  GUI_DispString(rect_of_keySS[17].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 (uint8_t *)msgtitle);

  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);
  Scroll_CreatePara(&scrollLine, (uint8_t *)msgbody, &msgRect);
  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
}

static inline void scrollMsg(void)
{
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&scrollLine, CENTER);
  GUI_RestoreColorDefault();
}

static inline void toggleTool(void)
{
  if (nextScreenUpdate(UPDATE_TOOL_TIME))
  {
    // increment hotend index
    if (infoSettings.hotend_count > 1)
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
    // increment fan index
    if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    {
      do
      {
        currentFan = (currentFan + 1) % MAX_FAN_COUNT;
      } while (!fanIsValid(currentFan));
    }
    // switch speed/flow
    currentSpeedID = (currentSpeedID + 1) % 2;
    drawTemperature();

    // gcode queries must be call after drawTemperature
    coordinateQuery(UPDATE_TOOL_TIME / 1000);
    speedQuery();
    ctrlFanQuery();
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
    if (infoHost.connected != lastConnection_status)
    {
      statusScreen_setReady();
      lastConnection_status = infoHost.connected;
    }

    if (msgNeedRefresh)
      drawStatusScreenMsg();

    scrollMsg();
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetCurrentIndex(currentTool);
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_1:
        heatSetCurrentIndex(BED);
        infoMenu.menu[++infoMenu.cur] = menuHeat;
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
        infoMenu.menu[++infoMenu.cur] = menuMain;
        break;

      case KEY_ICON_7:
        infoMenu.menu[++infoMenu.cur] = menuPrint;
        break;

      case KEY_INFOBOX:
        infoMenu.menu[++infoMenu.cur] = menuNotification;
      default:
        break;
    }

    toggleTool();
    loopProcess();
  }
  // disable position auto report
  coordinateQuery(0);
}
