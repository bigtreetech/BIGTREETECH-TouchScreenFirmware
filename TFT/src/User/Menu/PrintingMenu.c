#include "Printing.h"
#include "includes.h"

const GUI_POINT printinfo_points[6] = {
    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
};

const GUI_RECT printinfo_val_rect[6] = {
    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},
};

static uint32_t nextInfoTime = 0;
static uint32_t nextDrawTime = 0;
const char *const Speed_ID[2] = {"Speed", "Flow"};
static char filamentInfo[150];
bool filDataSeen;
SCROLL infoScroll;
FILAMENTDATA filData = {"", 0, 0, 0, 0};

#define TOGGLE_TIME 2000 // 1 seconds is 1000
#define DRAW_TIME 500    // 1 seconds is 1000

#define MAX_FILE_CHAR 25 // max character length to store

#define LAYER_TITLE "Layer"
#define EXT_ICON_POS 0
#define BED_ICON_POS 1
#define FAN_ICON_POS 2
#define TIM_ICON_POS 3
#define Z_ICON_POS   4
#define SPD_ICON_POS 5

const ITEM itemIsPause[2] = {
  // icon       label
  {ICON_PAUSE,  LABEL_PAUSE},
  {ICON_RESUME, LABEL_RESUME},
};

const ITEM itemIsPrinting[6] = {
  // icon           label
  {ICON_BACKGROUND, LABEL_BACKGROUND},
  {ICON_MAINMENU,   LABEL_MAIN_SCREEN},
  {ICON_BABYSTEP,   LABEL_BABYSTEP},
  {ICON_MORE,       LABEL_MORE},
  {ICON_BACK,       LABEL_BACK},
  {ICON_STOP,       LABEL_STOP},
};

void menuBeforePrinting(void)
{
  //load stat/end/cancel gcodes from spi flash
  long size = 0;
  switch (infoFile.source)
  {
    case BOARD_SD: // GCode from file on ONBOARD SD
  #ifdef RepRapFirmware
      /*in RepRapFirmware, M23 not return the size of the file. So we send M36, to get file information*/
      size = request_M36(infoFile.title + 5);
      request_M23(infoFile.title + 5);
  #else
      size = request_M23(infoFile.title + 5);
  #endif

      //  if( powerFailedCreate(infoFile.title)==false)
      //  {
      //
      //  }    // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

      if (size == 0)
      {
        ExitDir();
        infoMenu.cur--;
        return;
      }

      infoPrinting.size = size;

      //    if(powerFailedExist())
      //    {
      request_M24(0);
      //    }
      //    else
      //    {
      //      request_M24(infoBreakPoint.offset);
      //    }

      if (infoMachineSettings.autoReportSDStatus == 1)
        request_M27(infoSettings.m27_refresh_time); //Check if there is a SD or USB print running.
      else
        request_M27(0);

      infoHost.printing = true; // Global lock info on printer is busy in printing.
      break;

    case TFT_UDISK:
    case TFT_SD: // GCode from file on TFT SD
      if (f_open(&infoPrinting.file, infoFile.title, FA_OPEN_EXISTING | FA_READ) != FR_OK)
      {
        ExitDir();
        infoMenu.cur--;
        return;
      }
      if (powerFailedCreate(infoFile.title) == false)
      {
      }
      powerFailedlSeek(&infoPrinting.file);

      infoPrinting.size = f_size(&infoPrinting.file);
      infoPrinting.cur = infoPrinting.file.fptr;
      if (infoSettings.send_start_gcode == 1 && infoPrinting.cur == 0) // PLR continue printing, CAN NOT use start gcode
      {
        sendPrintCodes(0);
      }
      break;
  }
  infoPrinting.printing = true;
  infoPrinting.time = 0;
  infoMenu.menu[infoMenu.cur] = menuPrinting;
  infoPrinting.time = 0;
  filData = (FILAMENTDATA){"", 0, 0, 0, 0};
  filDataSeen = false;
  initEpos();
}

