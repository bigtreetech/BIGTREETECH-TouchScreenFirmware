#include "Printing.h"
#include "includes.h"

const GUI_RECT printinfo_val_rect[6] = {
  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_SM_EX, PICON_START_Y+PICON_HEIGHT*0+PICON_SPACE_Y*0+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*0+PICON_SPACE_X*0+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*1+PICON_SPACE_X*1+PICON_VAL_LG_EX, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y+BYTE_HEIGHT},

  {START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_X,     PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y,
   START_X+PICON_LG_WIDTH*2+PICON_SPACE_X*2+PICON_VAL_SM_EX, PICON_START_Y+PICON_HEIGHT*1+PICON_SPACE_Y*1+PICON_VAL_Y+BYTE_HEIGHT},
};

#define PROGRESS_BAR_RAW_X0 (START_X)                             // X0 aligned to first icon
#define PROGRESS_BAR_RAW_X1 (START_X + 4*ICON_WIDTH + 3*SPACE_X)  // X1 aligned to last icon

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

const GUI_RECT progressBar = {PROGRESS_BAR_X0, PICON_START_Y + PICON_HEIGHT * 2 + PICON_SPACE_Y * 2 + 1,
                              PROGRESS_BAR_X1, ICON_START_Y + ICON_HEIGHT + SPACE_Y - PICON_SPACE_Y - 1};

#ifdef UNIFORM_LIVE_TEXT_BG_COLOR
  uint16_t textBgColor;
#endif

enum
{
  PRINT_ICON = (1 << 0),
  PRINT_TOP_ROW = (1 << 1),
  PRINT_BOTTOM_ROW = (1 << 2),
};

const uint8_t printingIcon[] = {ICON_PRINTING_NOZZLE, ICON_PRINTING_BED,    ICON_PRINTING_FAN,
                                ICON_PRINTING_TIMER,  ICON_PRINTING_ZLAYER, ICON_PRINTING_SPEED};

const uint8_t printingIcon2nd[] = {ICON_PRINTING_CHAMBER, ICON_PRINTING_FLOW};

const char *const speedId[2] = {"Speed", "Flow "};
bool hasFilamentData;
PROGRESS_DISPLAY progDisplayType;
LAYER_TYPE layerDisplayType;

#define TOGGLE_TIME  2000  // 1 seconds is 1000
#define LAYER_DELTA  0.1   // minimal layer height change to update the layer display (avoid congestion in vase mode)

#define LAYER_TITLE  "Layer"

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
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
  {ICON_MAINMENU,                LABEL_MAIN_SCREEN},
  {ICON_BACK,                    LABEL_BACK},
};

static void setLayerHeightText(char * layer_height_txt)
{
  float layer_height;
  layer_height = (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS);
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
  if (getLayerNumber() > 0)
  {
    if (getLayerCount() > 0)
    {
      sprintf(layer_number_txt, "%u/%u", getLayerNumber(), getLayerCount());
    }
    else
    {
      sprintf(layer_number_txt, "%u", getLayerNumber());
    }
  }
  else
  {
    strcpy(layer_number_txt, "---");
  }
}

void menuBeforePrinting(void)
{
  // load stat/end/cancel gcodes from spi flash

  switch (infoFile.source)
  {
    case BOARD_SD:  // GCode from file on ONBOARD SD
      {
        uint32_t size;

        size = request_M23_M36(infoFile.title + (infoMachineSettings.firmwareType == FW_REPRAPFW ? 0 : 5));
        //if (powerFailedCreate(infoFile.title) == false)
        //{
        //
        //}  // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

        if (size == 0)
        {
          ExitDir();
          infoMenu.cur--;
          return;
        }

        //if (powerFailedExist())
        //{
        request_M24(0);
        //}
        //else
        //{
        //request_M24(infoBreakPoint.offset);
        //}

        printStart(NULL, size);
        break;
      }

    case TFT_UDISK:
    case TFT_SD:  // GCode from file on TFT SD
      {
        FIL file;

        if (f_open(&file, infoFile.title, FA_OPEN_EXISTING | FA_READ) != FR_OK)
        {
          ExitDir();
          infoMenu.cur--;
          return;
        }

        if (powerFailedCreate(infoFile.title) == false)
        {}
        powerFailedlSeek(&file);

        printStart(&file, f_size(&file));
        break;
      }

    default:
      ExitDir();
      infoMenu.cur--;
      return;
  }

  // initialize things before the print starts
  progDisplayType = infoSettings.prog_disp_type;
  layerDisplayType = infoSettings.layer_disp_type * 2;
  setLayerNumber(0);
  coordinateSetAxisActual(Z_AXIS, 0);
  coordinateSetAxisTarget(Z_AXIS, 0);
  setM73_presence(false);
  setTotalTime(0);
  
  infoMenu.menu[infoMenu.cur] = menuPrinting;
}

