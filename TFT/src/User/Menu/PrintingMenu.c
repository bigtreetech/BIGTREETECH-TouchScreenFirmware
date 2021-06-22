#include "Printing.h"
#include "includes.h"

const GUI_POINT printinfo_points[6] = {
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
};

const GUI_RECT printinfo_val_rect[6] = {
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, PICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, PICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},
};

#define PROGRESS_BAR_RAW_X0    (START_X)                                             // X0 aligned to first icon
#define PROGRESS_BAR_RAW_X1    (START_X + 4 * ICON_WIDTH + 3 * SPACE_X)              // X1 aligned to last icon

#ifdef MARKED_PROGRESS_BAR
  #define PROGRESS_BAR_DELTA_X   ((PROGRESS_BAR_RAW_X1 - PROGRESS_BAR_RAW_X0) % 10)  // use marked progress bar. Width rounding factor multiple of 10 slices
#else
  #define PROGRESS_BAR_DELTA_X   2                                                   // use standard progress bar. Reserve a 2 pixels width for vertical borders
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

const uint8_t printingIcon[] = {ICON_PRINTING_NOZZLE, ICON_PRINTING_BED,   ICON_PRINTING_FAN,
                                ICON_PRINTING_TIMER, ICON_PRINTING_ZLAYER, ICON_PRINTING_SPEED, ICON_PRINTING_FLOW};
const char *const speedId[2] = {"Speed", "Flow "};
bool hasFilamentData;
PROGRESS_DISPLAY progDisplayType;

#define TOGGLE_TIME  2000  // 1 seconds is 1000
#define LAYER_DELTA  0.1   // minimal layer height change to update the layer display (avoid congestion in vase mode)

#define LAYER_TITLE  "Layer"
#define EXT_ICON_POS 0
#define BED_ICON_POS 1
#define FAN_ICON_POS 2
#define TIM_ICON_POS 3
#define Z_ICON_POS   4
#define SPD_ICON_POS 5

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

void menuBeforePrinting(void)
{
  // load stat/end/cancel gcodes from spi flash

  switch (infoFile.source)
  {
    case BOARD_SD:  // GCode from file on ONBOARD SD
      {
        uint32_t size;

        size = request_M23_M36(infoFile.title + 5);
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
  progDisplayType = infoSettings.prog_disp_type;
  infoMenu.menu[infoMenu.cur] = menuPrinting;
}

static inline void reDrawPrintingValue(uint8_t icon_pos, uint8_t draw_type)
{
  uint8_t icon = (icon_pos == SPD_ICON_POS) ? printingIcon[icon_pos + currentSpeedID] : printingIcon[icon_pos];
  char tempstr[10];

  if (draw_type & PRINT_ICON)
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, icon);

  ICON_PrepareRead(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, icon);

  #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it set the sampled background color as the background color to be used
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetBkColor(textBgColor);
  #endif

  if (draw_type & PRINT_TOP_ROW)
  {
    uint8_t* textString = NULL;  // always initialize to NULL pointer as default value

    switch (icon_pos)
    {
      case EXT_ICON_POS:
        textString = (uint8_t *)heatDisplayID[currentTool];
        break;

      case BED_ICON_POS:
        textString = (uint8_t *)heatDisplayID[BED];
        break;

      case FAN_ICON_POS:
        textString = (uint8_t *)fanID[currentFan];
        break;

      case TIM_ICON_POS:
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

      case Z_ICON_POS:
        textString = (uint8_t *)LAYER_TITLE;
        break;

      case SPD_ICON_POS:
        textString = (uint8_t *)speedId[currentSpeedID];
        break;

      default:
        break;
    }

    GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, textString);
  }

  if (draw_type & PRINT_BOTTOM_ROW)
  {
    tempstr[0] = 0;  // always initialize to empty string as default value

    switch (icon_pos)
    {
      case EXT_ICON_POS:
        sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
        break;

      case BED_ICON_POS:
        sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
        break;

      case FAN_ICON_POS:
        if (infoSettings.fan_percentage == 1)
          sprintf(tempstr, "%3d%%", fanGetCurPercent(currentFan));
        else
          sprintf(tempstr, "%3d", fanGetCurSpeed(currentFan));
        break;

      case TIM_ICON_POS:
        {
          uint8_t hour, min, sec;

          if ((getPrintRemainingTime() == 0) || (progDisplayType == PERCENTAGE_ELAPSED))
          {
            getPrintTimeDetail(&hour, &min, &sec);
          }
          else
          {
            getPrintRemainingTimeDetail(&hour, &min, &sec);
          }
          sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
        }
        break;

      case Z_ICON_POS:
        sprintf(tempstr, "%6.2fmm", (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));
        break;

      case SPD_ICON_POS:
        sprintf(tempstr, "%3d%%", speedGetCurPercent(currentSpeedID));
        break;

      default:
        break;
    }

    GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  }

  ICON_PrepareReadEnd();
}

