#include "Printing.h"
#include "includes.h"

typedef struct
{
  FIL        file;
  uint32_t   time;        // Printed time in sec
  uint32_t   size;        // Gcode file total size
  uint32_t   cur;         // Gcode has printed file size
  uint8_t    progress;
  bool       runout;      // 1: runout in printing, 0: idle
  bool       printing;    // 1: means printing, 0: means idle
  bool       pause;       // 1: means paused
  PAUSE_TYPE pauseType;   // pause type trigged by different sources and gcodes like M0 & M600
} PRINTING;

PRINTING infoPrinting;
PRINT_SUMMARY infoPrintSummary = {.name[0] = '\0', 0, 0, 0, 0};

static bool updateM27_waiting = false;
static float last_E_pos;
bool filamentRunoutAlarm;

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

void breakAndContinue(void)
{
  setRunoutAlarmFalse();
  clearCmdQueue();
  Serial_Puts(SERIAL_PORT, "M108\n");
}

void resumeAndPurge(void)
{
  setRunoutAlarmFalse();
  clearCmdQueue();
  Serial_Puts(SERIAL_PORT, "M876 S0\n");
}

void resumeAndContinue(void)
{
  setRunoutAlarmFalse();
  clearCmdQueue();
  Serial_Puts(SERIAL_PORT, "M876 S1\n");
}

void setPrintTime(uint32_t RTtime)
{
  if (RTtime % 1000 == 0)
  {
    if (infoPrinting.printing && !infoPrinting.pause)
    {
      infoPrinting.time++;
    }
  }
}

uint32_t getPrintTime(void)
{
  return infoPrinting.time;
}

void getPrintTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec)
{
  *hour = infoPrinting.time / 3600;
  *min = infoPrinting.time % 3600 / 60;
  *sec = infoPrinting.time % 60;
}

uint32_t getPrintSize(void)
{
  return infoPrinting.size;
}

uint32_t getPrintCur(void)
{
  return infoPrinting.cur;
}

void setPrintProgress(float cur, float size)
{
  infoPrinting.cur = cur;
  infoPrinting.size = size;
}

bool updatePrintProgress(void)
{
  uint8_t curProgress = infoPrinting.progress;

  // in case not printing or a wrong size was set, we consider progress as 100%
  if (infoPrinting.size == 0)  // avoid a division for 0 (a crash) and set progress to 100%
    infoPrinting.progress = 100;
  else
    infoPrinting.progress = MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100);

  if (infoPrinting.progress != curProgress)
    return true;

  return false;
}

uint8_t getPrintProgress(void)
{
  return infoPrinting.progress;
}

void setPrintRunout(bool runout)
{
  infoPrinting.runout = runout;
}

bool getPrintRunout(void)
{
  return infoPrinting.runout;
}

