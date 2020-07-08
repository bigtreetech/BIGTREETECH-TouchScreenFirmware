#include "includes.h"
#include "Printing.h"


PRINTING infoPrinting;

static void (*action_printfinish)() = NULL;

static bool update_waiting = false;


//
void setPrintfinishAction(void (*_printfinish)())
{
  action_printfinish = _printfinish;
}

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

bool getPrintRunout(void)
{
  return infoPrinting.runout;
}

void setPrintRunout(bool runout)
{
  infoPrinting.runout = runout;
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
  for (TOOL i = BED; i < HEATER_COUNT; i++)
  {
    mustStoreCmd("%s S0\n", heatCmd[i]);
  }
  // disable all fan
  for (u8 i = 0; i < (infoSettings.fan_count); i++)
  {
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

//send print codes [0: start gcode, 1: end gcode 2: cancel gcode]
void sendPrintCodes(uint8_t index)
{
  PRINT_GCODES printcodes;
  W25Qxx_ReadBuffer((uint8_t*)&printcodes,PRINT_GCODES_ADDR,sizeof(PRINT_GCODES));
  switch (index)
  {
  case 0:
    mustStoreCmd(printcodes.start_gcode);
    break;
  case 1:
    mustStoreCmd(printcodes.end_gcode);
    break;
  case 2:
    mustStoreCmd(printcodes.cancel_gcode);
    break;

  default:
    break;
  }
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
        popupReminder(DIALOG_TYPE_ALERT, textSelect(LABEL_PAUSE), textSelect(LABEL_M0_PAUSE));
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
          mustStoreCmd("G1 X%.3f Y%.3f F%d\n", infoSettings.pause_pos[X_AXIS], infoSettings.pause_pos[Y_AXIS], infoSettings.pause_feedrate[X_AXIS]);
        }

        if (isCoorRelative == true)     mustStoreCmd("G91\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M83\n");
      }
      else
      {
        if(isM0_Pause() == true)
        {
          setM0Pause(is_m0pause);
          breakAndContinue();
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
  pauseLock = false;
  return true;
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
    sendPrintCodes(1);
  }
  printerGotoIdle();
}


void printingFinished(void)
{
  BUZZER_PLAY(sound_success);
  endPrinting();
  if(action_printfinish != NULL)
    action_printfinish();
  if(infoSettings.auto_off) // Auto shut down after printing
  {
    startShutdown();
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
      clearCmdQueue();
      if (infoSettings.send_cancel_gcode == 1)
        sendPrintCodes(2);
      break;
  }
  heatClearIsWaiting();

  endPrinting();
  exitPrinting();
}

// Shut down menu, when the hotend temperature is higher than "AUTO_SHUT_DOWN_MAXTEMP"
// wait for cool down, in the meantime, you can shut down by force
void shutdown(void)
{
  for(u8 i = 0; i < infoSettings.fan_count; i++)
  {
    mustStoreCmd("%s S0\n", fanCmd[i]);
  }
  mustStoreCmd("M81\n");
  popupReminder(DIALOG_TYPE_INFO, textSelect(LABEL_SHUT_DOWN), textSelect(LABEL_SHUTTING_DOWN));
}

void shutdownLoop(void)
{
  bool tempIsLower = true;
  for (TOOL i = NOZZLE0; i < HEATER_COUNT; i++)
  {
    if (heatGetCurrentTemp(i) >= AUTO_SHUT_DOWN_MAXTEMP)
      tempIsLower = false;
  }
  if (tempIsLower)
  {
   shutdown();
  }
}

void startShutdown(void)
{
  char tempstr[75];
  my_sprintf(tempstr, (char *)textSelect(LABEL_WAIT_TEMP_SHUT_DOWN), infoSettings.auto_off_temp);

  for(u8 i = 0; i < infoSettings.fan_count; i++)
  {
    mustStoreCmd("%s S255\n", fanCmd[i]);
  }
  showDialog(DIALOG_TYPE_INFO,textSelect(LABEL_SHUT_DOWN), (u8 *)tempstr,
              textSelect(LABEL_FORCE_SHUT_DOWN), textSelect(LABEL_CANCEL), shutdown, NULL, shutdownLoop);
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
    else if (sd_count >= CMD_MAX_CHAR - 2) {  }   //when the command length beyond the maximum, ignore the following bytes
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
    printingFinished();
  }
}

void breakAndContinue(void)
{
   Serial_Puts(SERIAL_PORT, "M108\n");
}

void loopCheckPrinting(void)
{
  static u32  nextTime=0;
  u32 update_time = infoSettings.m27_refresh_time * 1000;
  do
  {  /* WAIT FOR M27  */
    if(update_waiting == true) {nextTime=OS_GetTimeMs()+update_time; break;}
    if(OS_GetTimeMs() < nextTime) break;

    if(storeCmd("M27\n")==false) break;

    nextTime=OS_GetTimeMs()+update_time;
    update_waiting=true;
  }while(0);
}
