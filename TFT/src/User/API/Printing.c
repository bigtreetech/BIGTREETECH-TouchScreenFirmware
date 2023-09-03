#include "Printing.h"
#include "includes.h"

typedef struct
{
  FIL        file;
  uint32_t   size;            // gcode file total size
  uint32_t   cur;             // gcode file printed size
  uint32_t   fileOffset;      // size of non-printing lines (calculated dynamically)
  uint32_t   expectedTime;    // expected print duration in sec
  uint32_t   elapsedTime;     // current elapsed time in sec
  uint32_t   remainingTime;   // current remaining time in sec
  uint16_t   layerNumber;     // current printing layer number
  uint16_t   layerCount;      // total number of layers
  PROG_FROM  progressSource;  // source for progress (file progress, time progress, progress from slicer)
  uint8_t    progress;        // printing progress in percentage (0% - 100%)
  bool       runout;          // 1: runout in printing, 0: idle
  bool       printing;        // 1: means printing, 0: means idle
  bool       paused;          // 1: means paused
  bool       aborted;         // 1: means aborted
  PAUSE_TYPE pauseType;       // pause type trigged by different sources and gcodes like M0 & M600
} PRINTING;

PRINTING infoPrinting = {0};
PRINT_SUMMARY infoPrintSummary = {.name[0] = '\0', 0, 0, 0, 0, false};

static bool updateM27Waiting = false;
static bool extrusionDuringPause = false;  // flag for extrusion during Print -> Pause
static bool filamentRunoutAlarm = false;
static float lastEPos = 0;                 // used only to update stats in infoPrintSummary

