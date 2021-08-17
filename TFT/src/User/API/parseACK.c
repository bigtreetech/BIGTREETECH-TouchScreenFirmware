#include "parseACK.h"
#include "includes.h"
#include "parseACKJson.hpp"

#define ACK_MAX_SIZE 512

char dmaL2Cache[ACK_MAX_SIZE];
static uint16_t ack_index = 0;
static uint8_t ack_cur_src = SERIAL_PORT;

static const char errormagic[] = "Error:";
static const char echomagic[] = "echo:";
static const char warningmagic[] = "Warning:";                     // RRF warning
static const char messagemagic[] = "message";                      // RRF message in Json format
static const char errorZProbe[] = "ZProbe triggered before move";  // smoothieware message

bool hostDialog = false;

struct HOST_ACTION
{
  char prompt_begin[30];
  char prompt_button1[20];
  char prompt_button2[20];
  bool prompt_show;         // Show popup reminder or not
  uint8_t button;           // Number of buttons
} hostAction;

typedef enum  // popup message types available to display an echo message
{
  ECHO_NOTIFY_NONE = 0,  // ignore the echo message
  ECHO_NOTIFY_TOAST,     // Show a non invasive toast on the title bar for a preset duration.
  ECHO_NOTIFY_DIALOG,    // Show a window to notify the user and alow interaction.
} ECHO_NOTIFY_TYPE;

typedef struct
{
  ECHO_NOTIFY_TYPE   notifyType;
  const char * const msg;
} ECHO;

// notify or ignore messages starting with following text
const ECHO knownEcho[] = {
  {ECHO_NOTIFY_NONE, "busy: paused for user"},
  {ECHO_NOTIFY_NONE, "busy: processing"},
  {ECHO_NOTIFY_NONE, "Now fresh file:"},
  {ECHO_NOTIFY_NONE, "Now doing file:"},
  //{ECHO_NOTIFY_NONE, "Probe Offset"},
  //{ECHO_NOTIFY_NONE, "enqueueing \"M117\""},
  {ECHO_NOTIFY_NONE, "Flow:"},
  {ECHO_NOTIFY_NONE, "echo:;"},                   // M503
  {ECHO_NOTIFY_NONE, "echo:  G"},                 // M503
  {ECHO_NOTIFY_NONE, "echo:  M"},                 // M503
  {ECHO_NOTIFY_NONE, "Cap:"},                     // M115
  {ECHO_NOTIFY_NONE, "Config:"},                  // M360
  {ECHO_NOTIFY_TOAST, "Settings Stored"},         // M500
  {ECHO_NOTIFY_TOAST, "echo:Bed"},                // M420
  {ECHO_NOTIFY_TOAST, "echo:Fade"},               // M420
  {ECHO_NOTIFY_TOAST, "echo:Active Extruder"},    // Tool Change
  {ECHO_NOTIFY_NONE, "Unknown command: \"M150"},  // M150
};

//uint8_t forceIgnore[ECHO_ID_COUNT] = {0};

//void setIgnoreEcho(ECHO_ID msgId, bool state)
//{
//  forceIgnore[msgId] = state;
//}

void setCurrentAckSrc(uint8_t src)
{
  ack_cur_src = src;
}

static bool ack_seen(const char * str)
{
  uint16_t i;
  for (ack_index = 0; ack_index < ACK_MAX_SIZE && dmaL2Cache[ack_index] != 0; ack_index++)
  {
    for (i = 0; str[i] != 0 && dmaL2Cache[ack_index + i] != 0 && dmaL2Cache[ack_index + i] == str[i]; i++)
    {}
    if (str[i] == 0)
    {
      ack_index += i;
      return true;
    }
  }
  return false;
}

static bool ack_continue_seen(const char * str)
{ // unlike "ack_seen()", this retains "ack_index" if the searched string is not found
  uint16_t i;
  uint16_t indexBackup = ack_index;

  for (; ack_index < ACK_MAX_SIZE && dmaL2Cache[ack_index] != 0; ack_index++)
  {
    for (i = 0; str[i] != 0 && dmaL2Cache[ack_index + i] != 0 && dmaL2Cache[ack_index + i] == str[i]; i++)
    {}
    if (str[i] == 0)
    {
      ack_index += i;
      return true;
    }
  }
  ack_index = indexBackup;
  return false;
}

static bool ack_cmp(const char * str)
{
  uint16_t i;
  for (i = 0; i < ACK_MAX_SIZE && str[i] != 0 && dmaL2Cache[i] != 0; i++)
  {
    if (str[i] != dmaL2Cache[i])
      return false;
  }
  if (dmaL2Cache[i] != 0)
    return false;
  return true;
}

static float ack_value()
{
  return (strtod(&dmaL2Cache[ack_index], NULL));
}

// Read the value after the / if exists
static float ack_second_value()
{
  char * secondValue = strchr(&dmaL2Cache[ack_index], '/');
  if (secondValue != NULL)
  {
    return (strtod(secondValue + 1, NULL));
  }
  else
  {
    return -0.5;
  }
}

void ack_values_sum(float *data)
{
  while (((dmaL2Cache[ack_index] < '0') || (dmaL2Cache[ack_index] > '9')) && dmaL2Cache[ack_index] != '\n')
    ack_index++;
  *data += ack_value();
  while ((((dmaL2Cache[ack_index] >= '0') && (dmaL2Cache[ack_index] <= '9')) ||
          (dmaL2Cache[ack_index] == '.')) && (dmaL2Cache[ack_index] != '\n'))
    ack_index++;
  if (dmaL2Cache[ack_index] != '\n')
    ack_values_sum(data);
}

void ackPopupInfo(const char * info)
{
  bool show_dialog = true;
  if (infoMenu.menu[infoMenu.cur] == menuTerminal ||
      (infoMenu.menu[infoMenu.cur] == menuStatus && info == echomagic))
    show_dialog = false;

  // play notification sound if buzzer for ACK is enabled
  if (info == errormagic)
    BUZZER_PLAY(sound_error);
  else if (info == echomagic && infoSettings.ack_notification == 1)
    BUZZER_PLAY(sound_notify);

  // set echo message in status screen
  if (info == echomagic || info == messagemagic)
  {
    // ignore all messages if parameter settings is open
    if (infoMenu.menu[infoMenu.cur] == menuParameterSettings)
      return;

    // show notification based on notificaiton settings
    if (infoSettings.ack_notification == 1)
    {
      addNotification(DIALOG_TYPE_INFO, (char *)info, (char *)dmaL2Cache + ack_index, show_dialog);
    }
    else if (infoSettings.ack_notification == 2)
    {
      addToast(DIALOG_TYPE_INFO, dmaL2Cache);  // show toast notificaion if turned on
    }
  }
  else
  {
    addNotification(DIALOG_TYPE_ERROR, (char *)info, (char *)dmaL2Cache + ack_index, show_dialog);
  }
}

