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

#define UPDATE_TOOL_TIME 2000  // 1 seconds is 1000

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

const ITEM BedItems[2] = {
  // icon                        label
  {ICON_STATUS_BED,              LABEL_BACKGROUND},
  {ICON_STATUS_CHAMBER,          LABEL_BACKGROUND},
};

const ITEM SpeedItems[2] = {
  // icon                        label
  {ICON_STATUS_SPEED,            LABEL_BACKGROUND},
  {ICON_STATUS_FLOW,             LABEL_BACKGROUND},
};

static int8_t lastConnection_status = -1;
static bool msgNeedRefresh = false;

static char msgtitle[20];
static char msgbody[MAX_MSG_LENGTH];

const char *const SpeedID[2] = SPEED_ID;

// text position rectangles for Live icons
const GUI_POINT ss_title_point = {SSICON_WIDTH - BYTE_WIDTH / 2, SSICON_NAME_Y0};
const GUI_POINT ss_val_point   = {SSICON_WIDTH / 2, SSICON_VAL_Y0};
#ifdef TFT70_V3_0
  const GUI_POINT ss_val2_point = {SSICON_WIDTH/2, SSICON_VAL2_Y0};
#endif

// info box msg area
const  GUI_RECT msgRect = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2, ICON_START_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2, ICON_START_Y + 2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

const GUI_RECT RecGantry = {START_X,                                SSICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            START_X + 4 * ICON_WIDTH + 3 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};

