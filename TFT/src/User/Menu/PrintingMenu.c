#include "Printing.h"
#include "includes.h"

const GUI_RECT printinfo_val_rect[6] = {
  {PS_ICON_VAL_X, PS_ICON_VAL_Y, PS_ICON_VAL_LG_EX, PS_ICON_VAL_Y + BYTE_HEIGHT},
  {PS_ICON_VAL_X, PS_ICON_VAL_Y, PS_ICON_VAL_LG_EX, PS_ICON_VAL_Y + BYTE_HEIGHT},
  {PS_ICON_VAL_X, PS_ICON_VAL_Y, PS_ICON_VAL_SM_EX, PS_ICON_VAL_Y + BYTE_HEIGHT},
  {PS_ICON_VAL_X, PS_ICON_VAL_Y, PS_ICON_VAL_LG_EX, PS_ICON_VAL_Y + BYTE_HEIGHT},
  {PS_ICON_VAL_X, PS_ICON_VAL_Y, PS_ICON_VAL_LG_EX, PS_ICON_VAL_Y + BYTE_HEIGHT},
  {PS_ICON_VAL_X, PS_ICON_VAL_Y, PS_ICON_VAL_SM_EX, PS_ICON_VAL_Y + BYTE_HEIGHT}
};

#define PROGRESS_BAR_RAW_X0   (START_X)                             // X0 aligned to first icon
#ifdef PORTRAIT_MODE
  #define PROGRESS_BAR_RAW_X1 (START_X + 3*ICON_WIDTH + 2*SPACE_X)  // X1 aligned to last icon
#else
  #define PROGRESS_BAR_RAW_X1 (START_X + 4*ICON_WIDTH + 3*SPACE_X)  // X1 aligned to last icon
#endif

#ifdef MARKED_PROGRESS_BAR
  #define PROGRESS_BAR_DELTA_X ((PROGRESS_BAR_RAW_X1 - PROGRESS_BAR_RAW_X0) % 10)  // use marked progress bar. Width rounding factor multiple of 10 slices
#else
  #define PROGRESS_BAR_DELTA_X 2                                                   // use standard progress bar. Reserve a 2 pixels width for vertical borders
#endif

// progress bar rounded and aligned to center of icons
#define PROGRESS_BAR_X0          (PROGRESS_BAR_RAW_X0 + PROGRESS_BAR_DELTA_X - PROGRESS_BAR_DELTA_X / 2)
#define PROGRESS_BAR_X1          (PROGRESS_BAR_RAW_X1 - PROGRESS_BAR_DELTA_X / 2)

#define PROGRESS_BAR_FULL_WIDTH  (PROGRESS_BAR_X1 - PROGRESS_BAR_X0)  // 100% progress bar width
#define PROGRESS_BAR_SLICE_WIDTH (PROGRESS_BAR_FULL_WIDTH / 10)       // 10% progress bar width

#ifdef PORTRAIT_MODE
  const GUI_RECT progressBar = {PROGRESS_BAR_X0, TITLE_END_Y + 1,
                                PROGRESS_BAR_X1, PS_ICON_START_Y - PS_ICON_SPACE_Y - 1};
#else
  const GUI_RECT progressBar = {PROGRESS_BAR_X0, PS_ICON_START_Y + PS_ICON_HEIGHT * 2 + PS_ICON_SPACE_Y * 2 + 1,
                                PROGRESS_BAR_X1, ICON_START_Y + ICON_HEIGHT + SPACE_Y - PS_ICON_SPACE_Y - 1};
#endif

enum
{
  LIVE_INFO_ICON = (1 << 0),
  LIVE_INFO_TOP_ROW = (1 << 1),
  LIVE_INFO_BOTTOM_ROW = (1 << 2),
};

const uint8_t printingIcon[] = {ICON_PRINTING_NOZZLE, ICON_PRINTING_BED,    ICON_PRINTING_FAN,
                                ICON_PRINTING_TIMER,  ICON_PRINTING_ZLAYER, ICON_PRINTING_SPEED};

