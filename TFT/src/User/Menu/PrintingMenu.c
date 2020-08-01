#include "Printing.h"
#include "includes.h"

const GUI_POINT printinfo_points[6] = {
{START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0, ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0},
{START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1, ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0},
{START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2, ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0},
{START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0, ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1},
{START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1, ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1},
{START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2, ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1},
};

const GUI_RECT printinfo_val_rect[6] = {
  {START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_SM_EX,     ICON_START_Y + PICON_HEIGHT*0 + PICON_SPACE_Y*0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*0 + PICON_SPACE_X*0 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_X,              ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*1 + PICON_SPACE_X*1 + PICON_VAL_LG_EX,     ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_X,               ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y,
        START_X + PICON_LG_WIDTH*2 + PICON_SPACE_X*2 + PICON_VAL_SM_EX,     ICON_START_Y + PICON_HEIGHT*1 + PICON_SPACE_Y*1 + PICON_VAL_Y + BYTE_HEIGHT},
};

static uint32_t nextTime = 0;
static uint32_t toggle_time = 2000; // 1 seconds is 1000
static uint8_t c_Tool = NOZZLE0;
static int c_fan = 0;
static int c_speedID = 0;
static int key_pause = 4;
const char* Speed_ID[2] = {"Speed","Flow"};


#define LAYER_TITLE "Layer"
#define EXT_ICON_POS  0
#define BED_ICON_POS  1
#define FAN_ICON_POS  2
#define TIM_ICON_POS  3
#define Z_ICON_POS    4
#define SPD_ICON_POS  5

//1title, ITEM_PER_PAGE item(icon + label)
MENUITEMS printingItems = {
//  title
LABEL_BACKGROUND,
// icon                       label
 {{ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_BABYSTEP,             LABEL_BABYSTEP},
  {ICON_MORE,                 LABEL_MORE},
  {ICON_STOP,                 LABEL_STOP},}
};
const ITEM itemBlank      = {ICON_BACKGROUND, LABEL_BACKGROUND};
const ITEM itemBabyStep   = {ICON_BABYSTEP, LABEL_BABYSTEP};
const ITEM itemIsPause[2] = {
// icon                       label
  {ICON_PAUSE,                LABEL_PAUSE},
  {ICON_RESUME,               LABEL_RESUME},
};


void completePrinting(void)
{
  printingItems.items[KEY_ICON_7].icon = ICON_BACK;
  printingItems.items[KEY_ICON_7].label.index = LABEL_BACK;
  if (infoMenu.menu[infoMenu.cur] == menuPrinting)
    menuDrawItem(&printingItems.items[KEY_ICON_7], KEY_ICON_7);
}

void menuBeforePrinting(void)
{
  //load stat/end/cancel gcodes from spi flash
  long size = 0;
  switch (infoFile.source)
  {
    case BOARD_SD: // GCode from file on ONBOARD SD
      size = request_M23(infoFile.title+5);

      //  if( powerFailedCreate(infoFile.title)==false)
      //  {
      //
      //  }    // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

      if(size == 0)
      {
        ExitDir();
        infoMenu.cur--;
        return;
      }

      infoPrinting.size  = size;

      //    if(powerFailedExist())
      //    {
      request_M24(0);
      //    }
      //    else
      //    {
      //      request_M24(infoBreakPoint.offset);
      //    }
      printSetUpdateWaiting(true);

      if (infoMachineSettings.autoReportSDStatus ==1){
        request_M27(infoSettings.m27_refresh_time*1000);                //Check if there is a SD or USB print running.
      }
      else{
        request_M27(0);
      }

      infoHost.printing=true; // Global lock info on printer is busy in printing.

      break;

    case TFT_UDISK:
    case TFT_SD: // GCode from file on TFT SD
      if(f_open(&infoPrinting.file,infoFile.title, FA_OPEN_EXISTING | FA_READ) != FR_OK)
      {
        ExitDir();
        infoMenu.cur--;
        return ;
      }
      if( powerFailedCreate(infoFile.title)==false)
      {

      }
      powerFailedlSeek(&infoPrinting.file);

      infoPrinting.size  = f_size(&infoPrinting.file);
      infoPrinting.cur   = infoPrinting.file.fptr;
      if(infoSettings.send_start_gcode == 1 && infoPrinting.cur == 0){ // PLR continue printing, CAN NOT use start gcode
        sendPrintCodes(0);
      }
      break;
  }
  infoPrinting.printing = true;
  setPrintfinishAction(completePrinting);
  infoMenu.menu[infoMenu.cur] = menuPrinting;
  printingItems.title.address = getCurGcodeName(infoFile.title);
  printingItems.items[KEY_ICON_7].icon = ICON_STOP;
  printingItems.items[KEY_ICON_7].label.index = LABEL_STOP;
}


