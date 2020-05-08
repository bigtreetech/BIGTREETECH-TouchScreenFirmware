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

static u32 nextTime = 0;
static u32 toggle_time = 2000; // 1 seconds is 1000
TOOL c_Ext = NOZZLE0;
static int c_fan = 0;
static int c_speedID = 0;
static int key_pause = 4;
const char* Speed_ID[2] = {"Speed","Flow"};

static PRINT_GCODES * printcodes = NULL;

#define LAYER_TITLE "Layer"
#define EXT_ICON_POS  0
#define BED_ICON_POS  1
#define FAN_ICON_POS  2
#define TIM_ICON_POS  3
#define Z_ICON_POS    4
#define SPD_ICON_POS  5

#ifdef RAPID_SERIAL_COMM
#define rapid_serial_loop()  loopBackEnd()
#else
#define rapid_serial_loop()
#endif
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

#ifndef M27_WATCH_OTHER_SOURCES
#define M27_WATCH_OTHER_SOURCES    false
#endif

#ifndef M27_REFRESH
#define M27_REFRESH   3
#endif

static PRINTING infoPrinting;

static bool    update_waiting = false;

//
bool isPrinting(void)
{
  return infoPrinting.printing;
}

//
bool isPause(void)
{
  return infoPrinting.pause;
}

bool isM0_Pause(void)
{
return infoPrinting.m0_pause;
}

//
void setPrintingTime(u32 RTtime)
{
  if(RTtime%1000 == 0)
  {
    if(isPrinting() && !isPause())
    {
      infoPrinting.time++;
    }
  }
}

//
u32 getPrintSize(void)
{
  return infoPrinting.size;
}

//
void setPrintSize(u32 size)
{
  infoPrinting.size = size;
}

//
u32 getPrintCur(void)
{
  return infoPrinting.cur;
}

//
void setPrintCur(u32 cur)
{
  infoPrinting.cur = cur;
}

u8 getPrintProgress(void)
{
  return infoPrinting.progress;
}
u32 getPrintTime(void)
{
  return infoPrinting.time;
}

void printSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}

void printerGotoIdle(void)
{
  // disable all heater
  for(TOOL i = BED; i < (infoSettings.tool_count+1); i++) {
    mustStoreCmd("%s S0\n", heatCmd[i]);
  }
  // disable all fan
  for(u8 i = 0; i < (infoSettings.fan_count+1); i++) {
    mustStoreCmd("%s S0\n", fanCmd[i]);
  }
  // disable all stepper
  mustStoreCmd("M18\n");
}

//only return gcode file name except path
//for example:"SD:/test/123.gcode"
//only return "123.gcode"
u8 *getCurGcodeName(char *path)
{
  int i=strlen(path);
  for(; path[i]!='/'&& i>0; i--)
  {}
  return (u8* )(&path[i+1]);
}

void menuBeforePrinting(void)
{
  //load stat/end/cancel gcodes from spi flash
  printcodes = (PRINT_GCODES*)malloc(sizeof(PRINT_GCODES));
  uint8_t *data_p = (uint8_t *)printcodes;
  W25Qxx_ReadBuffer(data_p,CUSTOM_GCODE_ADDR,sizeof(printcodes));


  long size = 0;
  switch (infoFile.source)
  {
    case BOARD_SD: // GCode from file on ONBOARD SD
      size = request_M23(infoFile.title+5);

      //  if( powerFailedCreate(infoFile.title)==false)
      //  {
      //
      //  }	  // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

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
        mustStoreCmd(printcodes->start_gcode);
      }
      break;
  }
  infoPrinting.printing = true;
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

void setM0Pause(bool m0_pause){
  infoPrinting.m0_pause = m0_pause;
}