const uint8_t printingIcon2nd[] = {ICON_PRINTING_CHAMBER, ICON_PRINTING_FLOW};

const char *const speedId[2] = {"Speed", "Flow "};

#define TOGGLE_TIME     2000  // 1 seconds is 1000
#define LAYER_DELTA     0.1  // minimal layer height change to update the layer display (avoid congestion in vase mode)
#define LAYER_TITLE     "Layer"
#define MAX_TITLE_LEN   70
#define TIME_FORMAT_STR "%02u:%02u:%02u"

bool hasFilamentData;
PROGRESS_DISPLAY progDisplayType;
LAYER_TYPE layerDisplayType;
char title[MAX_TITLE_LEN] = "";

enum
{
  PRINT_ICON = (1 << 0),
  PRINT_TOP_ROW = (1 << 1),
  PRINT_BOTTOM_ROW = (1 << 2),
};

enum
{
  ICON_POS_EXT = 0,
  ICON_POS_BED,
  ICON_POS_FAN,
  ICON_POS_TIM,
  ICON_POS_Z,
  ICON_POS_SPD,
};

const ITEM itemIsPause[2] = {
  // icon                        label
  {ICON_PAUSE,                   LABEL_PAUSE},
  {ICON_RESUME,                  LABEL_RESUME},
};

const ITEM itemIsPrinting[3] = {
  // icon                        label
  {ICON_NULL,                    LABEL_NULL},
  {ICON_MAINMENU,                LABEL_MAIN_SCREEN},
  {ICON_BACK,                    LABEL_BACK},
};

static void setLayerHeightText(char * layer_height_txt)
{
  float layer_height;
  layer_height = coordinateGetAxis(Z_AXIS);
  if (layer_height > 0)
  {
    sprintf(layer_height_txt, "%6.2fmm", layer_height);
  }
  else
  {
    strcpy(layer_height_txt, " --- mm ");  // leading and trailing space char so the text is centered on both rows
  }
}

static void setLayerNumberTxt(char * layer_number_txt)
{
  uint16_t layerNumber = getPrintLayerNumber();
  uint16_t layerCount = getPrintLayerCount();

  if (layerNumber > 0)
  {
    if (layerCount > 0
        #ifndef TFT70_V3_0
          && layerCount < 1000  // there's no space to display layer number & count if the layer count is above 999
        #endif
       )
    {
      sprintf(layer_number_txt, " %u/%u ", layerNumber, layerCount);
    }
    else
    {
      sprintf(layer_number_txt, "%s%u%s", "  ", layerNumber, "  ");
    }
  }
  else
  {
    strcpy(layer_number_txt, "---");
  }
}

// initialize printing info before opening Printing menu
void initMenuPrinting(void)
{
  getPrintTitle(title, MAX_TITLE_LEN);  // get print title according to print originator (remote or local to TFT)
  clearInfoFile();                      // as last, clear and free memory for file list

  progDisplayType = infoSettings.prog_disp_type;
  layerDisplayType = infoSettings.layer_disp_type * 2;
  coordinateSetAxisActual(Z_AXIS, 0);
  coordinateSetAxisTarget(Z_AXIS, 0);
  setM73R_presence(false);
}

// start print originated or handled by remote host
// (e.g. print started from remote onboard media or hosted by remote host) and open Printing menu
void startRemotePrint(const char * filename)
{
  if (!printRemoteStart(filename))
    return;

  // NOTE: call just before opening Printing menu because initMenuPrinting function will
  //       call clearInfoFile function that will clear and free memory for file list
  initMenuPrinting();

  infoMenu.cur = 1;  // clear menu buffer when Printing menu is activated by remote
  REPLACE_MENU(menuPrinting);
}