static inline void reDrawPrintingValue(uint8_t icon_pos, uint8_t draw_type)
{
  uint8_t icon = printingIcon[icon_pos];
  char tempstr[14];

  switch (icon_pos)
  {
    case ICON_POS_BED:
      if (currentBCIndex != 0)
        icon = printingIcon2nd[0];
      break;

    case ICON_POS_SPD:
      if (currentSpeedID != 0)
        icon = printingIcon2nd[1];
      break;

    default:
      break;
  }

  if (draw_type & PRINT_ICON)
  {
    ICON_ReadDisplay(rect_of_keyPS[icon_pos].x0, rect_of_keyPS[icon_pos].y0, icon);
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  }
  else
  {
    ICON_PrepareRead(rect_of_keyPS[icon_pos].x0, rect_of_keyPS[icon_pos].y0, icon);
  }

  #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it set the sampled background color as the background color to be used
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetBkColor(textBgColor);
  #endif

  if (draw_type & PRINT_TOP_ROW)
  {
    uint8_t* textString = NULL;  // always initialize to NULL pointer as default value

    switch (icon_pos)
    {
      case ICON_POS_EXT:
        textString = (uint8_t *)heatDisplayID[currentTool];
        break;

      case ICON_POS_BED:
        textString = (uint8_t *)heatDisplayID[BED + currentBCIndex];
        break;

      case ICON_POS_FAN:
        textString = (uint8_t *)fanID[currentFan];
        break;

      case ICON_POS_TIM:
      {
        if ((getPrintRemainingTime() == 0) || (progDisplayType != ELAPSED_REMAINING))
        {
          uint8_t printProgress = getPrintProgress();
          snprintf(tempstr, 9, "%d%%      ", printProgress);
        }
        else
        {
          uint8_t hour, min, sec;

          getPrintTimeDetail(&hour, &min, &sec);
          sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
        }
        textString = (uint8_t *)tempstr;
        break;
      }

      case ICON_POS_Z:
        if (layerDisplayType == SHOW_LAYER_BOTH)
        {
          setLayerHeightText(tempstr);
        }
        else if (layerDisplayType == CLEAN_LAYER_NUMBER || layerDisplayType == CLEAN_LAYER_BOTH)
        {
          strcpy(tempstr, "        ");
        }
        else
        {
          strcpy(tempstr, LAYER_TITLE);
        }
        textString = (uint8_t *)tempstr;
        break;

      case ICON_POS_SPD:
        textString = (uint8_t *)speedId[currentSpeedID];
        break;

      default:
        break;
    }

    GUI_DispString(rect_of_keyPS[icon_pos].x0 + PICON_TITLE_X, rect_of_keyPS[icon_pos].y0 + PICON_TITLE_Y, textString);
  }

  if (draw_type & PRINT_BOTTOM_ROW)
  {
    tempstr[0] = 0;  // always initialize to empty string as default value

    switch (icon_pos)
    {
      case ICON_POS_EXT:
        sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
        break;

      case ICON_POS_BED:
        sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(BED + currentBCIndex), heatGetTargetTemp(BED + currentBCIndex));
        break;

      case ICON_POS_FAN:
        if (infoSettings.fan_percentage == 1)
          sprintf(tempstr, "%3d%%", fanGetCurPercent(currentFan));  // 4 chars
        else
          sprintf(tempstr, "%3d ", fanGetCurSpeed(currentFan));  // 4 chars
        break;

      case ICON_POS_TIM:
        {
          uint8_t hour, min, sec;

          if ((getPrintRemainingTime() == 0) || (progDisplayType == PERCENTAGE_ELAPSED))
            getPrintTimeDetail(&hour, &min, &sec);
          else
            getPrintRemainingTimeDetail(&hour, &min, &sec);

          sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
        }
        break;

      case ICON_POS_Z:
        if (layerDisplayType == SHOW_LAYER_HEIGHT)  // layer height
        {
          setLayerHeightText(tempstr);
        }
        else if (layerDisplayType == SHOW_LAYER_NUMBER || layerDisplayType == SHOW_LAYER_BOTH)  // layer number or height & number (both)
        {
          setLayerNumberTxt(tempstr);
        }
        else
        {
          strcpy(tempstr, "        ");
        }
        break;

      case ICON_POS_SPD:
        sprintf(tempstr, "%3d%%", speedGetCurPercent(currentSpeedID));
        break;

      default:
        break;
    }

    GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  }

  if (!(draw_type & PRINT_ICON))
    ICON_PrepareReadEnd();
}  // reDrawPrintingValue

