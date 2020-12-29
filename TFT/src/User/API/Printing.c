#include "includes.h"
#include "Printing.h"


PRINTING infoPrinting;

bool filamentRunoutAlarm;

static bool updateM27_waiting = false;
static float last_E_pos;

void initEpos(void)
{
  last_E_pos = ((infoFile.source == BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
}

void updateFilamentUsed(void)
{
  float E_pos = ((infoFile.source == BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
  if ((E_pos + MAX_RETRACT_LIMIT) < last_E_pos) //Check whether E position reset (G92 E0)
  {
    last_E_pos = 0;
  }
  filData.length += (E_pos - last_E_pos) / 1000;
  last_E_pos = E_pos;
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
void setPrintingTime(uint32_t RTtime)
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
uint32_t getPrintSize(void)
{
  return infoPrinting.size;
}

//
void setPrintSize(uint32_t size)
{
  infoPrinting.size = size;
}

//
uint32_t getPrintCur(void)
{
  return infoPrinting.cur;
}

//
void setPrintCur(uint32_t cur)
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

void setRunoutAlarmTrue(void)
{
  filamentRunoutAlarm = true;
}

void setRunoutAlarmFalse(void)
{
  filamentRunoutAlarm = false;
}

bool getRunoutAlarm(void)
{
  return filamentRunoutAlarm;
}

void setPrintModelIcon(bool exist)
{
  infoPrinting.model_icon = exist;
}

bool getPrintModelIcon(void)
{
  return infoPrinting.model_icon;
}

uint8_t getPrintProgress(void)
{
  return infoPrinting.progress;
}

uint32_t getPrintTime(void)
{
  return infoPrinting.time;
}

void printSetUpdateWaiting(bool isWaiting)
{
  updateM27_waiting = isWaiting;
}

//only return gcode file name except path
//for example:"SD:/test/123.gcode"
//only return "123.gcode"
uint8_t *getCurGcodeName(char *path)
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
    mustStoreScript(printcodes.start_gcode);
    break;
  case 1:
    mustStoreScript(printcodes.end_gcode);
    break;
  case 2:
    mustStoreScript(printcodes.cancel_gcode);
    break;

  default:
    break;
  }
}

static inline void setM0Pause(bool m0_pause)
{
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
      if (is_pause)
        request_M25();
      else
        request_M24(0);
      break;

    case TFT_UDISK:
    case TFT_SD:
      infoPrinting.pause = is_pause;
      if(infoPrinting.pause == true && is_m0pause == false)
      {
        while (infoCmd.count != 0) {loopProcess();}
      }

      bool isCoorRelative = coorGetRelative();
      bool isExtrudeRelative = eGetRelative();
      static COORDINATE tmp;

      if(infoPrinting.pause)
      {
        //restore status before pause
        //if pause was triggered through M0/M1 then break
        if(is_m0pause == true)
        {
          setM0Pause(is_m0pause);
          popupReminder(DIALOG_TYPE_ALERT, LABEL_PAUSE, LABEL_PAUSE);
          break;
        }

        coordinateGetAll(&tmp);

        if (isCoorRelative == true)     mustStoreCmd("G90\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M82\n");

        if (heatGetCurrentTemp(heatGetCurrentHotend()) > infoSettings.min_ext_temp)
        {
          mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len,
                       infoSettings.pause_feedrate[FEEDRATE_E]);
        }
        if (coordinateIsKnown())
        {
          mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS] + infoSettings.pause_z_raise, infoSettings.pause_feedrate[FEEDRATE_Z]);
          mustStoreCmd("G1 X%.3f Y%.3f F%d\n", infoSettings.pause_pos[X_AXIS], infoSettings.pause_pos[Y_AXIS],
                       infoSettings.pause_feedrate[FEEDRATE_XY]);
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
          mustStoreCmd("G1 X%.3f Y%.3f F%d\n", tmp.axis[X_AXIS], tmp.axis[Y_AXIS], infoSettings.pause_feedrate[FEEDRATE_XY]);
          mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS], infoSettings.pause_feedrate[FEEDRATE_Z]);
        }
        if (heatGetCurrentTemp(heatGetCurrentHotend()) > infoSettings.min_ext_temp)
        {
          mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len + infoSettings.resume_purge_len,
                       infoSettings.pause_feedrate[FEEDRATE_E]);
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
  memset(&infoPrinting, 0, sizeof(PRINTING));
  ExitDir();
}

void endPrinting(void)
{
  switch (infoFile.source)
  {
    case BOARD_SD:
      break;

    case TFT_UDISK:
    case TFT_SD:
      f_close(&infoPrinting.file);
      break;
  }
  infoPrinting.printing = infoPrinting.pause = false;
  powerFailedClose();
  powerFailedDelete();
  if(infoSettings.send_end_gcode == 1)
  {
    sendPrintCodes(1);
  }
}