void setExtrusionDuringPause(bool extruded)
{
  extrusionDuringPause = extruded;
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

void clearQueueAndMore(void)
{
  clearCmdQueue();
  setRunoutAlarmFalse();
  heatSetUpdateWaiting(false);
  setPrintUpdateWaiting(false);
}

void breakAndContinue(void)
{
  clearQueueAndMore();
  sendEmergencyCmd("M108\n");
}

void resumeAndPurge(void)
{
  clearQueueAndMore();
  sendEmergencyCmd("M876 S0\n");
}

void resumeAndContinue(void)
{
  clearQueueAndMore();
  sendEmergencyCmd("M876 S1\n");
}

void abortAndTerminate(void)
{
  clearQueueAndMore();

  if (infoMachineSettings.firmwareType != FW_REPRAPFW)
  {
    // clear the command queue and send the M524 gcode immediately if there is an already pending gcode waiting for an ACK message.
    // Otherwise, store the gcode on command queue to send it waiting for its related ACK message
    //
    if (isPendingCmd())
      sendEmergencyCmd("M524\n");
    else
      mustStoreCmd("M524\n");
  }
  else  // if RepRap
  {
    if (!infoPrinting.paused)
      request_M25();  // must pause the print before cancelling it

    request_M0();  // M524 is not supported in RepRap firmware
  }
}

void loopBreakToCondition(CONDITION_CALLBACK condCallback)
{
  // M108 is sent to Marlin because consecutive blocking operations such as heating bed, extruder may defer processing of other gcodes.
  // If there's any ongoing blocking command, "M108" will take that out from the closed loop and a response will be received
  // from that command. Than another "M108" will be sent to unlock a next possible blocking command.
  // This way enough "M108" will be sent to unlock all possible blocking command(s) (ongoing or enqueued) but not too much and
  // not too fast one after another to overload/overrun the serial communication
  TASK_LOOP_WHILE(condCallback(), if (Serial_Available(SERIAL_PORT) != 0) sendEmergencyCmd("M108\n"));

  // remove any enqueued command that could come from a supplementary serial port or TFT media
  // (if printing from remote host or TFT media) during the loop above
  clearQueueAndMore();
}

void setPrintExpectedTime(uint32_t expectedTime)
{
  infoPrinting.expectedTime = expectedTime;
}

uint32_t getPrintExpectedTime(void)
{
  return infoPrinting.expectedTime;
}

void updatePrintTime(uint32_t osTime)
{
  if (osTime % 1000 == 0)
  {
    if (infoPrinting.printing && !infoPrinting.paused)
    {
      infoPrinting.elapsedTime++;

      if (infoPrinting.remainingTime > 0 && !heatHasWaiting())
        infoPrinting.remainingTime--;
    }
  }
}

uint32_t getPrintTime(void)
{
  return infoPrinting.elapsedTime;
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

void setPrintProgressSource(PROG_FROM progressSource)
{
  infoPrinting.progressSource = progressSource;
}

PROG_FROM getPrintProgressSource(void)
{
  return infoPrinting.progressSource;
}

uint32_t getPrintDataSize(void)
{
  return infoPrinting.size;
}

uint32_t getPrintDataCur(void)
{
  return infoPrinting.cur;
}

void setPrintProgressData(float cur, float size)
{
  infoPrinting.cur = cur;
  infoPrinting.size = size;
}

void setPrintProgressPercentage(uint8_t percentage)
{
  infoPrinting.progress = percentage;
}

uint8_t updatePrintProgress(void)
{
  switch (infoPrinting.progressSource)
  {
    case PROG_FILE:
      // in case of not printing, a wrong size was set or current position at the end of file, we consider progress as 100%
      if (infoPrinting.size <= infoPrinting.cur)
        infoPrinting.progress = 100;
      else
        infoPrinting.progress = (uint8_t)((float)(infoPrinting.cur - infoPrinting.fileOffset) / (infoPrinting.size - infoPrinting.fileOffset) * 100);

      break;

    case PROG_RRF:
    case PROG_SLICER:
      break;  // progress percentage already updated by the slicer or RRF direct percentage report ("fraction_printed")

    case PROG_TIME:
      infoPrinting.progress = ((float)infoPrinting.elapsedTime / (infoPrinting.elapsedTime + infoPrinting.remainingTime)) * 100;
      break;
  }

  return infoPrinting.progress;
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
  for (uint8_t i = NOZZLE0; i < infoSettings.hotend_count; i++)
  {
    if (heatGetCurrentTemp(i) >= infoSettings.auto_shutdown_temp)
      return;
  }

  shutdown();
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

  popupDialog(DIALOG_TYPE_INFO, LABEL_SHUT_DOWN, (uint8_t *)tempstr, LABEL_FORCE_SHUT_DOWN, LABEL_CANCEL, shutdown, NULL, shutdownLoop);
}

void initPrintSummary(void)
{
  lastEPos = coordinateGetAxis(E_AXIS);
  infoPrintSummary = (PRINT_SUMMARY){.name[0] = '\0', 0, 0, 0, 0, false};

  // save print filename (short or long filename)
  sprintf(infoPrintSummary.name, "%." STRINGIFY(SUMMARY_NAME_LEN) "s", getPrintFilename());
}

void preparePrintSummary(void)
{
  infoPrintSummary.time = infoPrinting.elapsedTime;

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

void setPrintUpdateWaiting(bool isWaiting)
{
  updateM27Waiting = isWaiting;
}

void updatePrintUsedFilament(void)
{
  float ePos = coordinateGetAxis(E_AXIS);

  if ((ePos + MAX_RETRACT_LIMIT) < lastEPos)  // check whether E position reset (G92 E0)
    lastEPos = 0;

  infoPrintSummary.length += (ePos - lastEPos) / 1000;
  lastEPos = ePos;
}

void clearInfoPrint(void)
{
  memset(&infoPrinting, 0, sizeof(PRINTING));
}

void completePrint(void)
{
  infoPrinting.cur = infoPrinting.size;  // always update the print progress to 100% even if the print terminated
  infoPrinting.progress = 100;           // set progress to 100% in case progress is controlled by slicer
  infoPrinting.printing = infoPrinting.paused = false;
  setPrintRemainingTime(0);
  preparePrintSummary();  // update print summary. infoPrinting is used

  switch (infoFile.source)
  {
    case FS_TFT_SD:
    case FS_TFT_USB:
      f_close(&infoPrinting.file);
      powerFailedClose();   // close PLR file
      powerFailedDelete();  // delete PLR file
      break;

    case FS_ONBOARD_MEDIA:
    case FS_ONBOARD_MEDIA_REMOTE:
      infoHost.status = HOST_STATUS_IDLE;
      request_M27(0);
      coordinateQueryTurnOff();  // disable position auto report, if any
      break;

    case FS_REMOTE_HOST:
      infoHost.status = HOST_STATUS_IDLE;
      break;
  }

  heatClearIsWaiting();
}

bool startPrintFromRemoteHost(const char * filename)
{
  infoHost.status = HOST_STATUS_PRINTING;  // always set first

  #ifdef HAS_EMULATOR
    if (MENU_IS(menuMarlinMode))  // do not process any printing info if Marlin Mode is active
      return false;
  #endif

  // if printing from TFT media or onboard media, exit
  if (infoPrinting.printing && infoFile.source <= FS_ONBOARD_MEDIA)
    return false;

  // always clean infoPrinting first and then set the needed attributes
  clearInfoPrint();

  // we assume infoPrinting is clean, so we need to set only the needed attributes
  infoPrinting.size = 1;  // .size must be different than .cur to avoid 100% progress on TFT
  infoPrinting.printing = true;

  // present just to make the code robust. It should never be used printing from remote onboard media
  infoFile.onboardSource = BOARD_SD;

  if (filename != NULL)
  {
    infoFile.source = FS_ONBOARD_MEDIA_REMOTE;  // set source first
    resetInfoFile();                            // then reset infoFile (source is restored)
    enterFolder(stripHead(filename));           // set path as last

    request_M27(infoSettings.m27_refresh_time);  // use gcode M27 in case of a print running from remote onboard media
  }
  else
  {
    infoFile.source = FS_REMOTE_HOST;  // set source first
    resetInfoFile();                   // then reset infoFile (source is restored)
  }

  initPrintSummary();  // init print summary as last (it requires infoFile to be properly set)

  return true;
}

bool startPrint(void)
{
  bool printRestore = false;

  // always clean infoPrinting first and then set the needed attributes
  clearInfoPrint();

  switch (infoFile.source)
  {
    case FS_TFT_SD:
    case FS_TFT_USB:
      if (f_open(&infoPrinting.file, infoFile.path, FA_OPEN_EXISTING | FA_READ) == FR_OK)
      {
        infoPrinting.size = f_size(&infoPrinting.file);

        if (infoPrinting.size == 0)
        {
          f_close(&infoPrinting.file);

          // disable print restore flag (one shot flag) for the next print.
          // The flag must always be explicitly re-enabled (e.g by powerFailedSetRestore function)
          powerFailedSetRestore(false);
          break;
        }

        infoPrinting.cur = infoPrinting.file.fptr;
        setExtrusionDuringPause(false);

        // initialize PLR info.
        // If print restore flag was enabled (e.g. by powerFailedSetRestore function called in PrintRestore.c),
        // try to load PLR info from file in order to restore the print from the failed point.
        // It finally disables print restore flag (one shot flag) for the next print.
        // The flag must always be explicitly re-enabled (e.g by powerFailedSetRestore function)
        powerFailedInitData();

        if (powerFailedCreate(infoFile.path))    // if PLR feature is enabled, open a new PLR file
        {
          printRestore = true;
          powerFailedlSeek(&infoPrinting.file);  // seek on PLR file
        }
      }

      break;

    case FS_ONBOARD_MEDIA:
      infoPrinting.size = request_M23_M36(infoFile.path);
      break;

    case FS_ONBOARD_MEDIA_REMOTE:  // present just to make the code robust. It should never be executed
    case FS_REMOTE_HOST:
      break;
  }

  if (infoPrinting.size == 0)
    return false;

  // we assume infoPrinting is clean, so we need to set only the needed attributes
  infoPrinting.printing = true;

  // execute pre print start tasks
  if (!printRestore && GET_BIT(infoSettings.send_gcodes, SEND_GCODES_START_PRINT))  // PLR continue printing, CAN NOT use start gcode
    sendPrintCodes(0);

  if (infoFile.source == FS_ONBOARD_MEDIA)
  {
    // let setPrintResume() (that will be called in parseAck.c by parsing ACK message for M24 or M27)
    // notify the print as started (infoHost.status set to "HOST_STATUS_PRINTING")
    infoHost.status = HOST_STATUS_RESUMING;

    request_M27(infoSettings.m27_refresh_time);  // use gcode M27 in case of a print running from onboard media
    request_M24(0);                              // start print from onboard media
  }

  initPrintSummary();  // init print summary as last (it requires infoFile to be properly set)

  return true;
}

void endPrint(void)
{
  // in case of printing from Marlin Mode (infoPrinting.printing set to "false"),
  // always force infoHost.status to "HOST_STATUS_IDLE"
  if (!infoPrinting.printing)
  {
    infoHost.status = HOST_STATUS_IDLE;
    return;
  }

  switch (infoFile.source)
  {
    case FS_TFT_SD:
    case FS_TFT_USB:
    case FS_ONBOARD_MEDIA:
      // execute post print end tasks
      if (GET_BIT(infoSettings.send_gcodes, SEND_GCODES_END_PRINT))
        sendPrintCodes(1);

      break;

    case FS_ONBOARD_MEDIA_REMOTE:  // nothing to do
    case FS_REMOTE_HOST:
      break;
  }

  BUZZER_PLAY(SOUND_SUCCESS);
  completePrint();

  if (infoSettings.auto_shutdown)  // auto shutdown after print
    shutdownStart();
}

void abortPrint(void)
{
  // used to avoid a possible loop in case an abort gcode (e.g. M524) is present in
  // the queue infoCmd and the function loopProcess() is invoked by this function
  static bool loopDetected = false;

  if (loopDetected) return;
  if (!infoPrinting.printing) return;

  loopDetected = true;

  clearQueueAndMore();

  switch (infoFile.source)
  {
    case FS_TFT_SD:
    case FS_TFT_USB:
      loopBreakToCondition(&isPendingCmd);  // break a pending gcode waiting for an ACK message, if any
      setPrintAbort();                      // finalize the print abort
      break;

    case FS_ONBOARD_MEDIA:
    case FS_ONBOARD_MEDIA_REMOTE:
      popupSplash(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);
      loopPopup();  // trigger the immediate draw of the above popup

      // clear the command queue and send the M524 gcode immediately if there is an already pending gcode waiting for an ACK message.
      // Otherwise, store the gcode on command queue to send it waiting for its related ACK message.
      // Furthermore, forward the print cancel action to all hosts (also TFT) to notify the print cancelation
      //
      // NOTE: the print cancel action received by the TFT always guarantees the invokation of setPrintAbort() in parseAck.c
      //       to finalize the print (e.g. stats) in case the ACK messages "Not SD printing" and/or "//action:cancel"
      //       are not received from Marlin
      //
      abortAndTerminate();
      mustStoreCmd("M118 P0 A1 action:cancel\n");

      // loop on break until infoHost.status is set to "HOST_STATUS_IDLE" by setPrintAbort() in parseAck.c
      loopBreakToCondition(&isPrintingFromOnboard);
      break;

    case FS_REMOTE_HOST:
      loopBreakToCondition(&isPendingCmd);  // break a pending gcode waiting for an ACK message, if any

      // forward a print cancel notification to all hosts (so also the one handling the print) asking to cancel the print
      mustStoreCmd("M118 P0 A1 action:notification remote cancel\n");

      loopDetected = false;  // finally, remove lock and exit
      return;
  }

  // execute post print cancel tasks
  if (GET_BIT(infoSettings.send_gcodes, SEND_GCODES_CANCEL_PRINT))
    sendPrintCodes(2);

  loopDetected = false;  // finally, remove lock and exit
}

bool pausePrint(bool isPause, PAUSE_TYPE pauseType)
{
  // used to avoid a possible loop in case a pause gcode (e.g. M25) is present in
  // the queue infoCmd and the function loopProcess() is invoked by this function
  static bool loopDetected = false;

  if (loopDetected) return false;
  if (!infoPrinting.printing) return false;
  if (infoPrinting.paused == isPause) return false;

  loopDetected = true;

  switch (infoFile.source)
  {
    case FS_TFT_SD:
    case FS_TFT_USB:
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
        else if (pauseType == PAUSE_NORMAL)  // send command only for pause originated from TFT
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
        else if (pauseType == PAUSE_NORMAL)  // send command only for pause originated from TFT
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

    case FS_ONBOARD_MEDIA:
    case FS_ONBOARD_MEDIA_REMOTE:
      if (isPause)
        request_M25();   // pause
      else
        request_M24(0);  // resume

      break;

    case FS_REMOTE_HOST:
      if (isPause)  // forward a print pause notification to all hosts (so also the one handling the print) asking to pause the print
        mustStoreCmd("M118 P0 A1 action:notification remote pause\n");
      else          // forward a print resume notification to all hosts (so also the one handling the print) asking to resume the print
        mustStoreCmd("M118 P0 A1 action:notification remote resume\n");

      loopDetected = false;  // finally, remove lock and exit
      return true;
  }

  infoPrinting.paused = isPause;  // update pause status after pause/resume procedure

  loopDetected = false;  // finally, remove lock and exit
  return true;
}

bool isPrinting(void)
{
  return infoPrinting.printing;
}

bool isPaused(void)
{
  return infoPrinting.paused;
}

bool isAborted(void)
{
  return infoPrinting.aborted;
}

bool isPrintingFromTFT(void)
{
  return (infoPrinting.printing && infoFile.source < FS_ONBOARD_MEDIA) ? true : false;
}

bool isPrintingFromOnboard(void)
{
  return (infoHost.status != HOST_STATUS_IDLE);
}

bool isPrintingFromRemoteHost(void)
{
  return (infoPrinting.printing && infoFile.source == FS_REMOTE_HOST) ? true : false;
}

void setPrintAbort(void)
{
  // in case of printing from Marlin Mode (infoPrinting.printing set to "false"),
  // always force infoHost.status to "HOST_STATUS_IDLE"
  if (!infoPrinting.printing)
  {
    infoHost.status = HOST_STATUS_IDLE;
    return;
  }

  infoPrinting.aborted = true;

  BUZZER_PLAY(SOUND_ERROR);
  completePrint();
}

void setPrintPause(HOST_STATUS hostStatus, PAUSE_TYPE pauseType)
{
  if (infoPrinting.printing)
  {
    infoPrinting.paused = true;
    infoPrinting.pauseType = pauseType;
  }

  // in case of printing from Marlin Mode (infoPrinting.printing set to "false") or printing from remote host
  // (e.g. OctoPrint) or infoSettings.m27_active set to "false", infoHost.status is always forced to
  // "HOST_STATUS_PAUSED" because no other notification will be received
  if (!infoPrinting.printing || (infoFile.source == FS_REMOTE_HOST || !infoSettings.m27_active))
    infoHost.status = HOST_STATUS_PAUSED;
  else
    infoHost.status = hostStatus;  // if printing from (remote) onboard media
}

void setPrintResume(HOST_STATUS hostStatus)
{
  // no need to check it is printing when setting the value to "false"
  infoPrinting.paused = false;

  // in case of printing from Marlin Mode (infoPrinting.printing set to "false") or printing from remote host
  // (e.g. OctoPrint) or infoSettings.m27_active set to "false", infoHost.status is always forced to
  // "HOST_STATUS_PRINTING" because no other notification will be received
  if (!infoPrinting.printing || (infoFile.source == FS_REMOTE_HOST || !infoSettings.m27_active))
    infoHost.status = HOST_STATUS_PRINTING;
  else
    infoHost.status = hostStatus;  // if printing from (remote) onboard media
}

// get gcode command from TFT media (e.g. TFT SD card or TFT USB disk)
void loopPrintFromTFT(void)
{
  if (!infoPrinting.printing) return;
  if (infoFile.source >= FS_ONBOARD_MEDIA) return;  // if not printing from TFT media
  if (heatHasWaiting() || isNotEmptyCmdQueue() || infoPrinting.paused) return;
  if (moveCacheToCmd() == true) return;

  powerFailedCache(infoPrinting.file.fptr);  // update Power-loss Recovery file

  CMD      gcode;
  uint8_t  gcode_count = 0;
  uint8_t  comment_count = 0;
  char     read_char = '\0';
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
        gcode[gcode_count] = '\0';  // terminate string
        storeCmdFromUART(gcode, PORT_1);

        break;
      }

      if (read_char == ';')  // if a comment was found then exit from loop, otherwise (empty line found) continue parsing the next line
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
          gCodeCommentLine[comment_count] = '\0';  // terminate string
        }

        break;  // line was parsed so always exit from loop
      }
      else if (comment_parsing)
      {
        if (read_char == ';')  // ';' is command comment flag
        {
          comment_count = 0;  // there might be a comment in a commented line, always consider the last comment
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

  if (gcode_count == 0)
    infoPrinting.fileOffset += ip_cur - infoPrinting.cur;

  infoPrinting.cur = ip_cur;  // update infoPrinting.cur with current file position

  if (ip_cur == ip_size)  // in case of end of gcode file, finalize the print
  {
    endPrint();
  }
  else if (ip_cur > ip_size)  // in case of print abort (ip_cur == ip_size + 1), display an error message and abort the print
  {
    BUZZER_PLAY(SOUND_ERROR);
    popupReminder(DIALOG_TYPE_ERROR, (infoFile.source == FS_TFT_SD) ? LABEL_TFT_SD_READ_ERROR : LABEL_TFT_USB_READ_ERROR, LABEL_PROCESS_ABORTED);

    abortPrint();
  }
}

void loopPrintFromOnboard(void)
{
  #ifdef HAS_EMULATOR
    if (MENU_IS(menuMarlinMode)) return;
  #endif

  if (!infoPrinting.printing) return;
  if (infoFile.source < FS_ONBOARD_MEDIA || infoFile.source > FS_ONBOARD_MEDIA_REMOTE) return;  // if not printing from (remote) onboard media
  if (infoMachineSettings.autoReportSDStatus == ENABLED) return;
  if (!infoSettings.m27_active) return;
  if (MENU_IS(menuTerminal)) return;

  static uint32_t nextCheckPrintTime = 0;
  uint32_t update_M27_time = SEC_TO_MS(infoSettings.m27_refresh_time);

  do
  { // WAIT FOR M27
    if (updateM27Waiting == true)
    {
      nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
      break;
    }

    if (OS_GetTimeMs() < nextCheckPrintTime)
      break;

    if (storeCmd("M27\n") == false)
      break;

    nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
    updateM27Waiting = true;
  } while (0);
}
