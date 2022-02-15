#include "Printing.h"
#include "includes.h"

typedef struct
{
  FIL        file;
  uint32_t   size;                // gcode file total size
  uint32_t   cur;                 // gcode file printed size
  uint32_t   expectedTime;        // expected print duration in sec
  uint32_t   time;                // current elapsed time in sec
  uint32_t   remainingTime;       // current remaining time in sec (if set with M73 or M117)
  uint16_t   layerNumber;
  uint16_t   layerCount;
  uint8_t    prevProgress;
  uint8_t    progress;
  bool       progressFromSlicer;  // 1: progress controlled by Slicer (if set with M73)
  bool       runout;              // 1: runout in printing, 0: idle
  bool       printing;            // 1: means printing, 0: means idle
  bool       pause;               // 1: means paused
  PAUSE_TYPE pauseType;           // pause type trigged by different sources and gcodes like M0 & M600
} PRINTING;

PRINTING infoPrinting;
PRINT_SUMMARY infoPrintSummary = {.name[0] = '\0', 0, 0, 0, 0};

static bool updateM27_waiting = false;
static bool extrusionDuringPause = false;  // flag for extrusion during Print -> Pause
static float last_E_pos;
bool filamentRunoutAlarm;

void setExtrusionDuringPause(bool extruded)
{
  extrusionDuringPause = extruded;
}