// Shut down menu, when the hotend temperature is higher than "AUTO_SHUT_DOWN_MAXTEMP"
// wait for cool down, in the meantime, you can shut down by force
void shutdown(void)
{
  for (uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (fanIsType(i, FAN_TYPE_F)) mustStoreCmd("%s S0\n", fanCmd[i]);
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

void shutdownStart(void)
{
  char tempstr[75];

  LABELCHAR(tempbody, LABEL_WAIT_TEMP_SHUT_DOWN);
  sprintf(tempstr, tempbody, infoSettings.auto_off_temp);

  for (uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (fanIsType(i, FAN_TYPE_F)) mustStoreCmd("%s S255\n", fanCmd[i]);
  }

  setDialogText(LABEL_SHUT_DOWN, (uint8_t *)tempstr, LABEL_FORCE_SHUT_DOWN, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, shutdown, NULL, shutdownLoop);
}

void initPrintSummary(void)
{
  last_E_pos = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
  infoPrintSummary = (PRINT_SUMMARY){.name[0] = '\0', 0, 0, 0, 0};
  hasFilamentData = false;
}

void preparePrintSummary(void)
{
  if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
    sprintf(infoPrintSummary.name,"%." STRINGIFY(SUMMARY_NAME_LEN) "s", infoFile.Longfile[infoFile.fileIndex]);
  else
    sprintf(infoPrintSummary.name,"%." STRINGIFY(SUMMARY_NAME_LEN) "s", getPrintName(infoFile.title));

  infoPrintSummary.time = infoPrinting.time;

  if (speedGetCurPercent(1) != 100)
  {
    infoPrintSummary.length = (infoPrintSummary.length * speedGetCurPercent(1)) / 100;  // multiply by flow percentage
    infoPrintSummary.weight = (infoPrintSummary.weight * speedGetCurPercent(1)) / 100;  // multiply by flow percentage
    infoPrintSummary.cost   = (infoPrintSummary.cost   * speedGetCurPercent(1)) / 100;  // multiply by flow percentage
  }
}

// send print codes [0: start gcode, 1: end gcode 2: cancel gcode]
void sendPrintCodes(uint8_t index)
{
  PRINT_GCODES printcodes;

  W25Qxx_ReadBuffer((uint8_t *)&printcodes, PRINT_GCODES_ADDR, sizeof(PRINT_GCODES));

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

void printSetUpdateWaiting(bool isWaiting)
{
  updateM27_waiting = isWaiting;
}

void updatePrintUsedFilament(void)
{
  float E_pos = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));

  if ((E_pos + MAX_RETRACT_LIMIT) < last_E_pos)  // Check whether E position reset (G92 E0)
  {
    last_E_pos = 0;
  }

  infoPrintSummary.length += (E_pos - last_E_pos) / 1000;
  last_E_pos = E_pos;
}

// only return gcode file name except path
// for example:"SD:/test/123.gcode"
// only return "123.gcode"
uint8_t * getPrintName(char * path)
{
  char * name = strrchr(path, '/');

  if (name != NULL)
    return (uint8_t *)(name + 1);
  else
    return (uint8_t *)path;
}

void clearInfoPrint(void)
{
  memset(&infoPrinting, 0, sizeof(PRINTING));
  ExitDir();
}

static inline void printRemoteStart(void)
{
  infoFile.source = BOARD_SD_REMOTE;  // avoid BOARD_SD be parsed as BOARD_SD_REMOTE in parseACK.c

  // always clean infoPrinting first and then set the needed attributes
  memset(&infoPrinting, 0, sizeof(PRINTING));

  // we assume infoPrinting is clean, so we need to set only the needed attributes
  infoPrinting.size = 1;  // .size should be different with .cur to avoid 100% progress on TFT, Get the correct value by M27
  infoPrinting.printing = true;

  if (infoMachineSettings.autoReportSDStatus == 1)
    request_M27(infoSettings.m27_refresh_time);  // check if there is a print running from onboard SD or remote host (USB)

  initPrintSummary();  // init print summary

  infoMenu.cur = 1;  // Clear menu buffer when printing menu is active by remote
  infoMenu.menu[infoMenu.cur] = menuPrinting;
}

void printStart(FIL * file, uint32_t size)
{
  // always clean infoPrinting first and then set the needed attributes
  memset(&infoPrinting, 0, sizeof(PRINTING));

  // we assume infoPrinting is clean, so we need to set only the needed attributes
  infoPrinting.size = size;
  infoPrinting.printing = true;

  switch (infoFile.source)
  {
    case BOARD_SD_REMOTE:  // present just to make the code robust. It should never been executed
    case BOARD_SD:
      //infoHost.printing = true;  // Not so fast! Let Marlin tell that he's done!

      if (infoMachineSettings.autoReportSDStatus == 1)
        request_M27(infoSettings.m27_refresh_time);  // check if there is a print running from onboard SD or remote host (USB)
      break;

    case TFT_UDISK:
    case TFT_SD:
      infoPrinting.file = *file;
      infoPrinting.cur = infoPrinting.file.fptr;

      if (infoSettings.send_start_gcode == 1 && infoPrinting.cur == 0)  // PLR continue printing, CAN NOT use start gcode
      {
        sendPrintCodes(0);
      }
      break;
  }

  initPrintSummary();  // init print summary
}

void printEnd(void)
{
  switch (infoFile.source)
  {
    case BOARD_SD_REMOTE:
    case BOARD_SD:
      if (infoMachineSettings.autoReportSDStatus == 1)
        request_M27(0);
      break;

    case TFT_UDISK:
    case TFT_SD:
      f_close(&infoPrinting.file);
      break;
  }

  powerFailedClose();
  powerFailedDelete();

  infoPrinting.cur = infoPrinting.size;  // always update the print progress to 100% even if the print was abaorted
  infoPrinting.printing = infoPrinting.pause = false;
  preparePrintSummary();  // update print summary. infoPrinting are used

  if (infoSettings.send_end_gcode == 1)
  {
    sendPrintCodes(1);
  }

  heatClearIsWaiting();
}

void printComplete(void)
{
  BUZZER_PLAY(sound_success);
  printEnd();

  if (infoSettings.auto_off)  // Auto shut down after print
  {
    shutdownStart();
  }
}

void printAbort(void)
{
  // used to avoid a possible loop in case an abort gcode (e.g. M524) is present in
  // the queue infoCmd and the function loopProcess() is invoked by this function
  static bool loopDetected = false;

  if (loopDetected) return;

  if (!infoPrinting.printing) return;

  switch (infoFile.source)
  {
    case BOARD_SD_REMOTE:
    case BOARD_SD:
      //infoHost.printing = false;  // Not so fast! Let Marlin tell that he's done!

      // Several M108 are sent to Marlin because consecutive blocking operations
      // such as heating bed, extruder may defer processing of M524
      breakAndContinue();
      breakAndContinue();
      breakAndContinue();
      breakAndContinue();

      if (infoMachineSettings.firmwareType != FW_REPRAPFW)
      {
        request_M524();
      }
      else  // if RepRap
      {
        if (!infoPrinting.pause)
        {
          request_M25();  // Must pause the print before cancel it
        }

        request_M0();  // M524 is not supportet in reprap firmware
      }

      setDialogText(LABEL_SCREEN_INFO, LABEL_BUSY, LABEL_BACKGROUND, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);

      do
      {
        loopProcess();  // NOTE: it is executed at leat one time to print the above splash screen
      }
      while (infoHost.printing == true);  // wait for the printer to settle down
      break;

    case TFT_UDISK:
    case TFT_SD:
      clearCmdQueue();
      break;
  }

  if (infoSettings.send_cancel_gcode == 1)
    sendPrintCodes(2);

  printEnd();
  clearInfoPrint();  // finally clear infoPrinting and exit from dir

  loopDetected = false;
}

bool printPause(bool isPause, PAUSE_TYPE pauseType)
{
  // used to avoid a possible loop in case a pause gcode (e.g. M25) is present in
  // the queue infoCmd and the function loopProcess() is invoked by this function
  static bool loopDetected = false;

  if (loopDetected)                  return false;
  if (!infoPrinting.printing)        return false;
  if (infoPrinting.pause == isPause) return false;

  loopDetected = true;
  infoPrinting.pause = isPause;

  switch (infoFile.source)
  {
    case BOARD_SD_REMOTE:
    case BOARD_SD:
      if (infoPrinting.pause)
        request_M25();   // pause
      else
        request_M24(0);  // resume
      break;

    case TFT_UDISK:
    case TFT_SD:
      if (infoPrinting.pause == true && pauseType == PAUSE_M0)
      {
        while (infoCmd.count != 0) {loopProcess();}
      }

      static COORDINATE tmp;
      bool isCoorRelative = coorGetRelative();
      bool isExtrudeRelative = eGetRelative();

      if (infoPrinting.pause)  // pause
      {
        // restore status before pause
        // if pause was triggered through M0/M1 then break
        if (pauseType == PAUSE_M0)
        {
          popupReminder(DIALOG_TYPE_ALERT, LABEL_PAUSE, LABEL_PAUSE);
          break;
        }
        // do not send any command if the pause originated outside TFT
        if (pauseType < PAUSE_EXTERNAL)
        {
          coordinateGetAll(&tmp);

          if (isCoorRelative == true)    mustStoreCmd("G90\n");
          if (isExtrudeRelative == true) mustStoreCmd("M82\n");

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

          if (isCoorRelative == true)    mustStoreCmd("G91\n");
          if (isExtrudeRelative == true) mustStoreCmd("M83\n");
        }
        // store pause type only on pause
        infoPrinting.pauseType = pauseType;
      }
      else  // resume
      {
        if (infoPrinting.pauseType == PAUSE_M0)
        {
          breakAndContinue();  // clear the queue and send a break and continue
          break;
        }
        // do not send any command if the pause originated outside TFT
        if (infoPrinting.pauseType < PAUSE_EXTERNAL)
        {
          if (isCoorRelative == true)    mustStoreCmd("G90\n");
          if (isExtrudeRelative == true) mustStoreCmd("M82\n");

          if (heatGetCurrentTemp(heatGetCurrentHotend()) > infoSettings.min_ext_temp)
          {
            mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len + infoSettings.resume_purge_len,
                         infoSettings.pause_feedrate[FEEDRATE_E]);
          }

          if (coordinateIsKnown())
          {
            mustStoreCmd("G1 X%.3f Y%.3f F%d\n", tmp.axis[X_AXIS], tmp.axis[Y_AXIS], infoSettings.pause_feedrate[FEEDRATE_XY]);
            mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS], infoSettings.pause_feedrate[FEEDRATE_Z]);
          }

          mustStoreCmd("G92 E%.5f\n", tmp.axis[E_AXIS]);
          mustStoreCmd("G1 F%d\n", tmp.feedrate);

          if (isCoorRelative == true)    mustStoreCmd("G91\n");
          if (isExtrudeRelative == true) mustStoreCmd("M83\n");
        }
      }

      break;
  }

  loopDetected = false;

  return true;
}