bool setPrintPause(bool is_pause, bool is_m0pause)
{
  static bool pauseLock = false;
  if(pauseLock)                      return false;
  if(!isPrinting())                  return false;
  if(infoPrinting.pause == is_pause) return false;

  pauseLock = true;
  switch (infoFile.source)
  {
    case BOARD_SD:
      infoPrinting.pause = is_pause;
      if (is_pause){
        request_M25();
      } else {
        request_M24(0);
      }
      break;

    case TFT_UDISK:
    case TFT_SD:
      infoPrinting.pause = is_pause;
      if(infoPrinting.pause == true && is_m0pause == false){
      while (infoCmd.count != 0) {loopProcess();}
      }

      bool isCoorRelative = coorGetRelative();
      bool isExtrudeRelative = eGetRelative();
      static COORDINATE tmp;

      if(infoPrinting.pause)
      {
        //restore status before pause
        //if pause was triggered through M0/M1 then break
      if(is_m0pause == true) {
        setM0Pause(is_m0pause);
        popupReminder(textSelect(LABEL_PAUSE), textSelect(LABEL_M0_PAUSE));
        break;
        }

        coordinateGetAll(&tmp);
        if (isCoorRelative == true)     mustStoreCmd("G90\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M82\n");

        if (heatGetCurrentTemp(heatGetCurrentToolNozzle()) > infoSettings.min_ext_temp)
        {
          mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len, infoSettings.pause_feedrate[E_AXIS]);
        }
        if (coordinateIsKnown())
        {
          mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS] + infoSettings.pause_z_raise, infoSettings.pause_feedrate[E_AXIS]);
          mustStoreCmd("G1 X%d Y%d F%d\n", infoSettings.pause_pos[X_AXIS], infoSettings.pause_pos[Y_AXIS], infoSettings.pause_feedrate[X_AXIS]);
        }

        if (isCoorRelative == true)     mustStoreCmd("G91\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M83\n");
      }
      else
      {
        if(isM0_Pause() == true)
        {
          setM0Pause(is_m0pause);
          Serial_Puts(SERIAL_PORT, "M108\n");
          break;
        }
        if (isCoorRelative == true)     mustStoreCmd("G90\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M82\n");

        if (coordinateIsKnown())
        {
          mustStoreCmd("G1 X%.3f Y%.3f F%d\n", tmp.axis[X_AXIS], tmp.axis[Y_AXIS], infoSettings.pause_feedrate[X_AXIS]);
          mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS], infoSettings.pause_feedrate[Z_AXIS]);
        }
        if(heatGetCurrentTemp(heatGetCurrentToolNozzle()) > infoSettings.min_ext_temp)
        {
          mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len + infoSettings.resume_purge_len, infoSettings.pause_feedrate[E_AXIS]);
        }
        mustStoreCmd("G92 E%.5f\n", tmp.axis[E_AXIS]);
        mustStoreCmd("G1 F%d\n", tmp.feedrate);

        if (isCoorRelative == true)     mustStoreCmd("G91\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M83\n");
      }
      break;
  }
  resumeToPause(is_pause);
  pauseLock = false;
  return true;
}

const GUI_RECT progressRect = {1*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + ICON_HEIGHT/4,
                               3*SPACE_X_PER_ICON, 0*ICON_HEIGHT+0*SPACE_Y+ICON_START_Y + ICON_HEIGHT*3/4};

#define BED_X  (progressRect.x1 - 9 * BYTE_WIDTH)
#define TEMP_Y (progressRect.y1 + 3)
#define TIME_Y (TEMP_Y + 1 * BYTE_HEIGHT + 3)

void reValueNozzle(int icon_pos)
{
  char tempstr[10];
  my_sprintf(tempstr, "%d/%d", heatGetCurrentTemp(c_Ext), heatGetTargetTemp(c_Ext));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_NOZZLE));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y, (u8* )heatDisplayID[c_Ext]);
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
  u8 fs;
  if (infoSettings.fan_percentage == 1)
  {
    fs = (fanGetSpeed(c_fan) * 100) / 255;
    my_sprintf(tempstr, "%d%%", fs);
  }
  else
  {
    fs = fanGetSpeed(c_fan);
    my_sprintf(tempstr, "%d", fs);
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
  sprintf(tempstr, "%02d:%02d:%02d", hour,min,sec);
  ICON_CustomReadDisplay(printinfo_points[icon_pos].x,printinfo_points[icon_pos].y,PICON_LG_WIDTH,PICON_HEIGHT,ICON_ADDR(ICON_PRINTING_TIMER));
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (u8 *)tempstr);
  //GUI_DispDec(printinfo_val_rect[icon_pos].x0 + 2 * BYTE_WIDTH, TIME_Y, hour, 2, LEFT);
  //GUI_DispDec(printinfo_val_rect[icon_pos].x0 + 5 * BYTE_WIDTH, TIME_Y, min, 2, LEFT);
  //GUI_DispDec(printinfo_val_rect[icon_pos].x0 + 8 * BYTE_WIDTH, TIME_Y, sec, 2, LEFT);

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
    if (infoSettings.tool_count > 1)
    {
      c_Ext = (TOOL)((c_Ext + 1) % (infoSettings.tool_count+1));
      if (c_Ext == 0)
      {
        c_Ext += 1;
      }
      rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle(EXT_ICON_POS);
    }

    if (infoSettings.fan_count > 1)
    {
      c_fan = (c_fan + 1) % FAN_NUM;
      rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
      reDrawFan(FAN_ICON_POS);
    }

    c_speedID = (c_speedID + 1) % 2;
    nextTime = OS_GetTimeMs() + toggle_time;
    rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
    reDrawSpeed(SPD_ICON_POS);
  }
}

