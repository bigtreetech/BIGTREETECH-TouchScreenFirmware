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

#define TOOL_TOGGLE_TIME 2000  // 1 seconds is 1000

#ifdef PORTRAIT_MODE
  #define XYZ_STATUS "X:%.2f Y:%.2f Z:%.2f"
#else
  #define XYZ_STATUS "   X: %.2f   Y: %.2f   Z: %.2f   "
#endif

const MENUITEMS statusItems = {
  // title
  LABEL_READY,
  // icon                          label
  {
    {ICON_STATUS_NOZZLE,           LABEL_NULL},
    {ICON_STATUS_BED,              LABEL_NULL},
    {ICON_STATUS_FAN,              LABEL_NULL},
    {ICON_STATUS_SPEED,            LABEL_NULL},
    #ifdef TFT70_V3_0
      {ICON_STATUS_FLOW,             LABEL_NULL},
      {ICON_MAINMENU,                LABEL_MAINMENU},
    #else
      {ICON_MAINMENU,                LABEL_MAINMENU},
      {ICON_NULL,                    LABEL_NULL},
    #endif
    {ICON_NULL,                    LABEL_NULL},
    {ICON_PRINT,                   LABEL_PRINT},
  }
};

const uint8_t bedIcons[2] = {ICON_STATUS_BED, ICON_STATUS_CHAMBER};

const uint8_t speedIcons[2] = {ICON_STATUS_SPEED, ICON_STATUS_FLOW};

static int8_t lastConnectionStatus = -1;
static bool msgNeedRefresh = false;

static char msgTitle[20];
static char msgBody[MAX_MSG_LENGTH];

const char *const speedID[2] = SPEED_ID;

// text position rectangles for Live icons
const GUI_POINT ss_title_point = {SS_ICON_WIDTH - BYTE_WIDTH / 2, SS_ICON_NAME_Y0};
const GUI_POINT ss_val_point   = {SS_ICON_WIDTH / 2, SS_ICON_VAL_Y0};
#ifdef TFT70_V3_0
  const GUI_POINT ss_val_point_2 = {SS_ICON_WIDTH / 2, SS_ICON_VAL_Y0_2};
#endif

// info box msg area
#ifdef PORTRAIT_MODE
  const  GUI_RECT msgRect = {START_X + 0.5 * ICON_WIDTH + 0 * SPACE_X + 2, ICON_START_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                             START_X + 2.5 * ICON_WIDTH + 1 * SPACE_X - 2, ICON_START_Y + 1 * ICON_HEIGHT + 0 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

  const GUI_RECT recGantry = {START_X - 3,                                SS_ICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                              START_X + 3 + 3 * ICON_WIDTH + 2 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};
#else
  const  GUI_RECT msgRect = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2, ICON_START_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                             START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2, ICON_START_Y + 2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

  const GUI_RECT recGantry = {START_X,                                SS_ICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                              START_X + 4 * ICON_WIDTH + 3 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};
#endif

void statusSetMsg(const uint8_t *title, const uint8_t *msg)
{
  strncpy_no_pad(msgTitle, (char *)title, sizeof(msgTitle));
  strncpy_no_pad(msgBody, (char *)msg, sizeof(msgBody));
  msgNeedRefresh = true;
}

void statusSetReady(void)
{
  strncpy_no_pad(msgTitle, (char *)textSelect(LABEL_STATUS), sizeof(msgTitle));

  if (infoHost.connected == false)
    strncpy_no_pad(msgBody, (char *)textSelect(LABEL_UNCONNECTED), sizeof(msgBody));
  else
    snprintf(msgBody, sizeof(msgBody), "%s %s", machine_type, (char *)textSelect(LABEL_READY));

  msgNeedRefresh = true;
}

void statusDraw(void)
{
  // icons and their values are updated one by one to reduce flicker/clipping
  char tempstr[45];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.lines[0].h_align = RIGHT;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].pos = ss_title_point;
  lvIcon.lines[0].font = SS_ICON_TITLE_FONT_SIZE;
  lvIcon.lines[0].fn_color = SS_NAME_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;  // default value

  lvIcon.enabled[1] = true;
  lvIcon.lines[1].h_align = CENTER;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].pos = ss_val_point;
  lvIcon.lines[1].font = SS_ICON_VAL_FONT_SIZE;
  lvIcon.lines[1].fn_color = SS_VAL_COLOR;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;  // default value

  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].pos = ss_val_point_2;
    lvIcon.lines[2].font = SS_ICON_VAL_FONT_SIZE_2;
    lvIcon.lines[2].fn_color = SS_VAL_COLOR_2;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;  // default value
  #endif

  #ifdef TFT70_V3_0
    char tempstr2[45];

    // TOOL / EXT
    lvIcon.iconIndex = ICON_STATUS_NOZZLE;
    lvIcon.lines[0].text = (uint8_t *)heatShortID[currentTool];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(0, &lvIcon, false);

    // BED / CHAMBER
    lvIcon.iconIndex = bedIcons[currentBCIndex];
    lvIcon.lines[0].text = (uint8_t *)heatShortID[BED + currentBCIndex];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(BED + currentBCIndex));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(BED + currentBCIndex));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(1, &lvIcon, infoSettings.chamber_en == 1);

    lvIcon.enabled[2] = false;
  #else
    // TOOL / EXT
    lvIcon.iconIndex = ICON_STATUS_NOZZLE;
    lvIcon.lines[0].text = (uint8_t *)heatShortID[currentTool];
    sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(0, &lvIcon, false);

    // BED
    lvIcon.iconIndex = bedIcons[currentBCIndex];
    lvIcon.lines[0].text = (uint8_t *)heatShortID[BED + currentBCIndex];
    sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(BED + currentBCIndex), heatGetTargetTemp(BED + currentBCIndex));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(1, &lvIcon, infoSettings.chamber_en == 1);
  #endif

  // FAN
  lvIcon.iconIndex = ICON_STATUS_FAN;
  lvIcon.lines[0].text = (uint8_t *)fanID[currentFan];

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%3d%%", fanGetCurPercent(currentFan));
  else
    sprintf(tempstr, "%3d", fanGetCurSpeed(currentFan));

  lvIcon.lines[1].text = (uint8_t *)tempstr;
  showLiveInfo(2, &lvIcon, false);

  #ifdef TFT70_V3_0
    // SPEED
    lvIcon.iconIndex = ICON_STATUS_SPEED;
    lvIcon.lines[0].text = (uint8_t *)speedID[0];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(0));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, false);

    // FLOW
    lvIcon.iconIndex = ICON_STATUS_FLOW;
    lvIcon.lines[0].text = (uint8_t *)speedID[1];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(1));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(4, &lvIcon, false);
  #else
    // SPEED / FLOW
    lvIcon.iconIndex = speedIcons[currentSpeedID];
    lvIcon.lines[0].text = (uint8_t *)speedID[currentSpeedID];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(currentSpeedID));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, true);
  #endif

  sprintf(tempstr, XYZ_STATUS, coordinateGetAxisActual(X_AXIS), coordinateGetAxisActual(Y_AXIS), coordinateGetAxisActual(Z_AXIS));

  #ifdef PORTRAIT_MODE
    int paddingWidth = ((recGantry.x1 - recGantry.x0) - (strlen(tempstr) * BYTE_WIDTH)) / 2;

    GUI_SetColor(GANTRY_XYZ_BG_COLOR);
    GUI_FillRect(recGantry.x0, recGantry.y0, recGantry.x0 + paddingWidth, recGantry.y1);  // left padding
    GUI_FillRect(recGantry.x1 - paddingWidth, recGantry.y0, recGantry.x1, recGantry.y1);  // right padding
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRY_XYZ_FONT_COLOR);
  GUI_SetBkColor(GANTRY_XYZ_BG_COLOR);
  GUI_DispStringInPrect(&recGantry, (uint8_t *)tempstr);

  GUI_RestoreColorDefault();
}

void statusDrawMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  IMAGE_ReadDisplay(rect_of_keySS[KEY_INFOBOX].x0, rect_of_keySS[KEY_INFOBOX].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BG_COLOR);
  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));

  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 (uint8_t *)msgTitle);

  GUI_SetBkColor(INFOMSG_BG_COLOR);
  GUI_FillPrect(&msgRect);
  Scroll_CreatePara(&scrollLine, (uint8_t *)msgBody, &msgRect);
  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
}

static inline void statusScrollMsg(void)
{
  GUI_SetBkColor(INFOMSG_BG_COLOR);
  GUI_SetColor(INFOMSG_FONT_COLOR);
  Scroll_DispString(&scrollLine, CENTER);
  GUI_RestoreColorDefault();
}

static inline void statusToggleTool(void)
{
  if (nextScreenUpdate(TOOL_TOGGLE_TIME))
  {
    // increment hotend index
    if (infoSettings.hotend_count > 1)
      currentTool = (currentTool + 1) % infoSettings.hotend_count;

    // switch bed/chamber index
    if (infoSettings.chamber_en == 1)
      TOGGLE_BIT(currentBCIndex, 0);

    // increment fan index
    if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    {
      do
      {
        currentFan = (currentFan + 1) % MAX_FAN_COUNT;
      } while (!fanIsValid(currentFan));
    }

    // switch speed/flow
    TOGGLE_BIT(currentSpeedID, 0);
    statusDraw();

    // gcode queries must be call after drawStatus
    coordinateQuery(MS_TO_SEC(TOOL_TOGGLE_TIME));
    speedQuery();
    ctrlFanQuery();
  }
}

void menuStatus(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  GUI_SetBkColor(infoSettings.bg_color);
  menuDrawPage(&statusItems);
  GUI_SetColor(GANTRY_XYZ_BG_COLOR);
  GUI_FillPrect(&recGantry);
  statusDraw();
  statusDrawMsg();

  while (MENU_IS(menuStatus))
  {
    if (infoHost.connected != lastConnectionStatus)
    {
      statusSetReady();
      lastConnectionStatus = infoHost.connected;
    }

    if (msgNeedRefresh)
      statusDrawMsg();

    statusScrollMsg();
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetCurrentIndex(LAST_NOZZLE);  // preselect last selected nozzle for "Heat" menu
        OPEN_MENU(menuHeat);
        break;

      case KEY_ICON_1:
        heatSetCurrentIndex(BED);  // preselect the bed for "Heat" menu
        OPEN_MENU(menuHeat);
        break;

      case KEY_ICON_2:
        OPEN_MENU(menuFan);
        break;

      case KEY_SPEEDMENU:
        SET_SPEEDMENUINDEX(0);
        OPEN_MENU(menuSpeed);
        break;

      #ifdef TFT70_V3_0
        case KEY_FLOWMENU:
          SET_SPEEDMENUINDEX(1);
          OPEN_MENU(menuSpeed);
          break;
      #endif

      case KEY_MAINMENU:
        OPEN_MENU(menuMain);
        break;

      case KEY_ICON_7:
        OPEN_MENU(menuPrint);
        break;

      case KEY_INFOBOX:
        OPEN_MENU(menuNotification);
      default:
        break;
    }

    statusToggleTool();
    loopProcess();
  }

  coordinateQueryTurnOff();  // disable position auto report, if any
}