bool isPrinting(void)
{
  return infoPrinting.printing;
}

bool isPaused(void)
{
  return infoPrinting.pause;
}

void setPrintHost(bool isPrinting)
{
  // global lock info telling the printer is busy in printing
  // from onboard SD or remote host (e.g. USB)
  infoHost.printing = isPrinting;
}

void setPrintAbort(void)
{
  // always reset the flag reporting the host is printing (even if the TFT didn't intercept it yet)
  // due to no further notifications will be sent by the host to notify it is no more printing
  infoHost.printing = false;

  // if printing from onboard SD or remote host
  if (infoPrinting.printing && infoFile.source >= BOARD_SD)
  {
    if (infoMachineSettings.autoReportSDStatus == 1)
      request_M27(0);
  }

  infoPrinting.cur = infoPrinting.size;
  infoPrinting.printing = infoPrinting.pause = false;
}

void setPrintPause(bool updateHost, PAUSE_TYPE pauseType)
{
  if (infoPrinting.printing)
  {
    infoPrinting.pause = true;
    infoPrinting.pauseType = pauseType;
  }
  if (updateHost)
    infoHost.printing = false;
}

void setPrintResume(bool updateHost)
{
  // no need to check it is printing when setting the value to false
  infoPrinting.pause = false;

  if (updateHost)
  {
    // if printing from onboard SD or remote host
    if (infoPrinting.printing && infoFile.source >= BOARD_SD)
      infoHost.printing = true;
  }
}