//extern SCROLL   titleScroll;
//extern GUI_RECT titleRect;


void printingDrawPage(void)
{
  //	Scroll_CreatePara(&titleScroll, infoFile.title,&titleRect);  //
  if(get_Pre_Icon() == true){
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
  KEY_VALUES  key_num = KEY_IDLE;
  u32         time = 0;
  HEATER      nowHeat;
  float       curLayer = 0;
  u8          nowFan[FAN_NUM] = {0};
  uint16_t    curspeed[2] = {0};
  memset(&nowHeat, 0, sizeof(HEATER));

  printingDrawPage();
  printingItems.items[key_pause] = itemIsPause[infoPrinting.pause];


  while(infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
//    Scroll_DispString(&titleScroll, LEFT); //Scroll display file name will take too many CPU cycles

    //check nozzle temp change
      if (nowHeat.T[c_Ext].current != heatGetCurrentTemp(c_Ext) || nowHeat.T[c_Ext].target != heatGetTargetTemp(c_Ext))
      {
        nowHeat.T[c_Ext].current = heatGetCurrentTemp(c_Ext);
        nowHeat.T[c_Ext].target = heatGetTargetTemp(c_Ext);
        rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
        reValueNozzle(EXT_ICON_POS);
      }

    //check bed temp change
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      rapid_serial_loop();	 //perform backend printing loop before drawing to avoid printer idling
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
        if(get_Pre_Icon() != true){
        setPrintPause(!isPause(),false);
        }
        break;

      case KEY_ICON_5:
        if(get_Pre_Icon() == true){
        setPrintPause(!isPause(),false);
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
    loopProcess();
    toggleinfo();
  }
}

void exitPrinting(void)
{
  memset(&infoPrinting,0,sizeof(PRINTING));
  ExitDir();
}

void endPrinting(void)
{
  switch (infoFile.source)
  {
    case BOARD_SD:
      printSetUpdateWaiting(infoSettings.m27_active);
      break;

    case TFT_UDISK:
    case TFT_SD:
      f_close(&infoPrinting.file);
      break;
  }
  infoPrinting.printing = infoPrinting.pause = false;
  powerFailedClose();
  powerFailedDelete();
  if(infoSettings.send_end_gcode == 1){
    mustStoreCmd(printcodes->end_gcode);
  }
  free(printcodes);
  printerGotoIdle();
}


void completePrinting(void)
{
  BUZZER_PLAY(sound_success);
  endPrinting();
  printingItems.items[KEY_ICON_7].icon = ICON_BACK;
  printingItems.items[KEY_ICON_7].label.index = LABEL_BACK;
  if (infoMenu.menu[infoMenu.cur] == menuPrinting)
    menuDrawItem(&printingItems.items[KEY_ICON_7], KEY_ICON_7);
  if(infoSettings.auto_off) // Auto shut down after printing
  {
		infoMenu.menu[++infoMenu.cur] = menuShutDown;
  }
}

void abortPrinting(void)
{
  switch (infoFile.source)
  {
    case BOARD_SD:
      request_M524();
      break;

    case TFT_UDISK:
    case TFT_SD:
      if (infoSettings.send_cancel_gcode == 1)
        mustStoreCmd(PRINT_CANCEL_GCODE);

      clearCmdQueue();
      break;
  }

  heatClearIsWaiting();

  endPrinting();
  exitPrinting();
}

void menuStopPrinting(void)
{
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_STOP_PRINT), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

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

// Shut down menu, when the hotend temperature is higher than "AUTO_SHUT_DOWN_MAXTEMP"
// wait for cool down, in the meantime, you can shut down by force
void menuShutDown(void)
{
  bool tempIsLower;
  u16 key_num = IDLE_TOUCH;
  char tempstr[75];
  my_sprintf(tempstr,(char*)textSelect(LABEL_WAIT_TEMP_SHUT_DOWN),infoSettings.auto_off_temp);
  popupDrawPage(bottomDoubleBtn, textSelect(LABEL_SHUT_DOWN), (u8*)tempstr, textSelect(LABEL_FORCE_SHUT_DOWN), textSelect(LABEL_CANCEL));

  for(u8 i = 0; i < FAN_NUM; i++)
  {
    mustStoreCmd("%s S255\n", fanCmd[i]);
  }
  while (infoMenu.menu[infoMenu.cur] == menuShutDown)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        goto shutdown;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }
    tempIsLower = true;
    for (TOOL i = NOZZLE0; i < HEATER_NUM; i++)
    {
      if(heatGetCurrentTemp(NOZZLE0) >= AUTO_SHUT_DOWN_MAXTEMP)
        tempIsLower = false;
    }
    if(tempIsLower)
    {
      shutdown:
        for(u8 i = 0; i < FAN_NUM; i++)
        {
          mustStoreCmd("%s S0\n", fanCmd[i]);
        }
        mustStoreCmd("M81\n");
        infoMenu.cur--;
        popupReminder(textSelect(LABEL_SHUT_DOWN), textSelect(LABEL_SHUTTING_DOWN));
    }
    loopProcess();
  }
}