static inline void reValueNozzle(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_NOZZLE);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (u8 *)heatDisplayID[currentTool]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reValueBed(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_BED);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (u8 *)heatDisplayID[BED]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawFan(int icon_pos)
{
  char tempstr[10];
  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%d%%", fanGetCurPercent(currentFan));
  else
    sprintf(tempstr, "%d", fanGetCurSpeed(currentFan));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_FAN);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (u8 *)fanID[currentFan]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawSpeed(int icon_pos)
{
  char tempstr[10];

  if (currentSpeedID == 0)
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_SPEED);
  else
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_FLOW);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedID));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (u8 *)Speed_ID[currentSpeedID]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawTime(int icon_pos)
{
  u8 hour = infoPrinting.time / 3600,
     min = infoPrinting.time % 3600 / 60,
     sec = infoPrinting.time % 60;

  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  char tempstr[10];
  sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_TIMER);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawProgress(int icon_pos)
{
  char buf[6];
  sprintf(buf, "%d%%", infoPrinting.progress);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispString(printinfo_points[3].x + PICON_TITLE_X, printinfo_points[3].y + PICON_TITLE_Y, (u8 *)buf);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawLayer(int icon_pos)
{
  if (OS_GetTimeMs() > nextDrawTime)
  {
    char tempstr[10];
    sprintf(tempstr, "%.2fmm",
            (infoFile.source == BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_ZLAYER);
    GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                   (u8 *)LAYER_TITLE);
    GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    nextDrawTime = OS_GetTimeMs() + DRAW_TIME;
  }
}

static inline void toggleInfo(void)
{
  if (OS_GetTimeMs() > nextInfoTime)
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    {
      currentFan = (currentFan + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }
    currentSpeedID = (currentSpeedID + 1) % 2;
    nextInfoTime = OS_GetTimeMs() + TOGGLE_TIME;
    RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
    reDrawSpeed(SPD_ICON_POS);
    speedQuery();
    if (infoFile.source == BOARD_SD)
      coordinateQuery();
    if (!filDataSeen && isPrinting())
      updateFilamentUsed();
  }
}

static inline void printingDrawPage(void)
{
  //  Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);
  reValueNozzle(EXT_ICON_POS);
  reValueBed(BED_ICON_POS);
  reDrawFan(FAN_ICON_POS);
  reDrawTime(TIM_ICON_POS);
  reDrawProgress(TIM_ICON_POS);
  reDrawLayer(Z_ICON_POS);
  reDrawSpeed(SPD_ICON_POS);
}

void drawPrintInfo(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(rect_of_keySS[17].x0, rect_of_keySS[17].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[17].x0 + STATUS_MSG_ICON_XOFFSET, rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET, IconCharSelect(ICONCHAR_INFO));
  GUI_DispStringInRectEOL(rect_of_keySS[17].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET, rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET, rect_of_keySS[17].x1 - BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET, rect_of_keySS[17].y1 - STATUS_MSG_ICON_YOFFSET, (uint8_t *)textSelect(LABEL_PRINT_FINISHED));
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);

  Scroll_CreatePara(&infoScroll, (u8 *)filamentInfo, &msgRect);

  GUI_RestoreColorDefault();
}

void scrollInfo(void)
{
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&infoScroll, CENTER);
  GUI_RestoreColorDefault();
}

void stopConfirm(void)
{
  abortPrinting();
  infoMenu.cur--;
}

void printFinished(void)
{
  char tempstr[30];
  strcpy(filamentInfo, "");

  if (strlen((char *)getCurGcodeName(infoFile.title)) > MAX_FILE_CHAR)
  {
    strncpy(filData.name, (char *)getCurGcodeName(infoFile.title), MAX_FILE_CHAR);
    strcat(filData.name, "~");
  }
  else
    strcpy(filData.name, (char *)getCurGcodeName(infoFile.title));

  filData.time = infoPrinting.time;

  if (speedGetCurPercent(1) != 100)
  {
    filData.length = (filData.length * speedGetCurPercent(1)) / 100; // multiply by flow percentage
    filData.weight = (filData.weight * speedGetCurPercent(1)) / 100; // multiply by flow percentage
    filData.cost = (filData.cost * speedGetCurPercent(1)) / 100;     // multiply by flow percentage
  }
  if (filData.length != 0)
  {
    sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_LENGTH), filData.length);
    strcat(filamentInfo, tempstr);
  }
  if (filData.weight != 0)
  {
    sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_WEIGHT), filData.weight);
    if (strlen(filamentInfo) > 0)
      strcat(filamentInfo, ",  ");
    strcat(filamentInfo, tempstr);
  }
  if (filData.cost != 0)
  {
    sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_COST), filData.cost);
    if (strlen(filamentInfo) > 0)
      strcat(filamentInfo, ",  ");
    strcat(filamentInfo, tempstr);
  }
  if (strlen(filamentInfo) == 0)
    strcpy(filamentInfo, (char *)textSelect(LABEL_NO_FILAMENT_STATS));

  strcat(filamentInfo, "  ");
  strcat(filamentInfo, (char *)textSelect(LABEL_CLICK_FOR_MORE));
  drawPrintInfo();
}

void printInfoPopup(void)
{
  uint8_t hour = filData.time / 3600;
  uint8_t min = filData.time % 3600 / 60;
  uint8_t sec = filData.time % 60;
  char showInfo[150];
  char tempstr[30];

  strcpy(showInfo, (char *)textSelect(LABEL_FILE_NAME));
  strcat(showInfo, filData.name);
  strcat(showInfo, "\n");
  strcat(showInfo, (char *)textSelect(LABEL_PRINT_TIME));
  sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
  strcat(showInfo, tempstr);
  if (filData.length > 0)
  {
    sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_LENGTH), filData.length);
    strcat(showInfo, "\n");
    strcat(showInfo, tempstr);
  }
  if (filData.weight > 0)
  {
    sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_WEIGHT), filData.weight);
    strcat(showInfo, "\n");
    strcat(showInfo, tempstr);
  }
  if (filData.cost > 0)
  {
    sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_COST), filData.cost);
    strcat(showInfo, "\n");
    strcat(showInfo, tempstr);
  }
  popupReminder(DIALOG_TYPE_INFO, LABEL_INFO, (uint8_t *)showInfo);
}