// get gcode command from sd card
void loopPrintFromTFT(void)
{
  bool    sd_comment_mode = false;
  bool    sd_comment_space = true;
  char    sd_char;
  uint8_t sd_count = 0;
  UINT    br = 0;

  if (heatHasWaiting() || infoCmd.count || infoPrinting.pause) return;

  if (moveCacheToCmd() == true) return;

  if (!infoPrinting.printing || infoFile.source >= BOARD_SD) return;

  powerFailedCache(infoPrinting.file.fptr);

  for (; infoPrinting.cur < infoPrinting.size;)
  {
    if (f_read(&infoPrinting.file, &sd_char, 1, &br) != FR_OK) break;

    infoPrinting.cur++;

    // Gcode
    if (sd_char == '\n' )  // '\n' is end flag for per command
    {
      sd_comment_mode  = false;  // for new command
      sd_comment_space = true;

      if (sd_count != 0)
      {
        infoCmd.queue[infoCmd.index_w].gcode[sd_count++] = '\n';
        infoCmd.queue[infoCmd.index_w].gcode[sd_count] = 0;  // terminate string
        infoCmd.queue[infoCmd.index_w].src = SERIAL_PORT;
        sd_count = 0;  // clear buffer
        infoCmd.index_w = (infoCmd.index_w + 1) % CMD_MAX_LIST;
        infoCmd.count++;
        break;
      }
    }
    else if (sd_count >= CMD_MAX_CHAR - 2)
    {}  // when the command length beyond the maximum, ignore the following bytes
    else
    {
      if (sd_char == ';')  // ';' is comment out flag
      {
        sd_comment_mode = true;
      }
      else
      {
        if (sd_comment_space && (sd_char == 'G' || sd_char == 'M' || sd_char == 'T'))  // ignore ' ' space bytes
          sd_comment_space = false;

        if (!sd_comment_mode && !sd_comment_space && sd_char != '\r')  // normal gcode
          infoCmd.queue[infoCmd.index_w].gcode[sd_count++] = sd_char;
      }
    }
  }

  if (infoPrinting.printing && (infoPrinting.cur >= infoPrinting.size))  // end of .gcode file
  {
    printComplete();
  }
}

void loopPrintFromHost(void)
{
  #ifdef HAS_EMULATOR
    if (infoMenu.menu[infoMenu.cur] == menuMarlinMode) return;
  #endif

  if (infoHost.printing && !infoPrinting.printing)  // if a print starting form a remote host is intercepted
  {
    printRemoteStart();
  }

  if (infoFile.source < BOARD_SD) return;
  if (infoMachineSettings.autoReportSDStatus == ENABLED) return;
  if (!infoSettings.m27_active && !infoPrinting.printing) return;

  static uint32_t nextCheckPrintTime = 0;
  uint32_t update_M27_time = infoSettings.m27_refresh_time * 1000;

  do
  { // WAIT FOR M27
    if (updateM27_waiting == true)
    {
      nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
      break;
    }

    if (OS_GetTimeMs() < nextCheckPrintTime)
      break;

    if (storeCmd("M27\n") == false)
      break;

    nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
    updateM27_waiting = true;
  } while (0);
}