// get gcode command from sd card
void getGcodeFromFile(void)
{
  bool    sd_comment_mode = false;
  bool    sd_comment_space = true;
  char    sd_char;
  u8      sd_count = 0;
  UINT    br = 0;

  if(isPrinting()==false || infoFile.source == BOARD_SD)  return;

  powerFailedCache(infoPrinting.file.fptr);

  if(heatHasWaiting() || infoCmd.count || infoPrinting.pause )  return;

  if(moveCacheToCmd() == true) return;

  for(;infoPrinting.cur < infoPrinting.size;)
  {
    if(f_read(&infoPrinting.file, &sd_char, 1, &br)!=FR_OK) break;

    infoPrinting.cur++;

    //Gcode
    if (sd_char == '\n' )         //'\n' is end flag for per command
    {
      sd_comment_mode = false;   //for new command
      sd_comment_space= true;
      if(sd_count!=0)
      {
        infoCmd.queue[infoCmd.index_w].gcode[sd_count++] = '\n';
        infoCmd.queue[infoCmd.index_w].gcode[sd_count] = 0; //terminate string
        infoCmd.queue[infoCmd.index_w].src = SERIAL_PORT;
        sd_count = 0; //clear buffer
        infoCmd.index_w = (infoCmd.index_w + 1) % CMD_MAX_LIST;
        infoCmd.count++;
        break;
      }
    }
    else if (sd_count >= CMD_MAX_CHAR - 2) {	}   //when the command length beyond the maximum, ignore the following bytes
    else
    {
      if (sd_char == ';')             //';' is comment out flag
        sd_comment_mode = true;
      else
      {
        if(sd_comment_space && (sd_char== 'G'||sd_char == 'M'||sd_char == 'T'))  //ignore ' ' space bytes
          sd_comment_space = false;
        if (!sd_comment_mode && !sd_comment_space && sd_char != '\r')  //normal gcode
          infoCmd.queue[infoCmd.index_w].gcode[sd_count++] = sd_char;
      }
    }
  }

  if((infoPrinting.cur>=infoPrinting.size) && isPrinting())  // end of .gcode file
  {
    completePrinting();
  }
}

void loopCheckPrinting(void)
{
  static u32  nextTime=0;
  u32 update_time = infoSettings.m27_refresh_time * 1000;
  do
  {  /* WAIT FOR M27	*/
    if(update_waiting == true) {nextTime=OS_GetTimeMs()+update_time; break;}
    if(OS_GetTimeMs() < nextTime) break;

    if(storeCmd("M27\n")==false) break;

    nextTime=OS_GetTimeMs()+update_time;
    update_waiting=true;
  }while(0);
}