// start print originated or handled by TFT
// (e.g. print started from TFT's GUI or hosted by TFT) and open Printing menu
void startPrint(void)
{
  bool printRestore = powerFailedGetRestore();  // temporary save print restore flag before it is cleared by printStart function

  if (!printStart())
  {
    // in case the calling function is menuPrintFromSource,
    // remove the filename from path to allow the files scanning from its folder avoiding a scanning error message
    exitFolder();
    return;
  }

  // if restoring a print after a power failure or printing from remote TFT media (with M23 - M24),
  // no filename is available in infoFile. Only infoFile.source and infoFile.path have been set
  //
  if (!printRestore && infoFile.fileCount == 0)  // if printing from remote TFT media
    infoMenu.cur = 0;                            // clear menu buffer

  // NOTE: call just before opening Printing menu because initMenuPrinting function will
  //       call clearInfoFile function that will clear and free memory for file list
  initMenuPrinting();

  OPEN_MENU(menuPrinting);
}

static inline void reDrawPrintingValue(uint8_t icon_pos, uint8_t draw_type)
{
  LIVE_INFO lvIcon;
  GUI_RECT const *curRect = &printinfo_val_rect[icon_pos];
  char tempstrTop[14];
  char tempstrBottom[14];

  lvIcon.enabled[2] = false;

  if (icon_pos == ICON_POS_BED && currentBCIndex != 0)  // Bed & Chamber
    lvIcon.iconIndex = printingIcon2nd[0];
  else if (icon_pos == ICON_POS_SPD && currentSpeedID != 0)  // Speed & Flow
    lvIcon.iconIndex = printingIcon2nd[1];
  else
    lvIcon.iconIndex = printingIcon[icon_pos];

  if (draw_type & LIVE_INFO_TOP_ROW)
  {
    lvIcon.enabled[0] = true;
    lvIcon.lines[0].h_align = LEFT;
    lvIcon.lines[0].v_align = TOP;
    lvIcon.lines[0].pos = (GUI_POINT){.x = PS_ICON_TITLE_X, .y = PS_ICON_TITLE_Y};
    lvIcon.lines[0].font = FONT_SIZE_NORMAL;
    lvIcon.lines[0].fn_color = infoSettings.font_color;
    lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
    lvIcon.lines[0].text = (uint8_t*)tempstrTop;

    switch (icon_pos)
    {
      case ICON_POS_EXT:
        lvIcon.lines[0].text = (uint8_t*)heatDisplayID[currentTool];
        break;

      case ICON_POS_BED:
        lvIcon.lines[0].text = (uint8_t*)heatDisplayID[BED + currentBCIndex];
        break;

      case ICON_POS_FAN:
        lvIcon.lines[0].text = (uint8_t*)fanID[currentFan];
        break;

      case ICON_POS_TIM:
        if ((getPrintRemainingTime() == 0) || (progDisplayType != ELAPSED_REMAINING))
          snprintf(tempstrTop, 9, "%d%%      ", getPrintProgress());
        else
          timeToString(tempstrTop, TIME_FORMAT_STR, getPrintTime());
        break;

      case ICON_POS_Z:
        if (layerDisplayType == SHOW_LAYER_BOTH)
          setLayerHeightText(tempstrTop);
        else if (layerDisplayType == CLEAN_LAYER_NUMBER || layerDisplayType == CLEAN_LAYER_BOTH)
          lvIcon.lines[0].text = (uint8_t*)("        ");
        else
          lvIcon.lines[0].text = (uint8_t*)LAYER_TITLE;
        break;

      case ICON_POS_SPD:
        lvIcon.lines[0].text = (uint8_t *)speedId[currentSpeedID];
        break;

      default:
        break;
    }
  }
  else
  {
    lvIcon.enabled[0] = false;
  }

  if (draw_type & LIVE_INFO_BOTTOM_ROW)
  {
    lvIcon.enabled[1] = true;
    lvIcon.lines[1].h_align = CENTER;
    lvIcon.lines[1].v_align = CENTER;
    lvIcon.lines[1].pos = (GUI_POINT) {.x = (curRect->x0 + curRect->x1) / 2, .y = (curRect->y0 + curRect->y1) / 2};
    lvIcon.lines[1].font = FONT_SIZE_NORMAL;
    lvIcon.lines[1].fn_color = infoSettings.font_color;
    lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
    lvIcon.lines[1].text = (uint8_t*)tempstrBottom;

    tempstrBottom[0] = 0;  // always initialize to empty string as default value

    switch (icon_pos)
    {
      case ICON_POS_EXT:
        sprintf(tempstrBottom, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
        break;

      case ICON_POS_BED:
        sprintf(tempstrBottom, "%3d/%-3d", heatGetCurrentTemp(BED + currentBCIndex), heatGetTargetTemp(BED + currentBCIndex));
        break;

      case ICON_POS_FAN:
        if (infoSettings.fan_percentage == 1)
          sprintf(tempstrBottom, "%3d%%", fanGetCurPercent(currentFan));  // 4 chars
        else
          sprintf(tempstrBottom, "%3d ", fanGetCurSpeed(currentFan));  // 4 chars
        break;

      case ICON_POS_TIM:
        if ((getPrintRemainingTime() == 0) || (progDisplayType == PERCENTAGE_ELAPSED))
          timeToString(tempstrBottom, TIME_FORMAT_STR, getPrintTime());
        else
          timeToString(tempstrBottom, TIME_FORMAT_STR, getPrintRemainingTime());
        break;

      case ICON_POS_Z:
        if (layerDisplayType == SHOW_LAYER_HEIGHT)  // layer height
          setLayerHeightText(tempstrBottom);
        else if (layerDisplayType == SHOW_LAYER_NUMBER || layerDisplayType == SHOW_LAYER_BOTH)  // layer number or height & number (both)
          setLayerNumberTxt(tempstrBottom);
        else
          lvIcon.lines[1].text=  (uint8_t*)("        ");
        break;

      case ICON_POS_SPD:
        sprintf(tempstrBottom, "%3d%%", speedGetCurPercent(currentSpeedID));
        break;

      default:
        break;
    }
  }
  else
  {
    lvIcon.enabled[1] = false;
  }

  RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
  showLiveInfo(icon_pos, &lvIcon, draw_type & LIVE_INFO_ICON);
}  // reDrawPrintingValue

static inline void toggleInfo(void)
{
  if (nextScreenUpdate(TOGGLE_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      reDrawPrintingValue(ICON_POS_EXT, LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
    }

    if (infoSettings.chamber_en == 1)
    {
      currentBCIndex = (currentBCIndex + 1) % 2;
      reDrawPrintingValue(ICON_POS_BED, LIVE_INFO_ICON | LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
    }
    else
    {
      reDrawPrintingValue(ICON_POS_BED, LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
    }

    if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    {
      do
      {
        currentFan = (currentFan + 1) % MAX_COOLING_FAN_COUNT;
      } while (!fanIsValid(currentFan));

      reDrawPrintingValue(ICON_POS_FAN, LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
    }

    currentSpeedID = (currentSpeedID + 1) % 2;
    reDrawPrintingValue(ICON_POS_SPD, LIVE_INFO_ICON | LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);

    speedQuery();

    if (infoFile.source >= FS_ONBOARD_MEDIA)
      coordinateQuery(MS_TO_SEC(TOGGLE_TIME));

    if (!hasFilamentData && isPrinting())
      updatePrintUsedFilament();
  }
}

static inline void reDrawProgressBar(uint8_t prevProgress, uint8_t nextProgress, uint16_t barColor, uint16_t sliceColor)
{
  uint16_t start = (PROGRESS_BAR_FULL_WIDTH * prevProgress) / 100;
  uint16_t end = (PROGRESS_BAR_FULL_WIDTH * nextProgress) / 100;

  GUI_FillRectColor(progressBar.x0 + start, progressBar.y0, progressBar.x0 + end, progressBar.y1, barColor);

  #ifdef MARKED_PROGRESS_BAR
    GUI_SetColor(sliceColor);

    start = prevProgress / 10 + 1;  // number of 10% markers + 1 (to skip redraw of 0% and already drawn marker)
    end = nextProgress / 10;        // number of 10% markers

    if (end == 10)  // avoid to draw the marker for 100% progress
      end--;

    for (int i = start; i <= end; i++)
    {
      GUI_VLine(progressBar.x0 + PROGRESS_BAR_SLICE_WIDTH * i - 1, progressBar.y0 + 1, progressBar.y1 - 1);
    }

    GUI_RestoreColorDefault();
  #endif
}

static inline void reDrawProgress(uint8_t prevProgress)
{
  reDrawProgressBar(prevProgress, getPrintProgress(), PB_FILL, PB_STRIPE_ELAPSED);

  if (progDisplayType != ELAPSED_REMAINING)
    reDrawPrintingValue(ICON_POS_TIM, LIVE_INFO_TOP_ROW);
}

static inline void drawLiveInfo(void)
{
  for (uint8_t i = 0; i < COUNT(printingIcon); i++)
  {
    reDrawPrintingValue(i, LIVE_INFO_ICON | LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
  }

  // progress
  GUI_SetColor(PB_BORDER);
  GUI_DrawRect(progressBar.x0 - 1, progressBar.y0 - 1, progressBar.x1 + 1, progressBar.y1 + 1);  // draw progress bar border
  reDrawProgressBar(0, 100, PB_BCKG, PB_STRIPE_REMAINING);  // draw progress bar
  reDrawProgress(0);  // draw progress
  GUI_RestoreColorDefault();
}

void drawPrintInfo(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  IMAGE_ReadDisplay(rect_of_keySS[KEY_INFOBOX].x0, rect_of_keySS[KEY_INFOBOX].y0, INFOBOX_ADDR);

  GUI_SetColor(INFOMSG_BG_COLOR);
  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + STATUS_MSG_ICON_XOFFSET, rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));
  GUI_DispStringInRectEOL(rect_of_keySS[KEY_INFOBOX].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                          rect_of_keySS[KEY_INFOBOX].x1 - STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[KEY_INFOBOX].y1 - STATUS_MSG_ICON_YOFFSET,
                          (uint8_t *)textSelect((getPrintAborted() == true) ? LABEL_PROCESS_ABORTED : LABEL_PRINT_FINISHED));

  GUI_SetColor(INFOMSG_FONT_COLOR);
  GUI_SetBkColor(INFOMSG_BG_COLOR);
  GUI_DispStringInPrect(&msgRect, LABEL_CLICK_FOR_MORE);
  GUI_RestoreColorDefault();
}

void stopConfirm(void)
{
  printAbort();
}

void printSummaryPopup(void)
{
  char showInfo[150];
  char tempstr[30];

  timeToString(showInfo, (char*)textSelect(LABEL_PRINT_TIME), infoPrintSummary.time);

  if (getPrintAborted() == true)
  {
    sprintf(tempstr, "\n\n%s", (char *)textSelect(LABEL_PROCESS_ABORTED));
    strcat(showInfo, tempstr);
  }
  else if (infoPrintSummary.length + infoPrintSummary.weight + infoPrintSummary.cost == 0)  // all  equals 0
  {
    strcat(showInfo, (char *)textSelect(LABEL_NO_FILAMENT_STATS));
  }
  else
  {
    if (infoPrintSummary.length > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_LENGTH), infoPrintSummary.length);
      strcat(showInfo, tempstr);
    }
    if (infoPrintSummary.weight > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_WEIGHT), infoPrintSummary.weight);
      strcat(showInfo, tempstr);
    }
    if (infoPrintSummary.cost > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_COST), infoPrintSummary.cost);
      strcat(showInfo, tempstr);
    }
  }
  popupReminder(DIALOG_TYPE_INFO, (uint8_t *)infoPrintSummary.name, (uint8_t *)showInfo);
}