void drawLiveText(uint8_t index, LIVE_INFO *lvIcon, const ITEM *lvItem)
{
  const ITEM *item = (lvItem != NULL ? lvItem : &StatusItems.items[index]);

  #if LIVE_TEXT_BG_COLOR_STATUS > 0  // it allows to eliminate flickering on alternating icons
    GUI_POINT p = getIconStartPoint(index);
    LIVE_DATA *lines = lvIcon->lines;

    #ifdef TFT70_V3_0
      #if LIVE_TEXT_BG_COLOR_STATUS != 5
        bool isLine2 = lvIcon->enabled[2];  // save original value
      #endif

      uint16_t y0 = lines[2].pos.y + (BYTE_HEIGHT / 2);
    #endif

    #define W0 (lines[0].pos.x - (3 * BYTE_WIDTH))   // width of the partial changed icon / X of live text 1
    #define H0 (lines[1].pos.y - (BYTE_HEIGHT / 2))  // height of the partial changed icon / Y of live text 2

    #if LIVE_TEXT_BG_COLOR_STATUS == 1 || LIVE_TEXT_BG_COLOR_STATUS == 2  // apply to live text 1
      // draw live text 1
      lvIcon->enabled[0] = true;
      lvIcon->enabled[1] = false;
      #ifdef TFT70_V3_0
        lvIcon->enabled[2] = false;
      #endif

      #if LIVE_TEXT_BG_COLOR_STATUS == 1  // if icon background colors has to be used
        showLiveInfo(index, lvIcon, item);
      #else  // if sampled icon background uniform color has to be used
        ICON_PrepareRead(p.x, p.y, item->icon);
        lines[0].bk_color = ICON_ReadPixel(p.x + lines[0].pos.x, p.y + lines[0].pos.y);  // sample icon background uniform color
        lines[0].text_mode = GUI_TEXTMODE_NORMAL;                                        // override default value (transparency)
        ICON_PrepareReadEnd();

        showLiveInfo(index, lvIcon, NULL);  // use also live text overridden values
      #endif

      // draw live text 2 (and live text 3 if TFT70)
      lvIcon->enabled[0] = false;
      lvIcon->enabled[1] = true;
      #ifdef TFT70_V3_0
        lvIcon->enabled[2] = isLine2;  // restore original value
      #endif

      if (lvItem != NULL)  // if icon has to be drawn then transparency has to be used
      {
        #ifdef TFT70_V3_0
          ICON_PartialReadDisplay(p.x, p.y + y0, W0, -1, item->icon, 0, y0);  // draw the partial changed icon
          ICON_PartialReadDisplay(p.x, p.y, -1, y0, item->icon, 0, 0);        // draw the partial live text 2 icon
        #else
          int16_t h0 = H0;

          ICON_PartialReadDisplay(p.x, p.y, W0, h0, item->icon, 0, 0);        // draw the partial changed icon
          ICON_PartialReadDisplay(p.x, p.y + h0, -1, -1, item->icon, 0, h0);  // draw the partial live text 2 icon
        #endif

        showLiveInfo(index, lvIcon, NULL);  // use also live text default values (transparency)
      }
      else  // if no icon has to be drawn then icon background colors has to be used
      {
        showLiveInfo(index, lvIcon, item);
      }
    #elif LIVE_TEXT_BG_COLOR_STATUS == 3 || LIVE_TEXT_BG_COLOR_STATUS == 4  // apply to live text 2
      // draw live text 2 (and live text 3 if TFT70)
      lvIcon->enabled[0] = false;
      lvIcon->enabled[1] = true;

      #if LIVE_TEXT_BG_COLOR_STATUS == 3  // if icon background colors has to be used
        showLiveInfo(index, lvIcon, item);
      #else  // if sampled icon background uniform color has to be used
        ICON_PrepareRead(p.x, p.y, item->icon);
        lines[1].bk_color = ICON_ReadPixel(p.x + lines[1].pos.x, p.y + lines[1].pos.y);  // sample icon background uniform color
        lines[1].text_mode = GUI_TEXTMODE_NORMAL;                                        // override default value (transparency)
        #ifdef TFT70_V3_0
          if (isLine2)
          {
            lines[2].bk_color = ICON_ReadPixel(p.x + lines[2].pos.x, p.y + lines[2].pos.y);  // sample icon background uniform color
            lines[2].text_mode = GUI_TEXTMODE_NORMAL;                                        // override default value (transparency)
          }
        #endif
        ICON_PrepareReadEnd();

        showLiveInfo(index, lvIcon, NULL);  // use also live text overridden values
      #endif

      // draw live text 1
      lvIcon->enabled[0] = true;
      lvIcon->enabled[1] = false;
      #ifdef TFT70_V3_0
        lvIcon->enabled[2] = false;
      #endif

      if (lvItem != NULL)  // if icon has to be drawn then transparency has to be used
      {
        #ifdef TFT70_V3_0
          ICON_PartialReadDisplay(p.x, p.y + y0, -1, -1, item->icon, 0, y0);  // draw the partial changed icon and live text 1 icon
        #else
          ICON_PartialReadDisplay(p.x, p.y, -1, H0, item->icon, 0, 0);        // draw the partial changed icon and live text 1 icon
        #endif

        showLiveInfo(index, lvIcon, NULL);  // use also live text default values (transparency)
      }
      else  // if no icon has to be drawn then icon background colors has to be used
      {
        showLiveInfo(index, lvIcon, item);
      }

      #ifdef TFT70_V3_0
        lvIcon->enabled[2] = isLine2;  // restore original value
      #endif
    #else  // apply to both live text 1 and live text 2
      // draw both live text 1 and live text 2 (and live text 3 if TFT70)
      #if LIVE_TEXT_BG_COLOR_STATUS == 5  // if icon background colors has to be used
        showLiveInfo(index, lvIcon, item);
      #else  // if sampled icon background uniform color has to be used
        ICON_PrepareRead(p.x, p.y, item->icon);
        lines[0].bk_color = ICON_ReadPixel(p.x + lines[0].pos.x, p.y + lines[0].pos.y);  // sample icon background uniform color
        lines[0].text_mode = GUI_TEXTMODE_NORMAL;                                        // override default value (transparency)
        lines[1].bk_color = ICON_ReadPixel(p.x + lines[1].pos.x, p.y + lines[1].pos.y);  // sample icon background uniform color
        lines[1].text_mode = GUI_TEXTMODE_NORMAL;                                        // override default value (transparency)
        #ifdef TFT70_V3_0
          if (isLine2)
          {
            lines[2].bk_color = ICON_ReadPixel(p.x + lines[2].pos.x, p.y + lines[2].pos.y);  // sample icon background uniform color
            lines[2].text_mode = GUI_TEXTMODE_NORMAL;                                        // override default value (transparency)
          }
        #endif
        ICON_PrepareReadEnd();

        showLiveInfo(index, lvIcon, NULL);  // use also live text overridden values
      #endif

      if (lvItem != NULL)  // if icon has to be drawn
      {
        #ifdef TFT70_V3_0
          ICON_PartialReadDisplay(p.x, p.y + y0, W0, -1, item->icon, 0, y0);  // draw the partial changed icon
        #else
          ICON_PartialReadDisplay(p.x, p.y, W0, H0, item->icon, 0, 0);        // draw the partial changed icon
        #endif
      }
    #endif
  #else
    if (lvItem != NULL)  // if icon has to be drawn then transparency has to be used
    {
      menuDrawIconOnly(item, index);      // draw the full changed icon
      showLiveInfo(index, lvIcon, NULL);  // use also live text default values (transparency)
    }
    else  // if no icon has to be drawn then icon background colors has to be used
    {
      showLiveInfo(index, lvIcon, item);  // use also icon background colors
    }
  #endif
}