void printingFinished(void)
{
  BUZZER_PLAY(sound_success);
  endPrinting();
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
      infoHost.printing = false;
      //Several M108 are sent to Marlin because consecutive blocking operations
      // such as heating bed, extruder may defer processing of M524
      breakAndContinue();
      breakAndContinue();
      breakAndContinue();
      breakAndContinue();
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
    if(fanIsType(i, FAN_TYPE_F)) mustStoreCmd("%s S0\n", fanCmd[i]);
  }
  mustStoreCmd("M81\n");
  popupReminder(DIALOG_TYPE_INFO, LABEL_SHUT_DOWN, LABEL_SHUTTING_DOWN);
}

void shutdownLoop(void)
{
  bool tempIsLower = true;
  for (uint8_t i = NOZZLE0; i < infoSettings.hotend_count; i++)
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
  LABELCHAR(tempbody, LABEL_WAIT_TEMP_SHUT_DOWN);
  sprintf(tempstr, tempbody, infoSettings.auto_off_temp);

  for(u8 i = 0; i < infoSettings.fan_count; i++)
  {
    if(fanIsType(i,FAN_TYPE_F)) mustStoreCmd("%s S255\n", fanCmd[i]);
  }
  setDialogText(LABEL_SHUT_DOWN, (u8 *)tempstr, LABEL_FORCE_SHUT_DOWN, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, shutdown, NULL, shutdownLoop);
}


// get gcode command from sd card
void getGcodeFromFile(void)
{
  bool    sd_comment_mode = false;
  bool    sd_comment_space = true;
  char    sd_char;
  u8      sd_count = 0;
  UINT    br = 0;

  if(isPrinting() == false || infoFile.source == BOARD_SD)  return;

  powerFailedCache(infoPrinting.file.fptr);

  if(heatHasWaiting() || infoCmd.count || infoPrinting.pause )  return;

  if(moveCacheToCmd() == true) return;

  for(;infoPrinting.cur < infoPrinting.size;)
  {
    if(f_read(&infoPrinting.file, &sd_char, 1, &br)!=FR_OK) break;

    infoPrinting.cur++;

    //Gcode
    if (sd_char == '\n' )  //'\n' is end flag for per command
    {
      sd_comment_mode = false;  //for new command
      sd_comment_space= true;
      if(sd_count!=0)
      {
        infoCmd.queue[infoCmd.index_w].gcode[sd_count++] = '\n';
        infoCmd.queue[infoCmd.index_w].gcode[sd_count] = 0;  //terminate string
        infoCmd.queue[infoCmd.index_w].src = SERIAL_PORT;
        sd_count = 0; //clear buffer
        infoCmd.index_w = (infoCmd.index_w + 1) % CMD_MAX_LIST;
        infoCmd.count++;
        break;
      }
    }
    else if (sd_count >= CMD_MAX_CHAR - 2)
    {}  //when the command length beyond the maximum, ignore the following bytes
    else
    {
      if (sd_char == ';')  //';' is comment out flag
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
  setRunoutAlarmFalse();
  Serial_Puts(SERIAL_PORT, "M108\n");
}

void resumeAndPurge(void)
{
  setRunoutAlarmFalse();
  Serial_Puts(SERIAL_PORT, "M876 S0\n");
}

void resumeAndContinue(void)
{
  setRunoutAlarmFalse();
  Serial_Puts(SERIAL_PORT, "M876 S1\n");
}

bool hasPrintingMenu(void)
{
  for (uint8_t i = 0; i <= infoMenu.cur; i++)
  {
    if (infoMenu.menu[i] == menuPrinting) return true;
  }
  return false;
}

void loopCheckPrinting(void)
{
  #if defined(ST7920_SPI) || defined(LCD2004_simulator)
    if(infoMenu.menu[infoMenu.cur] == menuMarlinMode) return;
  #endif

  if (infoHost.printing && !infoPrinting.printing)
  {
    infoPrinting.printing = true;
    if (!hasPrintingMenu())
    {
      infoMenu.menu[++infoMenu.cur] = menuPrinting;
    }
  }

  if (infoFile.source != BOARD_SD) return;
  if (infoMachineSettings.autoReportSDStatus == ENABLED) return;
  if (!infoSettings.m27_active && !infoPrinting.printing) return;

  static uint32_t nextCheckPrintTime = 0;
  uint32_t update_M27_time = infoSettings.m27_refresh_time * 1000;

  do
  {  /* WAIT FOR M27  */
    if(updateM27_waiting == true)
    {
      nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
      break;
    }
    if(OS_GetTimeMs() < nextCheckPrintTime)
      break;
    if(storeCmd("M27\n") == false)
      break;
    nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
    updateM27_waiting = true;
  } while(0);
}