void menuPrinting(void)
{
  //1title, ITEM_PER_PAGE item(icon + label)
  MENUITEMS printingItems = {
    //  title
    LABEL_BACKGROUND,
    // icon                       label
    {{ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND}}};

  uint8_t nowFan[MAX_FAN_COUNT] = {0};
  uint16_t curspeed[2] = {0};
  uint32_t time = 0;
  HEATER nowHeat;
  float curLayer = 0;
  bool lastPause = isPause();
  bool lastPrinting = isPrinting();
  memset(&nowHeat, 0, sizeof(HEATER));

  printingItems.title.address = getCurGcodeName(infoFile.title);
  if (lastPrinting == false)
  {
    printingItems.items[KEY_ICON_4] = itemIsPrinting[1];
    printingItems.items[KEY_ICON_5].icon = itemIsPrinting[0].icon;
  }
  else
  {
    printingItems.items[KEY_ICON_4] = itemIsPause[isPause()];
    printingItems.items[KEY_ICON_5].icon = (infoFile.source != BOARD_SD && infoPrinting.model_icon) ? ICON_PREVIEW : ICON_BABYSTEP;
  }
  printingItems.items[KEY_ICON_5].label = itemIsPrinting[lastPrinting * 2].label;
  printingItems.items[KEY_ICON_6] = itemIsPrinting[lastPrinting * 3];
  printingItems.items[KEY_ICON_7] = itemIsPrinting[lastPrinting + 4];
  menuDrawPage(&printingItems);
  printingDrawPage();
  if (lastPrinting == false)
    drawPrintInfo();

  while (infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
    //    Scroll_DispString(&titleScroll, LEFT); //Scroll display file name will take too many CPU cycles

    //check nozzle temp change
    if (nowHeat.T[currentTool].current != heatGetCurrentTemp(currentTool) ||
        nowHeat.T[currentTool].target != heatGetTargetTemp(currentTool))
    {
      nowHeat.T[currentTool].current = heatGetCurrentTemp(currentTool);
      nowHeat.T[currentTool].target = heatGetTargetTemp(currentTool);
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    //check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reValueBed(BED_ICON_POS);
    }

    //check Fan speed change
    if (nowFan[currentFan] != fanGetCurSpeed(currentFan))
    {
      nowFan[currentFan] = fanGetCurSpeed(currentFan);
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }

    //check printing progress
    if (infoPrinting.size != 0)
    {
      //check print time change
      if (time != infoPrinting.time ||
          infoPrinting.progress != MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100))
      {
        time = infoPrinting.time;
        infoPrinting.progress = MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reDrawTime(TIM_ICON_POS);
        reDrawProgress(TIM_ICON_POS);
      }
    }
    else
    {
      if (infoPrinting.progress != 100)
      {
        infoPrinting.progress = 100;
        reDrawTime(TIM_ICON_POS);
        reDrawProgress(TIM_ICON_POS);
      }
    }

    //Z_AXIS coordinate
    if (curLayer != ((infoFile.source == BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS)))
    {
      curLayer = (infoFile.source == BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS);
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reDrawLayer(Z_ICON_POS);
    }

    //check change in speed or flow
    if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
    {
      curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
      RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
      reDrawSpeed(SPD_ICON_POS);
    }

    // check if print is paused
    if (lastPause != isPause())
    {
      lastPause = isPause();
      printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
    }

    // check if print is just finished
    if (lastPrinting != isPrinting())
    {
      lastPrinting = isPrinting();
      if (lastPrinting == true) // print is ongoing
      {
        printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
      }
      else
      {
        printingItems.items[KEY_ICON_4] = itemIsPrinting[1];
      }
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
      printingItems.items[KEY_ICON_5] = itemIsPrinting[lastPrinting * 2];
      menuDrawItem(&printingItems.items[KEY_ICON_5], KEY_ICON_5);
      printingItems.items[KEY_ICON_6] = itemIsPrinting[lastPrinting * 3];
      menuDrawItem(&printingItems.items[KEY_ICON_6], KEY_ICON_6);
      printingItems.items[KEY_ICON_7] = itemIsPrinting[lastPrinting + 4];
      menuDrawItem(&printingItems.items[KEY_ICON_7], KEY_ICON_7);
      if (lastPrinting == false) // printing finished
      {
        printFinished();
      }
    }

    if (!isPrinting())
      scrollInfo();

    toggleInfo();

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_4:
        if (isPrinting())
        {
          setPrintPause(!isPause(), false);
        }
        else
        {
          infoMenu.cur = 0;
        }
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuBabystep;
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
          exitPrinting();
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