static inline void toggleInfo(void)
{
  if (nextScreenUpdate(TOGGLE_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_EXT, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    }

    if (infoSettings.chamber_en == 1)
    {
      currentBCIndex = (currentBCIndex + 1) % 2;
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling

      #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it allows to eliminate flickering on alternating icons
        ICON_PartialReadDisplay(rect_of_keyPS[ICON_POS_BED].x0, rect_of_keyPS[ICON_POS_BED].y0, PICON_TITLE_X, -1,
                                (currentBCIndex == 0) ? ICON_PRINTING_BED : ICON_PRINTING_CHAMBER, 0, 0);
        reDrawPrintingValue(ICON_POS_BED, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
      #else
        reDrawPrintingValue(ICON_POS_BED, PRINT_ICON | PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
      #endif
    }
    else
    {
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_BED, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    }

    if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    {
      do
      {
        currentFan = (currentFan + 1) % MAX_FAN_COUNT;
      } while (!fanIsValid(currentFan));

      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_FAN, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    }

    currentSpeedID = (currentSpeedID + 1) % 2;
    RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling

    #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it allows to eliminate flickering on alternating icons
      ICON_PartialReadDisplay(rect_of_keyPS[ICON_POS_SPD].x0, rect_of_keyPS[ICON_POS_SPD].y0, PICON_TITLE_X, -1,
                              (currentSpeedID == 0) ? ICON_PRINTING_SPEED : ICON_PRINTING_FLOW, 0, 0);
      reDrawPrintingValue(ICON_POS_SPD, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    #else
      reDrawPrintingValue(ICON_POS_SPD, PRINT_ICON | PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    #endif

    speedQuery();

    if (infoFile.source >= BOARD_SD)
      coordinateQuery(TOGGLE_TIME / 1000);

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
      GUI_VLine(progressBar.x0 + PROGRESS_BAR_SLICE_WIDTH * i - 1, progressBar.y0 + 1, progressBar.y1 - 1);

    GUI_RestoreColorDefault();
  #endif
}

static inline void reDrawProgress(uint8_t prevProgress)
{
  uint8_t nextProgress = getPrintProgress();

  if (nextProgress != prevProgress)
  { // we need speed, do not draw anything if progress isn't changed
    if (nextProgress > prevProgress)
      reDrawProgressBar(prevProgress, nextProgress, MAT_ORANGE, BLACK);
    else  // if regress, swap indexes and colors
      reDrawProgressBar(nextProgress, prevProgress, DARKGRAY, MAT_ORANGE);
    if (progDisplayType != ELAPSED_REMAINING)
    {
      reDrawPrintingValue(ICON_POS_TIM, PRINT_TOP_ROW);
    }
  }
}

static inline void printingDrawPage(void)
{
  #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it samples the background color from an icon
    ICON_PrepareRead(rect_of_keyPS[ICON_POS_EXT].x0, rect_of_keyPS[ICON_POS_EXT].y0, ICON_PRINTING_NOZZLE);
    textBgColor = ICON_ReadPixel(rect_of_keyPS[ICON_POS_EXT].x0 + PICON_TITLE_X, rect_of_keyPS[ICON_POS_EXT].y0 + PICON_TITLE_Y);
    ICON_PrepareReadEnd();
  #endif

  for (uint8_t i = 0; i < 6; i++)
    reDrawPrintingValue(i, PRINT_ICON | PRINT_TOP_ROW | PRINT_BOTTOM_ROW);

  // progress
  GUI_SetColor(ORANGE);
  GUI_DrawRect(progressBar.x0 - 1, progressBar.y0 - 1, progressBar.x1 + 1, progressBar.y1 + 1);  // draw progress bar border
  GUI_RestoreColorDefault();
  reDrawProgressBar(0, 100, DARKGRAY, MAT_ORANGE);  // draw progress bar
  reDrawProgress(0);  // draw progress
}

void drawPrintInfo(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  IMAGE_ReadDisplay(rect_of_keySS[KEY_INFOBOX].x0, rect_of_keySS[KEY_INFOBOX].y0, INFOBOX_ADDR);

  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + STATUS_MSG_ICON_XOFFSET, rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));
  GUI_DispStringInRectEOL(rect_of_keySS[KEY_INFOBOX].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                          rect_of_keySS[KEY_INFOBOX].x1 - STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[KEY_INFOBOX].y1 - STATUS_MSG_ICON_YOFFSET,
                          (uint8_t *)textSelect(LABEL_PRINT_FINISHED));

  GUI_SetColor(INFOMSG_COLOR);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_DispStringInPrect(&msgRect, LABEL_CLICK_FOR_MORE);
  GUI_RestoreColorDefault();
}

void stopConfirm(void)
{
  printAbort();
  infoMenu.cur--;
}

void printInfoPopup(void)
{
  uint8_t hour = infoPrintSummary.time / 3600;
  uint8_t min = infoPrintSummary.time % 3600 / 60;
  uint8_t sec = infoPrintSummary.time % 60;
  char showInfo[150];
  char tempstr[30];

  sprintf(showInfo, (char*)textSelect(LABEL_PRINT_TIME), hour, min, sec);

  if (infoPrintSummary.length + infoPrintSummary.weight + infoPrintSummary.cost == 0)  // all  equals 0
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
    LABEL_BACKGROUND,
    // icon                          label
    {
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BABYSTEP},
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
    if (infoMachineSettings.longFilename == ENABLED && infoFile.source == BOARD_SD)
      printingItems.title.address = (uint8_t *) infoFile.Longfile[infoFile.fileIndex];
    else
      printingItems.title.address = getPrintName(infoFile.title);

    printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
    printingItems.items[KEY_ICON_5].icon = (infoFile.source < BOARD_SD && isPrintModelIcon()) ? ICON_PREVIEW : ICON_BABYSTEP;
  }
  else  // returned to this menu after a print was done (ex: after a popup)
  {
    printingItems.title.address = (uint8_t *)infoPrintSummary.name;

    printingItems.items[KEY_ICON_4] = itemIsPrinting[1];  // MainScreen
    printingItems.items[KEY_ICON_5] = itemIsPrinting[0];  // BackGround
    printingItems.items[KEY_ICON_6] = itemIsPrinting[0];  // BackGround
    printingItems.items[KEY_ICON_7] = itemIsPrinting[2];  // Back
  }

  menuDrawPage(&printingItems);
  printingDrawPage();

  if (lastPrinting == false)
    drawPrintInfo();

  while (infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
    //Scroll_DispString(&titleScroll, LEFT);  // Scroll display file name will take too many CPU cycles

    // check nozzle temp change
    if (nowHeat.T[currentTool].current != heatGetCurrentTemp(currentTool) ||
        nowHeat.T[currentTool].target != heatGetTargetTemp(currentTool))
    {
      nowHeat.T[currentTool].current = heatGetCurrentTemp(currentTool);
      nowHeat.T[currentTool].target = heatGetTargetTemp(currentTool);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_EXT, PRINT_BOTTOM_ROW);
    }

    // check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_BED, PRINT_BOTTOM_ROW);
    }

    // check Fan speed change
    if (nowFan[currentFan] != fanGetCurSpeed(currentFan))
    {
      nowFan[currentFan] = fanGetCurSpeed(currentFan);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_FAN, PRINT_BOTTOM_ROW);
    }

    // check printing progress
    if (getPrintSize() != 0)
    {
      // check print time change
      if (time != getPrintTime())
      {
        time = getPrintTime();
        RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
        if (progDisplayType == ELAPSED_REMAINING)
        {
          reDrawPrintingValue(ICON_POS_TIM, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
        }
        else
        {
          reDrawPrintingValue(ICON_POS_TIM, PRINT_BOTTOM_ROW);
        }
      }

      // check print progress percentage change
      if (updatePrintProgress())
      {
        RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
        reDrawProgress(oldProgress);
        oldProgress = getPrintProgress();
      }
    }
    else
    {
      if (getPrintProgress() != 100)
      {
        reDrawPrintingValue(ICON_POS_TIM, PRINT_BOTTOM_ROW);
        updatePrintProgress();
        reDrawProgress(oldProgress);
        oldProgress = getPrintProgress();
      }
    }

    // Z_AXIS coordinate
    if (layerDisplayType == SHOW_LAYER_BOTH || layerDisplayType == SHOW_LAYER_HEIGHT)
    {
      curLayerHeight = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));
      if (prevLayerHeight != curLayerHeight)
      {
        if (ABS(curLayerHeight - usedLayerHeight) >= LAYER_DELTA)
        {
          layerDrawEnabled = true;
        }
        if (layerDrawEnabled == true)
        {
          usedLayerHeight = curLayerHeight;
          RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
          reDrawPrintingValue(ICON_POS_Z, (layerDisplayType == SHOW_LAYER_BOTH) ? PRINT_TOP_ROW : PRINT_BOTTOM_ROW);
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
      curLayerNumber = getLayerNumber();
      if (curLayerNumber != prevLayerNumber)
      {
        prevLayerNumber = curLayerNumber;
        RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
        reDrawPrintingValue(ICON_POS_Z, PRINT_BOTTOM_ROW);
      }
    }

    // check change in speed or flow
    if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
    {
      curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(ICON_POS_SPD, PRINT_BOTTOM_ROW);
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
      return;  // It will restart this interface if directly return this function without modify the value of infoMenu
    }

    toggleInfo();

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      case PS_KEY_0:
        heatSetCurrentIndex(currentTool);
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case PS_KEY_1:
        heatSetCurrentIndex(BED + currentBCIndex);
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case PS_KEY_2:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;

      case PS_KEY_3:
        progDisplayType = (progDisplayType + 1) % 3;
        reDrawPrintingValue(ICON_POS_TIM, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
        break;

      case PS_KEY_4:
        layerDisplayType ++;  // trigger cleaning previous values
        if (layerDisplayType != CLEAN_LAYER_HEIGHT)
        {
          reDrawPrintingValue(ICON_POS_Z, PRINT_TOP_ROW);
        }
        reDrawPrintingValue(ICON_POS_Z, PRINT_BOTTOM_ROW);

        layerDisplayType = (layerDisplayType + 1) % 6;  // iterate layerDisplayType
        if (layerDisplayType != SHOW_LAYER_NUMBER)  // upper row content changes
        {
          reDrawPrintingValue(ICON_POS_Z, PRINT_TOP_ROW);
        }
        reDrawPrintingValue(ICON_POS_Z, PRINT_BOTTOM_ROW);
        break;

      case PS_KEY_5:
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      case PS_KEY_6:
        if (isPrinting())
        {
          if (getHostDialog())
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_BUSY));
          else if (getPrintRunout())
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_FILAMENT_RUNOUT));
          else
            printPause(!isPaused(), PAUSE_NORMAL);
        }
        else
        {
          clearInfoPrint();
          clearInfoFile();
          infoMenu.cur = 0;
        }
        break;

      case PS_KEY_7:
        infoMenu.menu[++infoMenu.cur] = menuBabystep;
        break;

      case PS_KEY_8:
        infoMenu.menu[++infoMenu.cur] = menuMore;
        break;

      case PS_KEY_9:
        if (isPrinting())
        {
          setDialogText(LABEL_WARNING, LABEL_STOP_PRINT, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, stopConfirm, NULL, NULL);
        }
        else
        {
          clearInfoPrint();
          infoMenu.cur--;
        }
        break;

      case PS_KEY_INFOBOX:
        printInfoPopup();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