bool isHostPrinting(void)
{
  return infoHost.printing;
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

void setPrintExpectedTime(uint32_t expectedTime)
{
  infoPrinting.expectedTime = expectedTime;
}

uint32_t getPrintExpectedTime(void)
{
  return infoPrinting.expectedTime;
}

void setPrintTime(uint32_t elapsedTime)
{
  if (elapsedTime % 1000 == 0)
  {
    if (infoPrinting.printing && !infoPrinting.pause)
    {
      infoPrinting.time++;

      if (infoPrinting.remainingTime > 0  && !heatHasWaiting())
        infoPrinting.remainingTime--;
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

void setPrintRemainingTime(int32_t remainingTime)
{
  float speedFactor = (float) (speedGetCurPercent(0)) / 100;  // speed (feed rate) factor (e.g. 50% -> 0.5)

  // Cura slicer put a negative value at the end instead of zero
  if (remainingTime < 0 || speedFactor <= 0.0f)
    remainingTime = 0;
  else
    remainingTime = remainingTime / speedFactor;  // remaining time = slicer remaining time / speed factor

  infoPrinting.remainingTime = remainingTime;
}

void parsePrintRemainingTime(char * buffer)
{
  int hour, min, sec;

  sscanf(buffer, "%dh%dm%ds", &hour, &min, &sec);
  setPrintRemainingTime(((int32_t) (hour) * 3600) + ((int32_t) (min) * 60) + (int32_t) (sec));
}

uint32_t getPrintRemainingTime()
{
  return infoPrinting.remainingTime;
}

void getPrintRemainingTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec)
{
  *hour = infoPrinting.remainingTime / 3600;
  *min = infoPrinting.remainingTime % 3600 / 60;
  *sec = infoPrinting.remainingTime % 60;
}

void setPrintLayerNumber(uint16_t layerNumber)
{
  infoPrinting.layerNumber = layerNumber;
}

uint16_t getPrintLayerNumber()
{
  return infoPrinting.layerNumber;
}

void setPrintLayerCount(uint16_t layerCount)
{
  infoPrinting.layerCount = layerCount;
}

uint16_t getPrintLayerCount()
{
  return infoPrinting.layerCount;
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

void setPrintProgressPercentage(uint8_t percentage)
{
  infoPrinting.progressFromSlicer = true;  // set to true to force a progress controlled by slicer
  infoPrinting.progress = percentage;
}

bool updatePrintProgress(void)
{
  uint8_t prevProgress = infoPrinting.prevProgress;

  if (!infoPrinting.progressFromSlicer)  // avoid to update progress if it is controlled by slicer
  {
    // in case not printing or a wrong size was set, we consider progress as 100%
    if (infoPrinting.size == 0)  // avoid a division for 0 (a crash) and set progress to 100%
      infoPrinting.progress = 100;
    else
      infoPrinting.progress = MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100);
  }

  if (infoPrinting.progress != prevProgress)
  {
    infoPrinting.prevProgress = infoPrinting.progress;

    return true;
  }

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
    mustStoreCmd(fanCmd[i], 0);
  }

  mustStoreCmd("M81\n");
  popupReminder(DIALOG_TYPE_INFO, LABEL_SHUT_DOWN, LABEL_SHUTTING_DOWN);
}

void shutdownLoop(void)
{
  bool tempIsLower = true;

  for (uint8_t i = NOZZLE0; i < infoSettings.hotend_count; i++)
  {
    if (heatGetCurrentTemp(i) >= infoSettings.auto_shutdown_temp)
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
  sprintf(tempstr, tempbody, infoSettings.auto_shutdown_temp);

  for (uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    mustStoreCmd(fanCmd[i], infoSettings.fan_max[i]);
  }

  setDialogText(LABEL_SHUT_DOWN, (uint8_t *)tempstr, LABEL_FORCE_SHUT_DOWN, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, shutdown, NULL, shutdownLoop);
}

void initPrintSummary(void)
{
  last_E_pos = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
  infoPrintSummary = (PRINT_SUMMARY){.name[0] = '\0', 0, 0, 0, 0};
  hasFilamentData = false;

  sprintf(infoPrintSummary.name, "%." STRINGIFY(SUMMARY_NAME_LEN) "s", getFilename(infoFile.fileIndex));  // get short or long filename
}

void preparePrintSummary(void)
{
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

void clearInfoPrint(void)
{
  memset(&infoPrinting, 0, sizeof(PRINTING));
  ExitDir();
}

void printComplete(void)
{
  infoPrinting.cur = infoPrinting.size;  // always update the print progress to 100% even if the print terminated
  infoPrinting.printing = infoPrinting.pause = false;
  setPrintRemainingTime(0);
  preparePrintSummary();  // update print summary. infoPrinting are used

  switch (infoFile.source)
  {
    case REMOTE_HOST:
      infoHost.printing = false;
      break;

    case BOARD_SD_REMOTE:
    case BOARD_SD:
      infoHost.printing = false;
      request_M27(0);
      coordinateQueryTurnOff();  // disable position auto report, if any
      break;

    case TFT_USB_DISK:
    case TFT_SD:
      f_close(&infoPrinting.file);
      powerFailedClose();   // close Power-loss Recovery file
      powerFailedDelete();  // delete Power-loss Recovery file
      break;
  }

  heatClearIsWaiting();
}

void printRemoteStart(const char * filename)
{
  infoHost.printing = true;  // always set (even if printing from onboard SD)

  if (infoPrinting.printing && infoFile.source <= BOARD_SD) return;  // if printing from TFT or onboard SD

  // always clean infoPrinting first and then set the needed attributes
  memset(&infoPrinting, 0, sizeof(PRINTING));

  // we assume infoPrinting is clean, so we need to set only the needed attributes
  infoPrinting.size = 1;  // .size must be different than .cur to avoid 100% progress on TFT
  infoPrinting.printing = true;
  initPrintSummary();  // init print summary

  if (filename != NULL)
  {
    strcpy(infoFile.title, filename);
    infoFile.source = BOARD_SD_REMOTE;
    request_M27(infoSettings.m27_refresh_time);  // use gcode M27 in case of a print running from remote onboard SD
  }
  else
  {
    strcpy(infoFile.title, "Remote printing...");
    infoFile.source = REMOTE_HOST;
  }

  infoMenu.cur = 1;  // clear menu buffer when printing menu is active by remote
  REPLACE_MENU(menuPrinting);
}

void printStart(FIL * file, uint32_t size)
{
  // always clean infoPrinting first and then set the needed attributes
  memset(&infoPrinting, 0, sizeof(PRINTING));

  // we assume infoPrinting is clean, so we need to set only the needed attributes
  infoPrinting.size = size;
  infoPrinting.printing = true;
  initPrintSummary();  // init print summary

  if (GET_BIT(infoSettings.send_gcodes, SEND_GCODES_START_PRINT))
  {
    sendPrintCodes(0);
  }

  switch (infoFile.source)
  {
    case REMOTE_HOST:      // present just to make the code robust. It should never be executed
    case BOARD_SD_REMOTE:
      return;

    case BOARD_SD:
      //infoHost.printing = true;                  // Not so fast! Let Marlin tell that he started printing!
      request_M24(0);                              // start print from onboard SD
      request_M27(infoSettings.m27_refresh_time);  // use gcode M27 in case of a print running from onboard SD
      break;

    case TFT_USB_DISK:
    case TFT_SD:
      infoPrinting.file = *file;
      infoPrinting.cur = infoPrinting.file.fptr;
      setExtrusionDuringPause(false);
      break;
  }
}

void printEnd(void)
{
  if (!infoPrinting.printing) return;

  switch (infoFile.source)
  {
    case REMOTE_HOST:  // nothing to do
    case BOARD_SD_REMOTE:
      break;

    case BOARD_SD:
    case TFT_USB_DISK:
    case TFT_SD:
      if (GET_BIT(infoSettings.send_gcodes, SEND_GCODES_END_PRINT))
      {
        sendPrintCodes(1);
      }

      break;
  }

  BUZZER_PLAY(SOUND_SUCCESS);
  printComplete();

  if (infoSettings.auto_shutdown)  // auto shutdown after print
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

  loopDetected = true;

  switch (infoFile.source)
  {
    case REMOTE_HOST:  // nothing to do
      loopDetected = false;
      return;

    case BOARD_SD_REMOTE:
    case BOARD_SD:
      //infoHost.printing = false;  // Not so fast! Let Marlin tell that he's done!

      // several M108 are sent to Marlin because consecutive blocking operations
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
          request_M25();  // must pause the print before cancel it
        }

        request_M0();  // M524 is not supportet in RepRap firmware
      }

      if (infoHost.printing)
      {
        REDRAW_MENU();
        setDialogText(LABEL_SCREEN_INFO, LABEL_BUSY, LABEL_NULL, LABEL_NULL);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);

        loopProcessToCondition(&isHostPrinting);  // wait for the printer to settle down
      }

      break;

    case TFT_USB_DISK:
    case TFT_SD:
      clearCmdQueue();
      break;
  }

  if (GET_BIT(infoSettings.send_gcodes, SEND_GCODES_CANCEL_PRINT))
  {
    sendPrintCodes(2);
  }

  printComplete();
  clearInfoPrint();  // finally clear infoPrinting and exit from dir

  loopDetected = false;
}

bool printPause(bool isPause, PAUSE_TYPE pauseType)
{
  // used to avoid a possible loop in case a pause gcode (e.g. M25) is present in
  // the queue infoCmd and the function loopProcess() is invoked by this function
  static bool loopDetected = false;

  if (loopDetected) return false;
  if (!infoPrinting.printing) return false;
  if (infoPrinting.pause == isPause) return false;

  loopDetected = true;

  switch (infoFile.source)
  {
    case REMOTE_HOST:  // nothing to do
      loopDetected = false;
      return true;

    case BOARD_SD_REMOTE:
    case BOARD_SD:
      if (isPause)
        request_M25();   // pause
      else
        request_M24(0);  // resume
      break;

    case TFT_USB_DISK:
    case TFT_SD:
      if (isPause == true && pauseType == PAUSE_M0)
        loopProcessToCondition(&isNotEmptyCmdQueue);  // wait for the communication to be clean

      static COORDINATE tmp;
      bool isCoorRelative = coorGetRelative();
      bool isExtrudeRelative = eGetRelative();

      if (isPause)  // pause
      {
        if (pauseType == PAUSE_M0)
        {
          popupReminder(DIALOG_TYPE_ALERT, LABEL_PAUSE, LABEL_PAUSE);
        }
        else if (pauseType == PAUSE_NORMAL)  // send command only if the pause originated from TFT
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
        }
        else if (pauseType == PAUSE_NORMAL)  // send command only if the pause originated from TFT
        {
          if (isCoorRelative == true)    mustStoreCmd("G90\n");
          if (isExtrudeRelative == true) mustStoreCmd("M82\n");

          if (extrusionDuringPause == true)  // check if extrusion done during Print -> Pause
          { // no purge
            extrusionDuringPause = false;
          }
          else if (heatGetCurrentTemp(heatGetCurrentHotend()) > infoSettings.min_ext_temp)
          { // purge
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

  infoPrinting.pause = isPause;  // update pause status after pause/resume procedure
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

bool isTFTPrinting(void)
{
  return (infoPrinting.printing && infoFile.source < BOARD_SD) ? true : false;
}

bool isRemoteHostPrinting(void)
{
  return (infoPrinting.printing && infoFile.source == REMOTE_HOST) ? true : false;
}

void setPrintAbort(void)
{
  BUZZER_PLAY(SOUND_ERROR);
  printComplete();
}

void setPrintPause(bool updateHost, PAUSE_TYPE pauseType)
{
  // pass value "false" for updateHost to let Marlin report (in case of printing from (remote) onboard SD)
  // when the host is not printing (when notification ack "Not SD printing" is caught).
  // In case of printing from remote host (e.g. USB) or infoSettings.m27_active set to "false", the host
  // printing status is always forced to "false" due to no other notification will be received

  if (infoPrinting.printing)
  {
    infoPrinting.pause = true;
    infoPrinting.pauseType = pauseType;
  }

  // in case of printing from remote host or infoSettings.m27_active set to "false", always force to "false"
  if (updateHost || infoFile.source == REMOTE_HOST || !infoSettings.m27_active)
    infoHost.printing = false;
}

void setPrintResume(bool updateHost)
{
  // pass value "true" for updateHost to report (in case of printing from (remote) onboard SD) the host is
  // printing without waiting from Marlin (when notification ack "SD printing byte" is caught).
  // In case of printing from remote host (e.g. USB) or infoSettings.m27_active set to "false", the host
  // printing status is always forced to "true" due to no other notification will be received

  // no need to check it is printing when setting the value to "false"
  infoPrinting.pause = false;

  // in case of printing from remote host or infoSettings.m27_active set to "false", always force to "true"
  if (updateHost || infoFile.source == REMOTE_HOST || !infoSettings.m27_active)
  {
    // if printing from (remote) onboard SD or remote host
    if (infoPrinting.printing && infoFile.source >= BOARD_SD)
      infoHost.printing = true;
  }
}

// get gcode command from TFT (SD card or USB disk)
void loopPrintFromTFT(void)
{
  if (!infoPrinting.printing) return;
  if (infoFile.source >= BOARD_SD) return;  // if not printing from TFT
  if (heatHasWaiting() || isNotEmptyCmdQueue() || infoPrinting.pause) return;
  if (moveCacheToCmd() == true) return;

  powerFailedCache(infoPrinting.file.fptr);  // update Power-loss Recovery file

  CMD      gcode;
  uint8_t  gcode_count = 0;
  uint8_t  comment_count = 0;
  char     read_char = '0';
  UINT     br = 0;
  FIL *    ip_file = &infoPrinting.file;
  uint32_t ip_cur = infoPrinting.cur;
  uint32_t ip_size = infoPrinting.size;

  for ( ; ip_cur < ip_size; ip_cur++)  // parse only the gcode (not the comment, if any)
  {
    if (f_read(ip_file, &read_char, 1, &br) != FR_OK)
    { // in case of error reading from file, force a print abort
      ip_cur = ip_size;
      continue;  // "continue" will force also to execute "ip_cur++" in the "for" statement
    }

    if (read_char == '\n' || read_char == ';')  // '\n' is command end flag, ';' is command comment flag
    {
      if (gcode_count != 0)  // if a gcode was found, finalize and enqueue the gcode and exit from loop
      {
        gcode[gcode_count++] = '\n';
        gcode[gcode_count] = 0;  // terminate string
        storeCmdFromUART(PORT_1, gcode);

        break;
      }

      if (read_char == ';')  // if a comment was found, exit from loop. Otherwise (empty line found), continue parsing the next line
        break;
    }
    else if (read_char == ' ' && gcode_count == 0)  // ignore initial ' ' space bytes
    {}
    else if (read_char != '\r')
    {
      if (gcode_count < CMD_MAX_SIZE - 2)
        gcode[gcode_count++] = read_char;
      else  // if command length is beyond the maximum, skip gcode (avoid to send a truncated gcode) and also further comment, if any
        break;
    }
  }

  if (read_char != '\n')  // continue to parse the line (e.g. comment) until command end flag
  {
    // if file comment parsing is enabled and a comment tag was previously intercepted parsing the gcode, enable comment parsing
    bool comment_parsing = (GET_BIT(infoSettings.general_settings, INDEX_FILE_COMMENT_PARSING) == 1 &&
                            read_char == ';') ? true : false;

    for ( ; ip_cur < ip_size; ip_cur++)  // continue to parse the line (e.g. comment) until command end flag
    {
      if (f_read(ip_file, &read_char, 1, &br) != FR_OK)
      { // in case of error reading from file, force a print abort
        ip_cur = ip_size;
        continue;  // "continue" will force also to execute "ip_cur++" in the "for" statement
      }

      if (read_char == '\n')  // '\n' is command end flag
      {
        if (comment_parsing && comment_count != 0)  // if a comment was found, finalize the comment data structure
        {
          gCodeCommentLine[comment_count++] = '\n';
          gCodeCommentLine[comment_count] = 0;  // terminate string
        }

        break;  // line was parsed so always exit from loop
      }
      else if (comment_parsing)
      {
        if (read_char == ';')  // ';' is command comment flag
        {
          comment_count = 0;  // there might be a comment in a commented line. We always consider the last comment
        }
        else if (read_char == ' ' && comment_count == 0)  // ignore initial ' ' space bytes
        {}
        else if (read_char != '\r')
        {
          if (comment_count < COMMENT_MAX_CHAR - 2)
            gCodeCommentLine[comment_count++] = read_char;
          else  // if comment length is beyond the maximum, skip comment but continue to parse the line until command end flag
            comment_parsing = false;
        }
      }
    }
  }

  infoPrinting.cur = ip_cur;  // update infoPrinting.cur with current file position

  if (ip_cur == ip_size)  // in case of end of gcode file, finalize the print
  {
    printEnd();
  }
  else if (ip_cur > ip_size)  // in case of print abort (ip_cur == ip_size + 1), display an error message and abort the print
  {
    BUZZER_PLAY(SOUND_ERROR);
    popupReminder(DIALOG_TYPE_ERROR, (infoFile.source == TFT_SD) ? LABEL_READ_TFTSD_ERROR : LABEL_READ_USB_DISK_ERROR, LABEL_PROCESS_ABORTED);

    printAbort();
  }
}

void loopPrintFromOnboardSD(void)
{
  #ifdef HAS_EMULATOR
    if (MENU_IS(menuMarlinMode)) return;
  #endif

  if (!infoPrinting.printing) return;
  if (infoFile.source < BOARD_SD || infoFile.source > BOARD_SD_REMOTE) return;  // if not printing from (remote) onboard SD
  if (infoMachineSettings.autoReportSDStatus == ENABLED) return;
  if (!infoSettings.m27_active) return;
  if (MENU_IS(menuTerminal)) return;

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