void drawStatus(void)
{
  // icons and their values are updated one by one to reduce flicker/clipping
  char tempstr[45];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.lines[0].h_align = RIGHT;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].pos = ss_title_point;
  lvIcon.lines[0].font = NAME_LARGE_FONT;
  lvIcon.lines[0].fn_color = SSICON_NAME_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;  // default value

  lvIcon.enabled[1] = true;
  lvIcon.lines[1].h_align = CENTER;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].pos = ss_val_point;
  lvIcon.lines[1].font = VAL_LARGE_FONT;
  lvIcon.lines[1].fn_color = SSICON_VAL_COLOR;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;  // default value

  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].pos = ss_val2_point;
    lvIcon.lines[2].font = VAL2_LARGE_FONT;
    lvIcon.lines[2].fn_color = SSICON_VAL2_COLOR;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;  // default value
  #endif

  #ifdef TFT70_V3_0
    char tempstr2[45];

    // TOOL / EXT
    lvIcon.lines[0].text = (uint8_t *)heatShortID[currentTool];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    drawLiveText(0, &lvIcon, NULL);

    // BED / CHAMBER
    lvIcon.lines[0].text = (uint8_t *)heatShortID[BED + currentBCIndex];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(BED + currentBCIndex));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(BED + currentBCIndex));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    drawLiveText(1, &lvIcon, (infoSettings.chamber_en == 1 ? &BedItems[currentBCIndex] : NULL));

    lvIcon.enabled[2] = false;
  #else
    // TOOL / EXT
    lvIcon.lines[0].text = (uint8_t *)heatShortID[currentTool];
    sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    drawLiveText(0, &lvIcon, NULL);

    // BED
    lvIcon.lines[0].text = (uint8_t *)heatShortID[BED + currentBCIndex];
    sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(BED + currentBCIndex), heatGetTargetTemp(BED + currentBCIndex));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    drawLiveText(1, &lvIcon, (infoSettings.chamber_en == 1 ? &BedItems[currentBCIndex] : NULL));
  #endif

  // FAN
  lvIcon.lines[0].text = (uint8_t *)fanID[currentFan];

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%3d%%", fanGetCurPercent(currentFan));
  else
    sprintf(tempstr, "%3d", fanGetCurSpeed(currentFan));

  lvIcon.lines[1].text = (uint8_t *)tempstr;
  drawLiveText(2, &lvIcon, NULL);

  #ifdef TFT70_V3_0
    // SPEED
    lvIcon.lines[0].text = (uint8_t *)SpeedID[0];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(0));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    drawLiveText(3, &lvIcon, NULL);

    // FLOW
    lvIcon.lines[0].text = (uint8_t *)SpeedID[1];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(1));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    drawLiveText(4, &lvIcon, NULL);
  #else
    // SPEED / FLOW
    lvIcon.lines[0].text = (uint8_t *)SpeedID[currentSpeedID];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(currentSpeedID));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    drawLiveText(3, &lvIcon, &SpeedItems[currentSpeedID]);
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

  IMAGE_ReadDisplay(rect_of_keySS[KEY_INFOBOX].x0, rect_of_keySS[KEY_INFOBOX].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));

  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
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

    // switch bed/chamber index
    if (infoSettings.chamber_en == 1)
      currentBCIndex = (currentBCIndex + 1) % 2;

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
    drawStatus();

    // gcode queries must be call after drawStatus
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
  drawStatus();
  drawStatusScreenMsg();

  while (MENU_IS(menuStatus))
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
        OPEN_MENU(menuHeat);
        break;

      case KEY_ICON_1:
        heatSetCurrentIndex(BED + currentBCIndex);
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

    toggleTool();
    loopProcess();
  }
  // disable position auto report
  coordinateQuery(0);
}