bool processKnownEcho(void)
{
  bool isKnown = false;
  uint8_t i;

  for (i = 0; i < COUNT(knownEcho); i++)
  {
    if (strstr(dmaL2Cache, knownEcho[i].msg))
    {
      isKnown = true;
      break;
    }
  }

  // display the busy indicator
  busyIndicator(STATUS_BUSY);

  if (isKnown)
  {
    if (knownEcho[i].notifyType == ECHO_NOTIFY_NONE)
      return isKnown;
    //if (forceIgnore[i] == 0)
    //{
      if (knownEcho[i].notifyType == ECHO_NOTIFY_TOAST)
      {
        addToast(DIALOG_TYPE_INFO, dmaL2Cache);
      }
      else if (knownEcho[i].notifyType == ECHO_NOTIFY_DIALOG)
      {
        BUZZER_PLAY(sound_notify);
        addNotification(DIALOG_TYPE_INFO, (char *)echomagic, (char *)dmaL2Cache + ack_index, true);
      }
    //}
  }
  return isKnown;
}

bool dmaL1NotEmpty(uint8_t port)
{
  return dmaL1Data[port].rIndex != dmaL1Data[port].wIndex;
}

void syncL2CacheFromL1(uint8_t port)
{
  uint16_t i = 0;

  while (dmaL1NotEmpty(port))
  {
    dmaL2Cache[i] = dmaL1Data[port].cache[dmaL1Data[port].rIndex];
    dmaL1Data[port].rIndex = (dmaL1Data[port].rIndex + 1) % dmaL1Data[port].cacheSize;
    if (dmaL2Cache[i++] == '\n') break;
  }
  dmaL2Cache[i] = 0;  // End character
}