static inline void toggleInfo(void)
{
  if (nextScreenUpdate(TOGGLE_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(EXT_ICON_POS, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    }

    if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    {
      do
      {
        currentFan = (currentFan + 1) % MAX_FAN_COUNT;
      } while (!fanIsValid(currentFan));

      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(FAN_ICON_POS, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    }

    currentSpeedID = (currentSpeedID + 1) % 2;
    RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling

    #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it allows to eliminate flickering on alternating icons
      ICON_PartialReadDisplay(printinfo_points[SPD_ICON_POS].x, printinfo_points[SPD_ICON_POS].y, PICON_TITLE_X, -1,
                              (currentSpeedID == 0) ? ICON_PRINTING_SPEED : ICON_PRINTING_FLOW, 0, 0);
      reDrawPrintingValue(SPD_ICON_POS, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
    #else
      reDrawPrintingValue(SPD_ICON_POS, PRINT_ICON | PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
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
      reDrawPrintingValue(TIM_ICON_POS, PRINT_TOP_ROW);
    }
  }
}

static inline void printingDrawPage(void)
{
  #ifdef UNIFORM_LIVE_TEXT_BG_COLOR  // it samples the background color from an icon
    ICON_PrepareRead(printinfo_points[EXT_ICON_POS].x, printinfo_points[EXT_ICON_POS].y, ICON_PRINTING_NOZZLE);
    textBgColor = ICON_ReadPixel(printinfo_points[EXT_ICON_POS].x + PICON_TITLE_X, printinfo_points[EXT_ICON_POS].y + PICON_TITLE_Y);
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

  IMAGE_ReadDisplay(rect_of_keySS[17].x0, rect_of_keySS[17].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[17].x0 + STATUS_MSG_ICON_XOFFSET, rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));
  GUI_DispStringInRectEOL(rect_of_keySS[17].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                          rect_of_keySS[17].x1 - STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[17].y1 - STATUS_MSG_ICON_YOFFSET,
                          (uint8_t *)textSelect(LABEL_PRINT_FINISHED));
  GUI_SetColor(INFOMSG_COLOR);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_DispStringInPrect(&msgRect,LABEL_CLICK_FOR_MORE);
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

  if (infoPrintSummary.length == 0 && infoPrintSummary.weight == 0 && infoPrintSummary.cost == 0)
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
  float curLayer = 0;
  float usedLayer = 0;
  float prevLayer = 0;
  bool layerDrawEnabled = false;
  bool lastPause = isPaused();
  bool lastPrinting = isPrinting();

  memset(&nowHeat, 0, sizeof(HEATER));

  if (lastPrinting == true)
  {
    if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
      printingItems.title.address = (uint8_t *) infoFile.Longfile[infoFile.fileIndex];
    else
      printingItems.title.address = getPrintName(infoFile.title);
    printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
    printingItems.items[KEY_ICON_5].icon = (infoFile.source < BOARD_SD && isPrintModelIcon()) ? ICON_PREVIEW : ICON_BABYSTEP;
  }
  else // returned to this menu after a print was done (ex: after a popup)
  {
    printingItems.title.address = (uint8_t *)infoPrintSummary.name;

    #ifdef TFT70_V3_0
      printingItems.items[KEY_ICON_5] = itemIsPrinting[1];  // MainScreen
    #else
      printingItems.items[KEY_ICON_4] = itemIsPrinting[1];  // MainScreen
      printingItems.items[KEY_ICON_5] = itemIsPrinting[0];  // BackGround
    #endif
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
      reDrawPrintingValue(EXT_ICON_POS, PRINT_BOTTOM_ROW);
    }

    // check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(BED_ICON_POS, PRINT_BOTTOM_ROW);
    }

    // check Fan speed change
    if (nowFan[currentFan] != fanGetCurSpeed(currentFan))
    {
      nowFan[currentFan] = fanGetCurSpeed(currentFan);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(FAN_ICON_POS, PRINT_BOTTOM_ROW);
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
          reDrawPrintingValue(TIM_ICON_POS, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
        }
        else
        {
          reDrawPrintingValue(TIM_ICON_POS, PRINT_BOTTOM_ROW);
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
        reDrawPrintingValue(TIM_ICON_POS, PRINT_BOTTOM_ROW);
        updatePrintProgress();
        reDrawProgress(oldProgress);
        oldProgress = getPrintProgress();
      }
    }

    // Z_AXIS coordinate
    curLayer = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));
    if (prevLayer != curLayer)
    {
      if (ABS(curLayer - usedLayer) >= LAYER_DELTA)
      {
        layerDrawEnabled = true;
      }
      if (layerDrawEnabled == true)
      {
        usedLayer = curLayer;
        RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
        reDrawPrintingValue(Z_ICON_POS, PRINT_BOTTOM_ROW);
      }
      if (ABS(curLayer - prevLayer) < LAYER_DELTA)
      {
        layerDrawEnabled = false;
      }
      prevLayer = curLayer;
    }

    // check change in speed or flow
    if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
    {
      curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
      RAPID_SERIAL_LOOP();  // perform backend printing loop before drawing to avoid printer idling
      reDrawPrintingValue(SPD_ICON_POS, PRINT_BOTTOM_ROW);
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
      case KEY_ICON_0:
        if (isPrinting())
        {
          infoMenu.menu[++infoMenu.cur] = menuHeat;
        }
        break;

      case KEY_ICON_1:
        if (isPrinting())
        {
          infoMenu.menu[++infoMenu.cur] = menuFan;
        }
        break;

      case KEY_ICON_2:
        if (isPrinting())
        {
          progDisplayType = (progDisplayType + 1) % 3;
          reDrawPrintingValue(TIM_ICON_POS, PRINT_TOP_ROW | PRINT_BOTTOM_ROW);
        }
        break;

      case KEY_ICON_3:
        if (isPrinting())
        {
          infoMenu.menu[++infoMenu.cur] = menuSpeed;
        }
        break;

      case KEY_ICON_4:
        if (isPrinting())
        {
          if (!isHostDialog())
            printPause(!isPaused(), PAUSE_NORMAL);
          else
            addToast(DIALOG_TYPE_ERROR, (char *)textSelect(LABEL_BUSY));
        }
        #ifndef TFT70_V3_0
          else
          {
            clearInfoPrint();
            clearInfoFile();
            infoMenu.cur = 0;
          }
        #endif
        break;

      case KEY_ICON_5:
        #ifdef TFT70_V3_0
          if (isPrinting())
            infoMenu.menu[++infoMenu.cur] = menuBabystep;
          else
          {
            clearInfoPrint();
            clearInfoFile();
            infoMenu.cur = 0;
          }
        #else
          infoMenu.menu[++infoMenu.cur] = menuBabystep;
        #endif
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuMore;
        break;

      case KEY_ICON_7:
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

      case KEY_INFOBOX:
        printInfoPopup();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