void resumeToPause(bool is_pause)
{
  if(infoMenu.menu[infoMenu.cur] != menuPrinting) return;
  printingItems.items[key_pause] = itemIsPause[is_pause];
  menuDrawItem(&printingItems.items[key_pause],key_pause);
}


const GUI_RECT progressRect = {1*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + ICON_HEIGHT/4,
                               3*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + ICON_HEIGHT*3/4};

#define BED_X  (progressRect.x1 - 9 * BYTE_WIDTH)
#define TEMP_Y (progressRect.y1 + 3)
#define TIME_Y (TEMP_Y + 1 * BYTE_HEIGHT + 3)

void reValueNozzle(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(c_Tool), heatGetTargetTemp(c_Tool));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_NOZZLE));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )heatDisplayID[c_Tool]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reValueBed(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_BED));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )heatDisplayID[BED]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawFan(int icon_pos)
{
  char tempstr[10];
  if (infoSettings.fan_percentage == 1)
  {
    my_sprintf(tempstr, "%d%%", fanGetSpeedPercent(c_fan));
  }
  else
  {
    my_sprintf(tempstr, "%d", fanGetSpeed(c_fan));
  }

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_SM_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_FAN));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8*)fanID[c_fan]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}


void reDrawSpeed(int icon_pos)
{
  char tempstr[10];
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  my_sprintf(tempstr, "%d%%", speedGetPercent(c_speedID) );

  if(c_speedID == 0){
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_SM_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_SPEED));
  }
  else{
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_SM_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_FLOW));
  }
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8 *)Speed_ID[c_speedID]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawTime(int icon_pos)
{
  u8  hour = infoPrinting.time/3600,
      min = infoPrinting.time%3600/60,
      sec = infoPrinting.time%60;

  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  char tempstr[10];
  sprintf(tempstr, "%02u:%02u:%02u", hour,min,sec);
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_TIMER));
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawProgress(int icon_pos)
{
  char buf[6];
  my_sprintf(buf, "%d%%", infoPrinting.progress);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  GUI_DispString(printinfo_points[3].x + PICON_TITLE_X, printinfo_points[3].y + PICON_TITLE_Y, (u8 *)buf);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void reDrawLayer(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%.2fMM",coordinateGetAxisTarget(Z_AXIS));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_ZLAYER));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )LAYER_TITLE);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

void toggleinfo(void)
{
  if (OS_GetTimeMs() > nextTime)
  {
    if (infoSettings.hotend_count > 1)
    {
      c_Tool = (c_Tool + 1) % infoSettings.hotend_count;
      rapid_serial_loop();   //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    if (infoSettings.fan_count > 1)
    {
      c_fan = (c_fan + 1) % infoSettings.fan_count;
      rapid_serial_loop();   //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }

    c_speedID = (c_speedID + 1) % 2;
    nextTime = OS_GetTimeMs() + toggle_time;
    rapid_serial_loop();   //perform backend printing loop before drawing to avoid printer idling
    reDrawSpeed(SPD_ICON_POS);
  }
}

//extern SCROLL   titleScroll;
//extern GUI_RECT titleRect;


void printingDrawPage(void)
{
  //  Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);  //
  if(infoPrinting.model_icon){
    key_pause = 5;
    //printingItems.items[key_pause - 1] = itemBlank;
    printingItems.items[key_pause - 1].icon = ICON_PREVIEW;
    printingItems.items[key_pause - 1].label.index = LABEL_BACKGROUND;
  }
  else{
    key_pause = 4;
    printingItems.items[key_pause+1] = itemBabyStep;
  }

    printingItems.items[key_pause] = itemIsPause[isPause()];

  menuDrawPage(&printingItems);
  reValueNozzle(EXT_ICON_POS);
  reValueBed(BED_ICON_POS);
  reDrawFan(FAN_ICON_POS);
  reDrawTime(TIM_ICON_POS);
  reDrawProgress(TIM_ICON_POS);
  reDrawLayer(Z_ICON_POS);
  reDrawSpeed(SPD_ICON_POS);
}


void menuPrinting(void)
{
  KEY_VALUES  key_num;
  u32         time = 0;
  HEATER      nowHeat;
  float       curLayer = 0;
  u8          nowFan[MAX_FAN_COUNT] = {0};
  uint16_t    curspeed[2] = {0};
  memset(&nowHeat, 0, sizeof(HEATER));

  printingDrawPage();
  printingItems.items[key_pause] = itemIsPause[infoPrinting.pause];


  while(infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
//    Scroll_DispString(&titleScroll, LEFT); //Scroll display file name will take too many CPU cycles

    //check nozzle temp change
    if (nowHeat.T[c_Tool].current != heatGetCurrentTemp(c_Tool) || nowHeat.T[c_Tool].target != heatGetTargetTemp(c_Tool))
    {
      nowHeat.T[c_Tool].current = heatGetCurrentTemp(c_Tool);
      nowHeat.T[c_Tool].target = heatGetTargetTemp(c_Tool);
      rapid_serial_loop();   //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    //check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      rapid_serial_loop();   //perform backend printing loop before drawing to avoid printer idling
      reValueBed(BED_ICON_POS);
    }

    //check Fan speed change
    if (nowFan[c_fan] != fanGetSpeed(c_fan))
    {
      nowFan[c_fan] = fanGetSpeed(c_fan);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }

    //check printing progress
    if( infoPrinting.size != 0)
    {
      //check print time change
      if(time!=infoPrinting.time || infoPrinting.progress!=limitValue(0,(uint64_t)infoPrinting.cur*100/infoPrinting.size,100))
      {
        time=infoPrinting.time;
        infoPrinting.progress=limitValue(0,(uint64_t)infoPrinting.cur*100/infoPrinting.size,100);
        rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
        reDrawTime(TIM_ICON_POS);
        reDrawProgress(TIM_ICON_POS);
      }
    }
    else
    {
      if(infoPrinting.progress != 100)
      {
        infoPrinting.progress = 100;
        reDrawTime(TIM_ICON_POS);
        reDrawProgress(TIM_ICON_POS);
      }
    }

    //Z_AXIS coordinate
    if(curLayer != coordinateGetAxisTarget(Z_AXIS)){
      curLayer = coordinateGetAxisTarget(Z_AXIS);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawLayer(Z_ICON_POS);
    }

    //check change in speed or flow
    if(curspeed[c_speedID] != speedGetPercent(c_speedID)){
      curspeed[c_speedID] = speedGetPercent(c_speedID);
      rapid_serial_loop();  //perform backend printing loop before drawing to avoid printer idling
      reDrawSpeed(SPD_ICON_POS);
    }

    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_4:
        if (!infoPrinting.model_icon){
          setPrintPause(!isPause(), false);
          resumeToPause(isPause());
        }
        break;

      case KEY_ICON_5:
        if (infoPrinting.model_icon){
          setPrintPause(!isPause(), false);
          resumeToPause(isPause());
        }
        else{
          infoMenu.menu[++infoMenu.cur] = menuBabyStep;
        }
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuMore;
        break;

      case KEY_ICON_7:
        if(isPrinting())
          infoMenu.menu[++infoMenu.cur] = menuStopPrinting;
        else
        {
          exitPrinting();
          infoMenu.cur--;
        }
        break;

      default :break;
    }
    toggleinfo();
    loopProcess();
  }
}

void menuStopPrinting(void)
{
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(DIALOG_TYPE_ALERT, bottomDoubleBtn, textSelect(LABEL_WARNING),
                  textSelect(LABEL_STOP_PRINT), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

  while(infoMenu.menu[infoMenu.cur] == menuStopPrinting)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        abortPrinting();
        infoMenu.cur-=2;
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }
    loopProcess();
  }
}