void hostActionCommands(void)
{
  char * find = strchr(dmaL2Cache + ack_index, '\n');
  *find = '\0';

  if (ack_seen(":notification "))
  {
    uint16_t index = ack_index;  // save the current index for further usage

    if (ack_seen("Time Left"))
    {
      parsePrintRemainingTime((char *)dmaL2Cache + ack_index);
    }
    else
    {
      statusScreen_setMsg((uint8_t *)echomagic, (uint8_t *)dmaL2Cache + index);  // always display the notification on status screen

      if (!ack_seen("Ready."))  // avoid to display unneeded/frequent useless notifications (e.g. "My printer Ready.")
      {
        if (infoMenu.menu[infoMenu.cur] != menuStatus)  // don't show it when in menuStatus
          addToast(DIALOG_TYPE_INFO, dmaL2Cache + index);

        if (infoSettings.notification_m117 == ENABLED)
          addNotification(DIALOG_TYPE_INFO, (char *)echomagic, (char *)dmaL2Cache + index, false);
      }
    }
  }
  else if (ack_seen(":paused") || ack_seen(":pause"))
  {
    if (infoMachineSettings.firmwareType == FW_MARLIN)
    {
      if (ack_seen(":paused"))  // if paused with ADVANCED_PAUSE_FEATURE enabled in Marlin (:paused),
        hostDialog = true;      // disable Resume/Pause button in the Printing menu
      //else                      // otherwise, if ADVANCED_PAUSE_FEATURE is disabled in Marlin (:pause),
      //  hostDialog = false;     // enable Resume/Pause button in the Printing menu
    }

    // pass value "false" to let Marlin report when the host is not
    // printing (when notification ack "Not SD printing" is caught)
    setPrintPause(false, PAUSE_EXTERNAL);

    if (ack_seen("filament_runout"))
    {
      setRunoutAlarmTrue();
    }
  }
  else if (ack_seen(":resumed") || ack_seen(":resume"))
  {
    hostDialog = false;  // enable Resume/Pause button in the Printing menu

    // pass value "true" to report the host is printing without waiting
    // from Marlin (when notification ack "SD printing byte" is caught)
    setPrintResume(true);
  }
  else if (ack_seen(":cancel"))  // To be added to Marlin abortprint routine
  {
    setPrintAbort();
  }
  else if (ack_seen(":prompt_begin "))
  {
    strcpy(hostAction.prompt_begin, dmaL2Cache + ack_index);
    hostAction.button = 0;
    hostAction.prompt_show = true;

    if (ack_seen("Nozzle Parked"))
    {
      // pass value "false" to let Marlin report when the host is not
      // printing (when notification ack "Not SD printing" is caught)
      setPrintPause(false, PAUSE_EXTERNAL);
    }
    else if (ack_seen("Resuming"))  // resuming from onboard SD or TFT
    {
      // pass value "true" to report the host is printing without waiting
      // from Marlin (when notification ack "SD printing byte" is caught)
      setPrintResume(true);

      if (infoMachineSettings.firmwareType != FW_REPRAPFW)
      {
        hostAction.prompt_show = false;
        Serial_Puts(SERIAL_PORT, "M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
      }
    }
    else if (ack_seen("Reheating"))
    {
      hostAction.prompt_show = false;
      Serial_Puts(SERIAL_PORT, "M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
  }
  else if (ack_seen(":prompt_button "))
  {
    hostAction.button++;
    if (hostAction.button == 1)
    {
      strcpy(hostAction.prompt_button1, dmaL2Cache + ack_index);
    }
    else
    {
      strcpy(hostAction.prompt_button2, dmaL2Cache + ack_index);
    }
  }
  else if (ack_seen(":prompt_show") && hostAction.prompt_show)
  {
    switch (hostAction.button)
    {
      case 0:
        BUZZER_PLAY(sound_notify);
        setDialogText((uint8_t *)"Message", (uint8_t *)hostAction.prompt_begin, LABEL_CONFIRM,
                      LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ALERT, setRunoutAlarmFalse, NULL, NULL);
        break;

      case 1:
        BUZZER_PLAY(sound_notify);
        setDialogText((uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin, (uint8_t *)hostAction.prompt_button1,
                      LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ALERT, breakAndContinue, NULL, NULL);
        break;

      case 2:
        BUZZER_PLAY(sound_notify);
        setDialogText((uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin, (uint8_t *)hostAction.prompt_button1,
                      (uint8_t *)hostAction.prompt_button2);
        showDialog(DIALOG_TYPE_ALERT, resumeAndPurge, resumeAndContinue, NULL);
        break;
    }
  }
}

void parseACK(void)
{
  if (infoHost.rx_ok[SERIAL_PORT] != true) return;  // not get response data

  while (dmaL1NotEmpty(SERIAL_PORT))
  {
    bool avoid_terminal = false;
    syncL2CacheFromL1(SERIAL_PORT);
    infoHost.rx_ok[SERIAL_PORT] = false;

    #if defined(SERIAL_DEBUG_PORT) && defined(DEBUG_SERIAL_COMM)
      // dump raw serial data received to debug port
      Serial_Puts(SERIAL_DEBUG_PORT, "<<");
      Serial_Puts(SERIAL_DEBUG_PORT, dmaL2Cache);
    #endif

    if (infoHost.connected == false)  // Not connected to printer
    {
      // parse error information even though not connected to printer
      if (ack_seen(errormagic)) ackPopupInfo(errormagic);

      // the first response should be such as "T:25/50\n"
      if (!(ack_seen("@") && ack_seen("T:")) && !ack_seen("T0:")) goto parse_end;

      // find hotend count and setup heaters
      uint8_t i;
      for (i = NOZZLE0; i < MAX_HOTEND_COUNT; i++)
      {
        if (!ack_seen(heaterID[i])) break;
      }
      infoSettings.hotend_count = i ? i : 1;
      if (infoSettings.ext_count < infoSettings.hotend_count) infoSettings.ext_count = infoSettings.hotend_count;
      if (ack_seen(heaterID[BED])) infoSettings.bed_en = ENABLED;
      if (ack_seen(heaterID[CHAMBER])) infoSettings.chamber_en = ENABLED;
      updateNextHeatCheckTime();

      if (!ack_seen("@"))  // It's RepRapFirmware
      {
        infoMachineSettings.firmwareType = FW_REPRAPFW;
        infoMachineSettings.softwareEndstops = ENABLED;
        infoHost.wait = false;
        storeCmd("M92\n");
        storeCmd("M115\n");
      }

      if (infoMachineSettings.firmwareType == FW_NOT_DETECTED)  // if never connected to the printer since boot
      {
        storeCmd("M503\n");  // Query detailed printer capabilities
        storeCmd("M92\n");   // Steps/mm of extruder is an important parameter for Smart filament runout
                             // Avoid can't getting this parameter due to disabled M503 in Marlin
        storeCmd("M115\n");
        storeCmd("M211\n");  // retrieve the software endstops state
      }
      infoHost.connected = true;
      requestCommandInfo.inJson = false;
    }

    // Onboard sd Gcode command response

    if (requestCommandInfo.inWaitResponse)
    {
      if (ack_seen(requestCommandInfo.startMagic))
      {
        requestCommandInfo.inResponse = true;
        requestCommandInfo.inWaitResponse = false;
      }
      else if ((requestCommandInfo.error_num > 0 && ack_seen(requestCommandInfo.errorMagic[0]))
            || (requestCommandInfo.error_num > 1 && ack_seen(requestCommandInfo.errorMagic[1]))
            || (requestCommandInfo.error_num > 2 && ack_seen(requestCommandInfo.errorMagic[2])))
      { // parse onboard sd error
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        requestCommandInfo.inError = true;
        requestCommandInfo.inWaitResponse = false;

        strcpy(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
        BUZZER_PLAY(sound_error);
        goto parse_end;
      }
      requestCommandInfo.inJson = false;
    }

    if (requestCommandInfo.inResponse)
    {
      if (strlen(requestCommandInfo.cmd_rev_buf) + strlen(dmaL2Cache) < CMD_MAX_REV)
      {
        strcat(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
        if (ack_seen(requestCommandInfo.stopMagic))
        {
          requestCommandInfo.done = true;
          requestCommandInfo.inResponse = false;
        }
      }
      else
      {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        ackPopupInfo(errormagic);
      }
      infoHost.wait = false;
      requestCommandInfo.inJson = false;
      goto parse_end;
    }
    // Onboard sd Gcode command response end

    if (!requestCommandInfo.inWaitResponse && !requestCommandInfo.inResponse && infoMachineSettings.firmwareType == FW_REPRAPFW)
    {
      if (strchr(dmaL2Cache, '{') != NULL)
      {
        requestCommandInfo.inJson = true;
      }
    }

    if (requestCommandInfo.inJson)
    {
      parseACKJson(dmaL2Cache);
    }

    if (ack_cmp("ok\n"))
    {
      infoHost.wait = false;
    }
    else
    {
      if (ack_seen("ok"))
        infoHost.wait = false;

      //----------------------------------------
      // Pushed / polled / on printing parsed responses
      //----------------------------------------

      // parse and store temperatures
      if ((ack_seen("@") && ack_seen("T:")) || ack_seen("T0:"))
      {
        heatSetCurrentTemp(NOZZLE0, ack_value() + 0.5f);
        if (!heatGetSendWaiting(NOZZLE0))
          heatSyncTargetTemp(NOZZLE0, ack_second_value() + 0.5f);

        for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
        {
          if (!heaterDisplayIsValid(i))
            continue;
          if (ack_seen(heaterID[i]))
          {
            heatSetCurrentTemp(i, ack_value() + 0.5f);
            if (!heatGetSendWaiting(i))
              heatSyncTargetTemp(i, ack_second_value() + 0.5f);
          }
        }
        avoid_terminal = !infoSettings.terminalACK;
        updateNextHeatCheckTime();
      }
      // parse and store M114, current position
      else if ((ack_seen("X:") && ack_index == 2) || ack_seen("C: X:"))  // Smoothieware axis position starts with "C: X:"
      {
        coordinateSetAxisActual(X_AXIS, ack_value());
        if (ack_seen("Y:"))
        {
          coordinateSetAxisActual(Y_AXIS, ack_value());
          if (ack_seen("Z:"))
          {
            coordinateSetAxisActual(Z_AXIS, ack_value());
            if (ack_seen("E:"))
            {
              coordinateSetAxisActual(E_AXIS, ack_value());
            }
          }
        }
        coordinateQuerySetWait(false);
      }
      // parse and store M114 E, extruder position. Required "M114_DETAIL" in Marlin
      else if (ack_seen("Count E:"))
      {
        coordinateSetExtruderActualSteps(ack_value());
      }
      // parse and store feed rate percentage
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW && ack_seen("Speed factor: ")) ||
               ack_seen("FR:"))
      {
        speedSetCurPercent(0, ack_value());
        speedQuerySetWait(false);
      }
      // parse and store flow rate percentage in case of RepRapFirmware
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && ack_seen("Extrusion factor"))
      {
        if (ack_continue_seen(": "))
        {
          speedSetCurPercent(1, ack_value());
          speedQuerySetWait(false);
        }
      }
      // parse and store flow rate percentage
      else if (ack_seen("Flow: "))
      {
        speedSetCurPercent(1, ack_value());
        speedQuerySetWait(false);
      }
      // parse and store feed rate percentage in case of Smoothieware
      else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("Speed factor at "))
      {
        speedSetCurPercent(0, ack_value());
        speedQuerySetWait(false);
      }
      // parse and store flow rate percentage in case of Smoothieware
      else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("Flow rate at "))
      {
        speedSetCurPercent(1, ack_value());
        speedQuerySetWait(false);
      }
      // parse and store M106, fan speed
      else if (ack_seen("M106 P"))
      {
        uint8_t i = ack_value();
        if (ack_seen("S"))
        {
          fanSetCurSpeed(i, ack_value());
        }
      }
      // parse and store M710, controller fan
      else if (ack_seen("M710"))
      {
        if (ack_seen("S")) fanSetCurSpeed(MAX_COOLING_FAN_COUNT, ack_value());
        if (ack_seen("I")) fanSetCurSpeed(MAX_COOLING_FAN_COUNT + 1, ack_value());
        ctrlFanQuerySetWait(false);
      }
      // parse pause message
      else if (!infoMachineSettings.promptSupport && ack_seen("paused for user"))
      {
        setDialogText((uint8_t*)"Printer is Paused", (uint8_t*)"Paused for user\ncontinue?", LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_QUESTION, breakAndContinue, NULL, NULL);
      }
      // parse host action commands. Required "HOST_ACTION_COMMANDS" and other settings in Marlin
      else if (ack_seen("//action:"))
      {
        hostActionCommands();
      }
      // parse and store M118, filament data update
      else if (ack_seen("filament_data"))
      {
        if (ack_seen("L:"))
        {
          ack_values_sum(&infoPrintSummary.length);
        }
        else if (ack_seen("W:"))
        {
          ack_values_sum(&infoPrintSummary.weight);
        }
        else if (ack_seen("C:"))
        {
          ack_values_sum(&infoPrintSummary.cost);
        }
        hasFilamentData = true;
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED &&
               ack_seen(infoMachineSettings.firmwareType != FW_REPRAPFW ? "File opened:" : "job.file.fileName"))
      {
        char * fileEndString;
        if (infoMachineSettings.firmwareType != FW_REPRAPFW)
        {
          // Marlin
          fileEndString = " Size:";  // File opened: 1A29A~1.GCO Size: 6974
        }
        else
        {
          // RRF
          ack_seen("result\":\"0:/gcodes/");  // {"key":"job.file.fileName","flags": "","result":"0:/gcodes/pig-4H.gcode"}
          fileEndString = "\"";
        }

        uint16_t start_index = ack_index;
        uint16_t end_index = ack_continue_seen(fileEndString) ? (ack_index - strlen(fileEndString)) : start_index;
        uint16_t path_len = MIN(end_index - start_index, MAX_PATH_LEN - strlen(getCurFileSource()) - 1);
        sprintf(infoFile.title,"%s/", getCurFileSource());
        strncat(infoFile.title, dmaL2Cache + start_index, path_len);
        infoFile.title[path_len + strlen(getCurFileSource()) + 1] = '\0';

        setPrintHost(true);
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED &&
               infoFile.source >= BOARD_SD &&
               ack_seen("Not SD printing"))
      {
        setPrintPause(true, PAUSE_EXTERNAL);
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED &&
               infoFile.source >= BOARD_SD &&
               ack_seen("SD printing byte"))
      {
        if (infoMachineSettings.firmwareType != FW_REPRAPFW)
          setPrintResume(false);

        // Parsing printing data
        // Example: SD printing byte 123/12345
        setPrintProgress(ack_value(), ack_second_value());
        //powerFailedCache(position);
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED &&
               infoFile.source >= BOARD_SD &&
               ack_seen(infoMachineSettings.firmwareType != FW_REPRAPFW ? "Done printing file" : "Finished printing file"))
      {
        setPrintHost(false);
        printComplete();
      }

      //----------------------------------------
      // Tuning parsed responses
      //----------------------------------------

      // parse and store build volume size
      else if (ack_seen("work:"))
      {
        if (ack_seen("min:"))
        {
          if (ack_seen("X:")) infoSettings.machine_size_min[X_AXIS] = ack_value();
          if (ack_seen("Y:")) infoSettings.machine_size_min[Y_AXIS] = ack_value();
          if (ack_seen("Z:")) infoSettings.machine_size_min[Z_AXIS] = ack_value();
        }
        if (ack_seen("max:"))
        {
          if (ack_seen("X:")) infoSettings.machine_size_max[X_AXIS] = ack_value();
          if (ack_seen("Y:")) infoSettings.machine_size_max[Y_AXIS] = ack_value();
          if (ack_seen("Z:")) infoSettings.machine_size_max[Z_AXIS] = ack_value();
        }
      }
      // parse M48, Repeatability Test
      else if (ack_seen("Mean:"))
      {
        char tmpMsg[100];
        strcpy (tmpMsg, "Mean: ");
        sprintf (&tmpMsg[strlen(tmpMsg)], "%0.5f", ack_value());
        if (ack_seen("Min: "))
        {
          sprintf (&tmpMsg[strlen(tmpMsg)], "\nMin: %0.5f", ack_value());
        }
        if (ack_seen("Max: "))
        {
          sprintf (&tmpMsg[strlen(tmpMsg)], "\nMax: %0.5f", ack_value());
        }
        if (ack_seen("Range: "))
        {
          sprintf (&tmpMsg[strlen(tmpMsg)], "\nRange: %0.5f", ack_value());
        }
        setDialogText((uint8_t* )"Repeatability Test", (uint8_t *)tmpMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      }
      // parse M48, Standard Deviation
      else if (ack_seen("Standard Deviation: "))
      {
        char tmpMsg[100];
        strncpy(tmpMsg, (char *)getDialogMsgStr(), 6);
        tmpMsg[6] = '\0';
        if (strcmp(tmpMsg, "Mean: ") == 0)
        {
          SetLevelCornerPosition(4, ack_value());  // save value for Lever Corner display
          sprintf(tmpMsg, "%s\nStandard Deviation: %0.5f", (char *)getDialogMsgStr(), ack_value());
          setDialogText((uint8_t* )"Repeatability Test", (uint8_t *)tmpMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
          showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
        }
      }
      // parse and store M211 or M503, software endstops state (e.g. from Probe Offset, MBL, Mesh Editor menus)
      else if (ack_seen("Soft endstops"))
      {
        uint8_t curValue = infoMachineSettings.softwareEndstops;
        infoMachineSettings.softwareEndstops = ack_seen("ON");

        if (curValue != infoMachineSettings.softwareEndstops)  // send a notification only if status is changed
          addToast(DIALOG_TYPE_INFO, dmaL2Cache);
      }
      // parse M303, PID Autotune finished message
      else if (ack_seen("PID Autotune finished"))
      {
        pidUpdateStatus(true);
      }
      // parse M303, PID Autotune failed message
      else if (ack_seen("PID Autotune failed"))
      {
        pidUpdateStatus(false);
      }
      // parse M303, PID Autotune finished message in case of Smoothieware
      else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("PID Autotune Complete!"))
      {
        //ack_index += 84; -> need length check
        pidUpdateStatus(true);
      }
      // parse M303, PID Autotune failed message in case of Smoothieware
      else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("// WARNING: Autopid did not resolve within"))
      {
        pidUpdateStatus(false);
      }
      // parse M303, PID Autotune completed message in case of RRF
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && ack_seen("Auto tuning heater") && ack_seen("completed"))
      {
        pidUpdateStatus(true);
      }
      // parse M303, PID Autotune failed message in case of RRF
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && (ack_seen("Error: M303") || (ack_seen("Auto tune of heater") && ack_seen("failed"))))
      {
        pidUpdateStatus(false);
      }
      // parse and store M355, Case light message
      else if (ack_seen("Case light:"))
      {
        if (ack_continue_seen("OFF"))
        {
          caseLightSetState(false);
        }
        else
        {
          caseLightSetState(true);
          caseLightSetBrightness(ack_value());
        }
        caseLightQuerySetWait(false);
        caseLightApplied(true);
      }
      // parse and store M420 V1 T1, Mesh data (e.g. from Mesh Editor menu)
      //
      // IMPORTANT: It must be placed before the following keys:
      //            1) echo:Bed Leveling
      //            2) mesh. Z offset:
      //
      else if (meshIsWaitingData())
      {
        meshUpdateData(dmaL2Cache);  // update mesh data
      }
      // parse and store M420 V1 T1 or M420 Sxx or M503, ABL state (e.g. from Bed Leveling menu)
      else if (ack_seen("echo:Bed Leveling"))
      {
        if (ack_seen("ON"))
          setParameter(P_ABL_STATE, 0, ENABLED);
        else
          setParameter(P_ABL_STATE, 0, DISABLED);
      }
      // parse and store M420 V1 T1 or G29 S0 (mesh. Z offset:) or M503 (G29 S4 Zxx), MBL Z offset value (e.g. from Babystep menu)
      else if (ack_seen("mesh. Z offset:") || ack_seen("G29 S4 Z"))
      {
        setParameter(P_MBL_OFFSET, 0, ack_value());
      }
      // parse and store M851, Probe Z offset value (e.g. from Babystep menu) and X an Y probe Offset for LevelCorner position limit to be fixed see ABL.c
      else if (ack_seen("Probe Offset"))
      {
        if (ack_seen("X")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_X, ack_value());
        if (ack_seen("Y")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Y, ack_value());
        if (ack_seen("Z:") || (ack_seen("Z"))) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Z, ack_value());
      }
      // parse G29 (ABL) + M118, ABL Completed message (ABL, BBL, UBL) (e.g. from ABL menu)
      else if (ack_seen("ABL Completed"))
      {
        ablUpdateStatus(true);
      }
      // parse G29 (MBL), MBL Completed message (e.g. from MBL menu)
      else if (ack_seen("Mesh probing done"))
      {
        mblUpdateStatus(true);
      }
      // G30 feedback to get the 4 corners Z value returned by Marlin for LevelCorner function
      else if (ack_seen("Bed X: "))
      {
        float valx = ack_value();
        float valy = 0;
        if (ack_seen("Y: ")) valy = ack_value();
        if (ack_seen("Z: "))
        {
          uint16_t x_mid = infoSettings.machine_size_min[X_AXIS] +
                           (infoSettings.machine_size_max[X_AXIS] - infoSettings.machine_size_min[X_AXIS]) / 2;
          uint16_t y_mid = infoSettings.machine_size_min[Y_AXIS] +
                           (infoSettings.machine_size_max[Y_AXIS] - infoSettings.machine_size_min[Y_AXIS]) / 2;

          if ((valx < x_mid) && (valy < y_mid))
          {
            SetLevelCornerPosition(0, ack_value());
          }
          else if ((valx > x_mid) && (valy < y_mid))
          {
            SetLevelCornerPosition(1, ack_value());
          }
          else if ((valx > x_mid) && (valy > y_mid))
          {
            SetLevelCornerPosition(2, ack_value());
          }
          else if ((valx < x_mid) && (valy > y_mid))
          {
            SetLevelCornerPosition(3, ack_value());
          }
        }
      }

      //----------------------------------------
      // Parameter / M503 / M115 parsed responses
      //----------------------------------------

      // parse and store stepper steps/mm values
      else if (ack_seen("M92"))
      {
        if (ack_seen("M92 X"))
        {
                             setParameter(P_STEPS_PER_MM, AXIS_INDEX_X, ack_value());
          if (ack_seen("Y")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Y, ack_value());
          if (ack_seen("Z")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Z, ack_value());
        }

        uint8_t i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("E")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_E0 + i, ack_value());
      }
      // parse and store stepper steps/mm values incase of RepRapFirmware
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && (ack_seen("Steps")))
      {
        if (ack_seen("X: ")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_X, ack_value());
        if (ack_seen("Y: ")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Y, ack_value());
        if (ack_seen("Z: ")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_Z, ack_value());
        if (ack_seen("E: ")) setParameter(P_STEPS_PER_MM, AXIS_INDEX_E0, ack_value());
      }
      // parse and store Filament settings values
      else if (ack_seen("M200"))
      {
        if (ack_seen("M200 S") || ack_seen("D0")) setParameter(P_FILAMENT_DIAMETER, 0, ack_value());

        uint8_t i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("D")) setParameter(P_FILAMENT_DIAMETER, 1 + i, ack_value());

        if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
        {
          if (getParameter(P_FILAMENT_DIAMETER, 1) > 0.01f)
            setParameter(P_FILAMENT_DIAMETER, 0, 1);  // filament_diameter>0.01 to enable  volumetric extrusion
          else
            setParameter(P_FILAMENT_DIAMETER, 0, 0);  // filament_diameter<=0.01 to disable volumetric extrusion
        }
      }
      // parse and store Max Acceleration values
      else if (ack_seen("M201"))
      {
        if (ack_seen("M201 X"))
        {
                             setParameter(P_MAX_ACCELERATION, AXIS_INDEX_X, ack_value());
          if (ack_seen("Y")) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_Y, ack_value());
          if (ack_seen("Z")) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_Z, ack_value());
        }

        uint8_t i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("E")) setParameter(P_MAX_ACCELERATION, AXIS_INDEX_E0 + i, ack_value());
      }
      // parse and store Max Feed Rate values
      else if (ack_seen("M203"))
      {
        if (ack_seen("M203 X"))
        {
                             setParameter(P_MAX_FEED_RATE, AXIS_INDEX_X, ack_value());
          if (ack_seen("Y")) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_Y, ack_value());
          if (ack_seen("Z")) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_Z, ack_value());
        }

        uint8_t i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("E")) setParameter(P_MAX_FEED_RATE, AXIS_INDEX_E0 + i, ack_value());
      }
      // parse and store Acceleration values
      else if (ack_seen("M204 P"))
      {
                           setParameter(P_ACCELERATION, 0, ack_value());
        if (ack_seen("R")) setParameter(P_ACCELERATION, 1, ack_value());
        if (ack_seen("T")) setParameter(P_ACCELERATION, 2, ack_value());
      }
      // parse and store jerk values
      else if (ack_seen("M205"))
      {
        if (ack_seen("X")) setParameter(P_JERK, AXIS_INDEX_X, ack_value());
        if (ack_seen("Y")) setParameter(P_JERK, AXIS_INDEX_Y, ack_value());
        if (ack_seen("Z")) setParameter(P_JERK, AXIS_INDEX_Z, ack_value());
        if (ack_seen("E")) setParameter(P_JERK, AXIS_INDEX_E0, ack_value());
        if (ack_seen("J")) setParameter(P_JUNCTION_DEVIATION, 0, ack_value());
      }
      // parse and store Home Offset values
      else if (ack_seen("M206 X"))
      {
                           setParameter(P_HOME_OFFSET, AXIS_INDEX_X, ack_value());
        if (ack_seen("Y")) setParameter(P_HOME_OFFSET, AXIS_INDEX_Y, ack_value());
        if (ack_seen("Z")) setParameter(P_HOME_OFFSET, AXIS_INDEX_Z, ack_value());
      }
      // parse and store FW retraction values
      else if (ack_seen("M207 S"))
      {
                           setParameter(P_FWRETRACT, 0, ack_value());
        if (ack_seen("W")) setParameter(P_FWRETRACT, 1, ack_value());
        if (ack_seen("F")) setParameter(P_FWRETRACT, 2, ack_value());
        if (ack_seen("Z")) setParameter(P_FWRETRACT, 3, ack_value());
      }
      // parse and store FW recover values
      else if (ack_seen("M208 S"))
      {
                           setParameter(P_FWRECOVER, 0, ack_value());
        if (ack_seen("W")) setParameter(P_FWRECOVER, 1, ack_value());
        if (ack_seen("F")) setParameter(P_FWRECOVER, 2, ack_value());
        if (ack_seen("R")) setParameter(P_FWRECOVER, 3, ack_value());
      }
      // parse and store auto FW retract state (M209 - Set Auto Retract)
      else if (ack_seen("M209 S"))
      {
        setParameter(P_AUTO_RETRACT, 0, ack_value());
      }
      // parse and store Offset 2nd Nozzle
      else if (ack_seen("M218 T1 X"))
      {
                           setParameter(P_HOTEND_OFFSET, 0, ack_value());
        if (ack_seen("Y")) setParameter(P_HOTEND_OFFSET, 1, ack_value());
        if (ack_seen("Z")) setParameter(P_HOTEND_OFFSET, 2, ack_value());
      }
      // parse and store Delta Configuration values
      //
      // IMPORTANT: It must be placed before the following keys:
      //            1) M420 S
      //
      else if (ack_seen("M665"))
      {
        if (ack_seen("H")) setParameter(P_DELTA_CONFIGURATION, 0, ack_value());
        if (ack_seen("S")) setParameter(P_DELTA_CONFIGURATION, 1, ack_value());
        if (ack_seen("R")) setParameter(P_DELTA_CONFIGURATION, 2, ack_value());
        if (ack_seen("L")) setParameter(P_DELTA_CONFIGURATION, 3, ack_value());
        if (ack_seen("X")) setParameter(P_DELTA_TOWER_ANGLE, AXIS_INDEX_X, ack_value());
        if (ack_seen("Y")) setParameter(P_DELTA_TOWER_ANGLE, AXIS_INDEX_Y, ack_value());
        if (ack_seen("Z")) setParameter(P_DELTA_TOWER_ANGLE, AXIS_INDEX_Z, ack_value());
      }
      // parse and store Delta Endstop Adjustments values
      //
      // IMPORTANT: It must be placed before the following keys:
      //            1) M420 S
      //
      else if (ack_seen("M666"))
      {
        if (ack_seen("X")) setParameter(P_DELTA_ENDSTOP, 0, ack_value());
        if (ack_seen("Y")) setParameter(P_DELTA_ENDSTOP, 1, ack_value());
        if (ack_seen("Z")) setParameter(P_DELTA_ENDSTOP, 2, ack_value());
      }
      // parse and store ABL on/off state & Z fade value on M503
      else if (ack_seen("M420 S"))
      {
        if (ack_seen("S")) setParameter(P_ABL_STATE, 0, ack_value());
        if (ack_seen("Z")) setParameter(P_ABL_STATE, 1, ack_value());
      }
      // parse and store TMC Stealth Chop
      else if (ack_seen("Driver stepping mode:"))  // poll stelthchop settings separately to
      {
        storeCmd("M569\n");
      }
      else if (ack_seen("driver mode:"))
      {
        uint8_t k = (ack_seen("stealthChop")) ? 1 : 0;
        int8_t i = 0;
        if (ack_seen("X"))
        {
          i = ack_value();
          i = STEPPER_INDEX_X + ((i > 0) ? (i - 1) : 0);
        }
        else if (ack_seen("Y"))
        {
          i = ack_value();
          i = STEPPER_INDEX_Y + ((i > 0) ? (i - 1) : 0);
        }
        else if (ack_seen("Z"))
        {
          i = ack_value();
          i = STEPPER_INDEX_Z + ((i > 0) ? (i - 1) : 0);
        }
        else if (ack_seen("E"))
        {
          i = ack_value() + STEPPER_INDEX_E0;
        }
        setParameter(P_STEALTH_CHOP, i, k);
      }
      // parse and store Probe Offset values
      else if (ack_seen("M851 X"))
      {
                           setParameter(P_PROBE_OFFSET, AXIS_INDEX_X, ack_value());
        if (ack_seen("Y")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Y, ack_value());
        if (ack_seen("Z")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Z, ack_value());
      }
      // parse and store linear advance values
      else if (ack_seen("M900"))
      {
        uint8_t i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("K")) setParameter(P_LIN_ADV, i, ack_value());
      }
      // parse and store stepper driver current values
      else if (ack_seen("M906"))
      {
        uint8_t i = (ack_seen("I")) ? ack_value() : 0;
        if (ack_seen("X")) setParameter(P_CURRENT, STEPPER_INDEX_X + i, ack_value());
        if (ack_seen("Y")) setParameter(P_CURRENT, STEPPER_INDEX_Y + i, ack_value());
        if (ack_seen("Z")) setParameter(P_CURRENT, STEPPER_INDEX_Z + i, ack_value());

        i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("E")) setParameter(P_CURRENT, STEPPER_INDEX_E0 + i, ack_value());
      }
      // parse and store TMC Hybrid Threshold Speed
      else if (ack_seen("M913"))
      {
        uint8_t i = (ack_seen("I")) ? ack_value() : 0;
        if (ack_seen("X")) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_X + i, ack_value());
        if (ack_seen("Y")) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_Y + i, ack_value());
        if (ack_seen("Z")) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_Z + i, ack_value());

        i = (ack_seen("T")) ? ack_value() : 0;
        if (ack_seen("E")) setParameter(P_HYBRID_THRESHOLD, STEPPER_INDEX_E0 + i, ack_value());
      }
      // parse and store TMC Bump sensitivity values
      else if (ack_seen("M914"))
      {
        uint8_t i = (ack_seen("I")) ? ack_value() : 0;
        if (ack_seen("X")) setParameter(P_BUMPSENSITIVITY, STEPPER_INDEX_X + i, ack_value());
        if (ack_seen("Y")) setParameter(P_BUMPSENSITIVITY, STEPPER_INDEX_Y + i, ack_value());
        if (ack_seen("Z")) setParameter(P_BUMPSENSITIVITY, STEPPER_INDEX_Z + i, ack_value());
      }
      // parse and store ABL type if auto-detect is enabled
      #if ENABLE_BL_VALUE == 1
        else if (ack_seen("Auto Bed Leveling"))
        {
          infoMachineSettings.leveling = BL_ABL;
        }
        else if (ack_seen("Unified Bed Leveling"))
        {
          infoMachineSettings.leveling = BL_UBL;
        }
        else if (ack_seen("Mesh Bed Leveling"))
        {
          infoMachineSettings.leveling = BL_MBL;
        }
      #endif
      // parse M115 capability report
      else if (ack_seen("FIRMWARE_NAME:"))
      {
        uint8_t *string = (uint8_t *)&dmaL2Cache[ack_index];
        uint16_t string_start = ack_index;
        uint16_t string_end = string_start;

        if (ack_seen("Marlin"))
        {
          infoMachineSettings.firmwareType = FW_MARLIN;
        }
        else if (ack_seen("RepRapFirmware"))
        {
          infoMachineSettings.firmwareType = FW_REPRAPFW;
          setupMachine();
        }
        else if (ack_seen("Smoothieware"))
        {
          infoMachineSettings.firmwareType = FW_SMOOTHIEWARE;
          setupMachine();
        }
        else
        {
          infoMachineSettings.firmwareType = FW_UNKNOWN;
          setupMachine();
        }
        if (ack_seen("FIRMWARE_URL:"))  // For Smoothieware
          string_end = ack_index - sizeof("FIRMWARE_URL:");
        else if (ack_seen("SOURCE_CODE_URL:"))  // For Marlin
          string_end = ack_index - sizeof("SOURCE_CODE_URL:");
        else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && ack_seen("ELECTRONICS"))  // For RepRapFirmware
          string_end = ack_index - sizeof("ELECTRONICS");

        infoSetFirmwareName(string, string_end - string_start);  // Set firmware name

        if (ack_seen("MACHINE_TYPE:"))
        {
          string = (uint8_t *)&dmaL2Cache[ack_index];
          string_start = ack_index;
          if (ack_seen("EXTRUDER_COUNT:"))
          {
            if (MIXING_EXTRUDER == 0)
            {
              infoSettings.ext_count = ack_value();
            }
            string_end = ack_index - sizeof("EXTRUDER_COUNT:");
          }
          infoSetMachineType(string, string_end - string_start);  // Set firmware name
        }
      }
      else if (ack_seen("Cap:EEPROM:"))
      {
        infoMachineSettings.EEPROM = ack_value();
      }
      else if (ack_seen("Cap:AUTOREPORT_TEMP:"))
      {
        infoMachineSettings.autoReportTemp = ack_value();
        if (infoMachineSettings.autoReportTemp)
        {
          storeCmd("M155 ");
        }
      }
      else if (ack_seen("Cap:AUTOREPORT_POS:"))
      {
        infoMachineSettings.autoReportPos = ack_value();
      }
      else if (ack_seen("Cap:AUTOLEVEL:") && infoMachineSettings.leveling == BL_DISABLED)
      {
        infoMachineSettings.leveling = ack_value();
      }
      else if (ack_seen("Cap:Z_PROBE:"))
      {
        infoMachineSettings.zProbe = ack_value();
      }
      else if (ack_seen("Cap:LEVELING_DATA:"))
      {
        infoMachineSettings.levelingData = ack_value();
      }
      else if (ack_seen("Cap:SOFTWARE_POWER:"))
      {
        infoMachineSettings.softwarePower = ack_value();
      }
      else if (ack_seen("Cap:TOGGLE_LIGHTS:"))
      {
        infoMachineSettings.toggleLights = ack_value();
      }
      else if (ack_seen("Cap:CASE_LIGHT_BRIGHTNESS:"))
      {
        infoMachineSettings.caseLightsBrightness = ack_value();
      }
      else if (ack_seen("Cap:EMERGENCY_PARSER:"))
      {
        infoMachineSettings.emergencyParser = ack_value();
      }
      else if (ack_seen("Cap:PROMPT_SUPPORT:"))
      {
        infoMachineSettings.promptSupport = ack_value();
      }
      else if (ack_seen("Cap:SDCARD:") && infoSettings.onboardSD == AUTO)
      {
        infoMachineSettings.onboard_sd_support = ack_value();
      }
      else if (ack_seen("Cap:AUTOREPORT_SD_STATUS:"))
      {
        infoMachineSettings.autoReportSDStatus = ack_value();
      }
      else if (ack_seen("Cap:LONG_FILENAME:") && infoSettings.longFileName == AUTO)
      {
        infoMachineSettings.long_filename_support = ack_value();
      }
      else if (ack_seen("Cap:BABYSTEPPING:"))
      {
        infoMachineSettings.babyStepping = ack_value();
      }
      else if (ack_seen("Cap:BUILD_PERCENT:"))  // M73 support. Required "LCD_SET_PROGRESS_MANUALLY" in Marlin
      {
        infoMachineSettings.buildPercent = ack_value();
      }
      else if (ack_seen("Cap:CHAMBER_TEMPERATURE:"))
      {
        infoSettings.chamber_en = ack_value();
        setupMachine();
      }

      //----------------------------------------
      // Error / echo parsed responses
      //----------------------------------------

      // parse error messages
      else if (ack_seen(errormagic))
      {
        ackPopupInfo(errormagic);
      }
      // parse echo messages
      else if (ack_seen(echomagic))
      {
        if (!processKnownEcho())  // if no known echo was found and processed, then popup the echo message
        {
          ackPopupInfo(echomagic);
        }
      }

      // keep it here and parse it the latest
      else if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      {
        if (ack_seen(warningmagic))
        {
          ackPopupInfo(warningmagic);
        }
        else if (ack_seen(messagemagic))
        {
          ackPopupInfo(messagemagic);
        }
        else if (ack_seen("access point "))
        {
          uint8_t *string = (uint8_t *)&dmaL2Cache[ack_index];
          uint16_t string_start = ack_index;
          uint16_t string_end = string_start;
          if (ack_seen(","))
            string_end = ack_index - 1 ;

          infoSetAccessPoint(string, string_end - string_start);  // Set access poing

          if (ack_seen("IP address "))
          {
            string = (uint8_t *)&dmaL2Cache[ack_index];
            string_start = ack_index;
            if (ack_seen("\n"))
              string_end = ack_index - 1;
            infoSetIPAddress(string, string_end - string_start);  // Set IP address
          }
        }
      }
      else if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      {
        if (ack_seen(errorZProbe))  // smoothieboard ZProbe triggered before move, aborting command.
        {
          ackPopupInfo("ZProbe triggered before move.\nAborting Print!");
        }
        // parse and store volumetric extrusion M200 response of Smoothieware
        else if (ack_seen("Volumetric extrusion is disabled"))
        {
          setParameter(P_FILAMENT_DIAMETER, 0, 0);
          setParameter(P_FILAMENT_DIAMETER, 1, 0.0F);
        }
        // parse and store volumetric extrusion M200 response of Smoothieware
        else if (ack_seen("Filament Diameter:"))
        {
          setParameter(P_FILAMENT_DIAMETER, 1, ack_value());
          if (getParameter(P_FILAMENT_DIAMETER, 1) > 0.01F)
            setParameter(P_FILAMENT_DIAMETER, 0, 1);  // filament_diameter>0.01 to enable  volumetric extrusion
          else
            setParameter(P_FILAMENT_DIAMETER, 0, 0);  // filament_diameter<=0.01 to disable volumetric extrusion
        }
      }
    }

  parse_end:
    if (ack_cur_src != SERIAL_PORT)
    {
      Serial_Puts(ack_cur_src, dmaL2Cache);
    }
    #ifdef SERIAL_PORT_2
      else if (!ack_seen("ok") || ack_seen("T:") || ack_seen("T0:"))  // if a spontaneous ACK message
      {
        // pass on the spontaneous ACK message to all the extra serial ports (since these messages come unrequested)
        for (uint8_t i = 0; i < PORT_COUNT; i++)
        {
          if (extraSerialPort[i].activePort)  // if the port is connected to an active device (a device that already sent data to the TFT)
          {
            // pass on the ACK message to the port
            Serial_Puts(extraSerialPort[i].port, dmaL2Cache);
          }
        }
      }
    #endif

    if (avoid_terminal != true)
    {
      terminalCache(dmaL2Cache, TERMINAL_ACK);
    }
  }
}

void parseRcvGcode(void)
{
  #ifdef SERIAL_PORT_2
    uint8_t port;

    // scan all the extra serial ports
    for (uint8_t i = 0; i < PORT_COUNT; i++)
    {
      port = extraSerialPort[i].port;

      if (infoHost.rx_ok[port] == true)
      {
        infoHost.rx_ok[port] = false;

        while (dmaL1NotEmpty(port))
        {
          syncL2CacheFromL1(port);
          storeCmdFromUART(port, dmaL2Cache);
        }

        extraSerialPort[i].activePort = true;
      }
    }
  #endif
}