void menuPrinting(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS printingItems = {
    // title
    LABEL_NULL,
    // icon                          label
    {
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_BABYSTEP},
      {ICON_MORE,                    LABEL_MORE},
      {ICON_STOP,                    LABEL_STOP},
    }
  };

  uint8_t nowFan[MAX_FAN_COUNT] = {0};
  uint8_t oldProgress = 0;
  uint16_t curspeed[2] = {0};
  uint32_t time = 0;
  HEATER nowHeat;
  float curLayerHeight = 0;
  float usedLayerHeight = 0;
  float prevLayerHeight = 0;
  uint16_t curLayerNumber = 0;
  uint16_t prevLayerNumber = 0;

  bool layerDrawEnabled = false;
  bool lastPause = isPaused();
  bool lastPrinting = isPrinting();

  memset(&nowHeat, 0, sizeof(HEATER));

  if (lastPrinting == true)
  {
    printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
    printingItems.items[KEY_ICON_5].icon = (infoFile.source < FS_ONBOARD_MEDIA && isPrintModelIcon()) ? ICON_PREVIEW : ICON_BABYSTEP;
  }
  else  // returned to this menu after print was done or aborted
  {
    printingItems.items[KEY_ICON_4] = itemIsPrinting[1];  // Main Screen
    printingItems.items[KEY_ICON_5] = itemIsPrinting[0];  // Background
    printingItems.items[KEY_ICON_6] = itemIsPrinting[0];  // Background
    printingItems.items[KEY_ICON_7] = itemIsPrinting[2];  // Back
    updatePrintProgress();
  }

  printingItems.title.address = title;

  menuDrawPage(&printingItems);
  drawLiveInfo();

  #ifndef PORTRAIT_MODE
    if (lastPrinting == false)
      drawPrintInfo();
  #endif

  while (MENU_IS(menuPrinting))
  {
    //Scroll_DispString(&titleScroll, LEFT);  // Scroll display file name will take too many CPU cycles

    // check nozzle temp change
    if (nowHeat.T[currentTool].current != heatGetCurrentTemp(currentTool) ||
        nowHeat.T[currentTool].target != heatGetTargetTemp(currentTool))
    {
      nowHeat.T[currentTool].current = heatGetCurrentTemp(currentTool);
      nowHeat.T[currentTool].target = heatGetTargetTemp(currentTool);
      reDrawPrintingValue(ICON_POS_EXT, LIVE_INFO_BOTTOM_ROW);
    }

    // check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      reDrawPrintingValue(ICON_POS_BED, LIVE_INFO_BOTTOM_ROW);
    }

    // check Fan speed change
    if (nowFan[currentFan] != fanGetCurSpeed(currentFan))
    {
      nowFan[currentFan] = fanGetCurSpeed(currentFan);
      reDrawPrintingValue(ICON_POS_FAN, LIVE_INFO_BOTTOM_ROW);
    }

    if (lastPrinting == true)
    {
      // check print time change
      if (time != getPrintTime())
      {
        time = getPrintTime();
        if (progDisplayType == ELAPSED_REMAINING)
          reDrawPrintingValue(ICON_POS_TIM, LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
        else
          reDrawPrintingValue(ICON_POS_TIM, LIVE_INFO_BOTTOM_ROW);
      }

      // check print progress percentage change
      updatePrintProgress();
      if (oldProgress < getPrintProgress())
      {
        reDrawProgress(oldProgress);
        oldProgress = getPrintProgress();
      }
    }

    // Z_AXIS coordinate
    if (layerDisplayType == SHOW_LAYER_BOTH || layerDisplayType == SHOW_LAYER_HEIGHT)
    {
      curLayerHeight = coordinateGetAxis(Z_AXIS);
      if (prevLayerHeight != curLayerHeight)
      {
        if (ABS(curLayerHeight - usedLayerHeight) >= LAYER_DELTA)
        {
          layerDrawEnabled = true;
        }
        if (layerDrawEnabled == true)
        {
          usedLayerHeight = curLayerHeight;
          reDrawPrintingValue(ICON_POS_Z, (layerDisplayType == SHOW_LAYER_BOTH) ? LIVE_INFO_TOP_ROW : LIVE_INFO_BOTTOM_ROW);
        }
        if (ABS(curLayerHeight - prevLayerHeight) < LAYER_DELTA)
        {
          layerDrawEnabled = false;
        }
        prevLayerHeight = curLayerHeight;
      }
    }

    if (layerDisplayType == SHOW_LAYER_BOTH || layerDisplayType == SHOW_LAYER_NUMBER)
    {
      curLayerNumber = getPrintLayerNumber();
      if (curLayerNumber != prevLayerNumber)
      {
        prevLayerNumber = curLayerNumber;
        reDrawPrintingValue(ICON_POS_Z, LIVE_INFO_BOTTOM_ROW);
      }
    }

    // check change in speed or flow
    if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
    {
      curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
      reDrawPrintingValue(ICON_POS_SPD, LIVE_INFO_BOTTOM_ROW);
    }

    // check if print is paused
    if (lastPause != isPaused())
    {
      lastPause = isPaused();
      printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
    }

    // check if print just started or just finished
    if (lastPrinting != isPrinting())
    {
      lastPrinting = isPrinting();

      #ifdef PORTRAIT_MODE
        if (lastPrinting == false)
          printSummaryPopup();
      #endif

      return;  // It will restart this interface if directly return this function without modify the value of infoMenu
    }

    toggleInfo();

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      case PS_KEY_0:
        heatSetCurrentIndex(-1);  // set last used hotend index
        OPEN_MENU(menuHeat);
        break;

      case PS_KEY_1:
        heatSetCurrentIndex(-2);  // set last used bed index
        OPEN_MENU(menuHeat);
        break;

      case PS_KEY_2:
        OPEN_MENU(menuFan);
        break;

      case PS_KEY_3:
        progDisplayType = (progDisplayType + 1) % 3;
        reDrawPrintingValue(ICON_POS_TIM, LIVE_INFO_TOP_ROW | LIVE_INFO_BOTTOM_ROW);
        break;

      case PS_KEY_4:
        layerDisplayType++;  // trigger cleaning previous values
        if (layerDisplayType != CLEAN_LAYER_HEIGHT)
        {
          reDrawPrintingValue(ICON_POS_Z, LIVE_INFO_TOP_ROW);
        }
        reDrawPrintingValue(ICON_POS_Z, LIVE_INFO_BOTTOM_ROW);

        layerDisplayType = (layerDisplayType + 1) % 6;  // iterate layerDisplayType
        if (layerDisplayType != SHOW_LAYER_NUMBER)  // upper row content changes
        {
          reDrawPrintingValue(ICON_POS_Z, LIVE_INFO_TOP_ROW);
        }
        reDrawPrintingValue(ICON_POS_Z, LIVE_INFO_BOTTOM_ROW);
        break;

      case PS_KEY_5:
        OPEN_MENU(menuSpeed);
        break;

      case PS_KEY_6:
        if (lastPrinting == true)  // if printing
        { // Pause button
          if (getHostDialog() || isRemoteHostPrinting())
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_BUSY));
          else if (getPrintRunout())
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_FILAMENT_RUNOUT));
          else
            printPause(!isPaused(), PAUSE_NORMAL);
        }
        else
        { // Main button
          clearInfoFile();
          infoMenu.cur = 0;
        }
        break;

      case PS_KEY_7:
        OPEN_MENU(menuBabystep);
        break;

      case PS_KEY_8:
        OPEN_MENU(menuMore);
        break;

      case PS_KEY_9:
        if (lastPrinting == true)  // if printing
        { // Stop button
          if (isRemoteHostPrinting())
          {
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_BUSY));
          }
          else
          {
            setDialogText(LABEL_WARNING, LABEL_STOP_PRINT, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, stopConfirm, NULL, NULL);
          }
        }
        else
        { // Back button
          CLOSE_MENU();
        }
        break;

      case PS_KEY_INFOBOX:
        printSummaryPopup();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
