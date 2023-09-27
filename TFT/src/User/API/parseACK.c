#include "parseACK.h"
#include "includes.h"
#include "RRFParseACK.hpp"

typedef enum  // popup message types available to display an echo message
{
  ECHO_NOTIFY_NONE = 0,  // ignore the echo message
  ECHO_NOTIFY_TOAST,     // show a non invasive toast on the title bar for a preset duration.
  ECHO_NOTIFY_DIALOG,    // show a window to notify the user and alow interaction.
} ECHO_NOTIFY_TYPE;

typedef struct
{
  ECHO_NOTIFY_TYPE notifyType;
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
  {ECHO_NOTIFY_TOAST, "echo:Active Mesh"},        // M503
  {ECHO_NOTIFY_TOAST, "echo:EEPROM can"},         // M503
  {ECHO_NOTIFY_NONE, "Cap:"},                     // M115
  {ECHO_NOTIFY_NONE, "Config:"},                  // M360
  {ECHO_NOTIFY_TOAST, "Settings Stored"},         // M500
  {ECHO_NOTIFY_TOAST, "echo:Bed"},                // M420
  {ECHO_NOTIFY_TOAST, "echo:Fade"},               // M420
  {ECHO_NOTIFY_TOAST, "echo:Active Extruder"},    // Tool Change
  {ECHO_NOTIFY_NONE, "Unknown command: \"M150"},  // M150
};

const char magic_error[] = "Error:";
const char magic_echo[] = "echo:";
const char magic_warning[] = "Warning:";  // RRF warning
const char magic_message[] = "message";   // RRF message in Json format

#define ACK_CACHE_SIZE 512  // including ending character '\0'

char ack_cache[ACK_CACHE_SIZE];             // buffer where read ACK messages are stored
uint16_t ack_len;                           // length of data currently present in ack_cache
uint16_t ack_index;
SERIAL_PORT_INDEX ack_port_index = PORT_1;  // index of target serial port for the ACK message (related to originating gcode)
bool hostDialog = false;

struct HOST_ACTION
{
  char prompt_begin[30];
  char prompt_button[2][20];
  bool prompt_show;           // show popup reminder or not
  uint8_t button;             // number of buttons
} hostAction;

void setHostDialog(bool isHostDialog)
{
  hostDialog = isHostDialog;
}

bool getHostDialog(void)
{
  return hostDialog;
}

void setCurrentAckSrc(SERIAL_PORT_INDEX portIndex)
{
  ack_port_index = portIndex;
}

// checks if the cache starts with the given parameter
static bool ack_starts_with(const char * str)
{
  uint16_t i = 0;

  while (str[i] == ack_cache[i])
  {
    if (str[++i] == '\0')
    {
      ack_index = i;
      return true;
    }
  }

  return false;
}

// searches the first appearance of the given string from the start
// of the cache, on success sets the current index of the cache
// ("ack_index") next to the position where the found string ended
static bool ack_seen(const char * str)
{
  uint16_t i;

  for (ack_index = 0, i = 0; ack_cache[ack_index] != '\0'; ack_index++, i = 0)
  {
    while (str[i] == ack_cache[ack_index + i])
    {
      if (str[++i] == '\0')
      {
        ack_index += i;
        return true;
      }
    }
  }

  return false;
}

// unlike "ack_seen()", this starts the search from the current index, where previous
// search left off and retains "ack_index" if the searched string is not found
static bool ack_continue_seen(const char * str)
{
  uint16_t tmp_index;
  uint16_t i;

  for (tmp_index = ack_index, i = 0; ack_cache[tmp_index] != '\0'; tmp_index++, i = 0)
  {
    while (str[i] == ack_cache[tmp_index + i])
    {
      if (str[++i] == '\0')
      {
        ack_index = tmp_index + i;
        return true;
      }
    }
  }

  return false;
}

static float ack_value()
{
  return (strtod(&ack_cache[ack_index], NULL));
}

// read the value after "/", if any
static float ack_second_value()
{
  char * secondValue = strchr(&ack_cache[ack_index], '/');

  if (secondValue != NULL)
    return (strtod(secondValue + 1, NULL));
  else
    return -0.5;
}

void ack_values_sum(float * data)
{
  while (((ack_cache[ack_index] < '0') || (ack_cache[ack_index] > '9')) && ack_cache[ack_index] != '\n')
  {
    ack_index++;
  }

  *data += ack_value();

  while ((((ack_cache[ack_index] >= '0') && (ack_cache[ack_index] <= '9')) ||
          (ack_cache[ack_index] == '.')) && (ack_cache[ack_index] != '\n'))
  {
    ack_index++;
  }

  if (ack_cache[ack_index] != '\n')
    ack_values_sum(data);
}

void ackPopupInfo(const char * info)
{
  bool show_dialog = true;

  if (MENU_IS(menuTerminal) || (MENU_IS(menuStatus) && info == magic_echo))
    show_dialog = false;

  // play notification sound if buzzer for ACK is enabled
  if (info == magic_error)
    BUZZER_PLAY(SOUND_ERROR);
  else if (info == magic_echo && infoSettings.ack_notification == 1)
    BUZZER_PLAY(SOUND_NOTIFY);

  // set echo message in status screen
  if (info == magic_echo || info == magic_message)
  {
    // ignore all messages if parameter settings is open
    if (MENU_IS(menuParameterSettings))
      return;

    // show notification based on notificaiton settings
    if (infoSettings.ack_notification == 1)
      addNotification(DIALOG_TYPE_INFO, (char *)info, (char *)ack_cache + ack_index, show_dialog);
    else if (infoSettings.ack_notification == 2)
      addToast(DIALOG_TYPE_INFO, ack_cache);  // show toast notificaion if turned on
  }
  else
  {
    addNotification(DIALOG_TYPE_ERROR, (char *)info, (char *)ack_cache + ack_index, show_dialog);
  }
}

bool processKnownEcho(void)
{
  bool isKnown = false;
  uint8_t i;

  for (i = 0; i < COUNT(knownEcho); i++)
  {
    if (strstr(ack_cache, knownEcho[i].msg))
    {
      isKnown = true;
      break;
    }
  }

  // display the busy indicator
  drawBusySign();

  if (isKnown)
  {
    if (knownEcho[i].notifyType == ECHO_NOTIFY_NONE)
      return isKnown;

    if (knownEcho[i].notifyType == ECHO_NOTIFY_TOAST)
    {
      addToast(DIALOG_TYPE_INFO, ack_cache);
    }
    else if (knownEcho[i].notifyType == ECHO_NOTIFY_DIALOG)
    {
      BUZZER_PLAY(SOUND_NOTIFY);
      addNotification(DIALOG_TYPE_INFO, (char *)magic_echo, (char *)ack_cache + ack_index, true);
    }
  }

  return isKnown;
}

void hostActionCommands(void)
{
  if (ack_seen(":notification "))
  {
    uint16_t index = ack_index;  // save the current index for further usage

    if (ack_continue_seen("Time Left"))  // parsing printing time left
    {
      // format: "Time Left <XX>h<YY>m<ZZ>s" (e.g. "Time Left 02h04m06s")
      parsePrintRemainingTime((char *)ack_cache + ack_index);
    }
    else if (ack_continue_seen("Layer Left"))  // parsing printing layer left
    {
      // format: "Layer Left <XXXX>/<YYYY>" (e.g. "Layer Left 51/940")
      setPrintLayerNumber(ack_value());
      setPrintLayerCount(ack_second_value());
    }
    else if (ack_continue_seen("Data Left"))  // parsing printing data left
    {
      // format: "Data Left <XXXX>/<YYYY>" (e.g. "Data Left 123/12345")
      setPrintProgressData(ack_value(), ack_second_value());
    }
    else
    {
      statusScreen_setMsg((uint8_t *)magic_echo, (uint8_t *)ack_cache + index);  // always display the notification on status screen

      if (!ack_continue_seen("Ready."))  // avoid to display unneeded/frequent useless notifications (e.g. "My printer Ready.")
      {
        if (MENU_IS_NOT(menuStatus))  // don't show it when in menuStatus
          addToast(DIALOG_TYPE_INFO, ack_cache + index);

        if (infoSettings.notification_m117 == ENABLED)
          addNotification(DIALOG_TYPE_INFO, (char *)magic_echo, (char *)ack_cache + index, false);
      }
    }
  }
  else if (ack_seen(":print_start"))  // print started from remote host (e.g. OctoPrint etc...)
  {
    startPrintingFromRemoteHost(NULL);  // start print originated and hosted by remote host and open Printing menu
  }
  else if (ack_seen(":print_end"))  // print ended from remote host (e.g. OctoPrint etc...)
  {
    endPrint();
  }
  else if (ack_seen(":pause") || ack_seen(":paused"))
  {
    if (infoMachineSettings.firmwareType == FW_MARLIN && ack_seen(":paused"))
    { // if paused with ADVANCED_PAUSE_FEATURE enabled in Marlin (:paused),
      // disable Resume/Pause button in the Printing menu
      hostDialog = true;
    }

    setPrintPause(HOST_STATUS_PAUSING, PAUSE_EXTERNAL);

    if (ack_continue_seen("filament_runout"))
      setRunoutAlarmTrue();
  }
  else if (ack_seen(":resume") || ack_seen(":resumed"))
  {
    hostDialog = false;  // enable Resume/Pause button in the Printing menu

    setPrintResume(HOST_STATUS_RESUMING);
  }
  else if (ack_seen(":cancel"))  // to be added to Marlin abortprint routine
  {
    setPrintAbort();
  }
  else if (ack_seen(":prompt_begin "))
  {
    strcpy(hostAction.prompt_begin, ack_cache + ack_index);
    hostAction.button = 0;
    hostAction.prompt_show = true;

    if (ack_continue_seen("Nozzle Parked"))
    {
      setPrintPause(HOST_STATUS_PAUSING, PAUSE_EXTERNAL);
    }
    else if (ack_continue_seen("Resuming"))  // resuming from TFT media or (remote) onboard media
    {
      setPrintResume(HOST_STATUS_RESUMING);

      hostAction.prompt_show = false;
      sendEmergencyCmd("M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
    else if (ack_continue_seen("Reheating"))
    {
      hostAction.prompt_show = false;
      sendEmergencyCmd("M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
  }
  else if (ack_seen(":prompt_button "))
  {
    strcpy(hostAction.prompt_button[hostAction.button++], ack_cache + ack_index);
  }
  else if (ack_seen(":prompt_show") && hostAction.prompt_show)
  {
    BUZZER_PLAY(SOUND_NOTIFY);

    switch (hostAction.button)
    {
      case 0:
        popupDialog(DIALOG_TYPE_ALERT, (uint8_t *)"Message", (uint8_t *)hostAction.prompt_begin,
                    LABEL_CONFIRM, LABEL_NULL, setRunoutAlarmFalse, NULL, NULL);
        break;

      case 1:
        popupDialog(DIALOG_TYPE_ALERT, (uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin,
                    (uint8_t *)hostAction.prompt_button[0], LABEL_NULL, breakAndContinue, NULL, NULL);
        break;

      case 2:
        popupDialog(DIALOG_TYPE_ALERT, (uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin,
                    (uint8_t *)hostAction.prompt_button[0], (uint8_t *)hostAction.prompt_button[1], resumeAndPurge, resumeAndContinue, NULL);
        break;
    }
  }
}

void parseACK(void)
{
  while ((ack_len = Serial_Get(SERIAL_PORT, ack_cache, ACK_CACHE_SIZE)) != 0)  // if some data have been retrieved
  {
    #if defined(SERIAL_DEBUG_PORT) && defined(DEBUG_SERIAL_COMM)
      // dump raw serial data received to debug port
      Serial_Put(SERIAL_DEBUG_PORT, "<<");
      Serial_Put(SERIAL_DEBUG_PORT, ack_cache);
    #endif

    bool avoid_terminal = false;

    //----------------------------------------
    // TFT to printer connection handling
    //----------------------------------------

    if (infoHost.connected == false)
    {
      // parse error information even though not connected to printer
      if (ack_seen(magic_error))
        ackPopupInfo(magic_error);

      // the first response should be such as "T:25/50\n"
      // the "T:0" response is specifically for Marlin when EXTRUDER_COUNT:0
      if (!(ack_seen("@") && ack_seen("T:")) && !ack_seen("T0:") && !ack_seen("T:0"))
        goto parse_end;

      for (uint8_t i = NOZZLE0; i < MAX_HOTEND_COUNT; i++)  // find hotend count and setup heaters
      {
        if (!ack_seen(heaterID[i]))
          break;
      }

      if (infoSettings.ext_count < infoSettings.hotend_count)
        infoSettings.ext_count = infoSettings.hotend_count;

      if (ack_seen(heaterID[BED]))
        infoSettings.bed_en = ENABLED;

      if (ack_seen(heaterID[CHAMBER]))
        infoSettings.chamber_en = ENABLED;

      updateNextHeatCheckTime();

      if (!ack_seen("@"))  // it's RepRapFirmware
      {
        storeCmd("M92\n");
        storeCmd("M115\n");  // as last command to identify the FW type!
        coordinateQuerySetWait(true);
      }
      else if (infoMachineSettings.firmwareType == FW_NOT_DETECTED)  // if never connected to the printer since boot
      {
        storeCmd("M503\n");  // query detailed printer capabilities
        storeCmd("M92\n");   // steps/mm of extruder is an important parameter for Smart filament runout
                             // avoid can't getting this parameter due to disabled M503 in Marlin
        storeCmd("M211\n");  // retrieve the software endstops state
        storeCmd("M115\n");  // as last command to identify the FW type!
      }

      // 1) store on command queue the above gcodes to detect printer info
      // 2) re-initialize infoHost when connected to avoid this code branch is executed again
      // 3) set requestCommandInfo.inJson to "false" and detect the presence of Marlin ADVANCED_OK
      //    feature (if any) and its command queue size
      // 4) finally, set listening mode flag according to its last state stored in flash
      //
      // NOTES:
      //   - 3) is applied only after all gcodes in 1) have been sent and their responses received and processed
      //   - InfoHost_UpdateListeningMode() must always be invoked as last due to no gcode could be sent in case
      //     listening mode is enabled (e.g. a TFT freeze will occur in case detectAdvancedOk() is invoked after
      //     InfoHost_UpdateListeningMode())

      InfoHost_Init(true);

      requestCommandInfo.inJson = false;

      detectAdvancedOk();

      InfoHost_UpdateListeningMode();
    }

    //----------------------------------------
    // Onboard media response handling (response requested by the use of setRequestCommandInfo() function)
    //----------------------------------------

    if (requestCommandInfo.inWaitResponse)
    {
      if (ack_seen(requestCommandInfo.startMagic))
      {
        requestCommandInfo.inResponse = true;
        requestCommandInfo.inWaitResponse = false;
      }
      else if ((requestCommandInfo.error_num > 0 && ack_seen(requestCommandInfo.errorMagic[0])) ||
               (requestCommandInfo.error_num > 1 && ack_seen(requestCommandInfo.errorMagic[1])) ||
               (requestCommandInfo.error_num > 2 && ack_seen(requestCommandInfo.errorMagic[2])))
      { // parse onboard media error
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        requestCommandInfo.inError = true;
        requestCommandInfo.inWaitResponse = false;

        if (requestCommandInfo.stream_handler != NULL)
        {
          clearRequestCommandInfo();  // unused if the streaming handler is involved
          requestCommandInfo.stream_handler(ack_cache);
        }
        else
        {
          strcpy(requestCommandInfo.cmd_rev_buf, ack_cache);
        }

        BUZZER_PLAY(SOUND_ERROR);

        goto parse_end;
      }

      requestCommandInfo.inJson = false;
    }

    if (requestCommandInfo.inResponse)
    {
      if (requestCommandInfo.stream_handler != NULL)
      {
        clearRequestCommandInfo();  // unused if the streaming handler is involved
        requestCommandInfo.stream_handler(ack_cache);

        if (ack_seen(requestCommandInfo.stopMagic))
        {
          requestCommandInfo.done = true;
          requestCommandInfo.inResponse = false;
        }
      }
      else if (strlen(requestCommandInfo.cmd_rev_buf) + ack_len < CMD_MAX_REV)
      {
        strcat(requestCommandInfo.cmd_rev_buf, ack_cache);

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
        ackPopupInfo(magic_error);
      }

      requestCommandInfo.inJson = false;

      if (requestCommandInfo.done)  // if command parsing is completed
      {
        // if RepRap or "ok" (e.g. in Marlin) is used as stop magic keyword,
        // proceed with generic OK response handling to update infoHost.tx_slots and infoHost.tx_count
        //
        if (infoMachineSettings.firmwareType == FW_REPRAPFW || ack_starts_with("ok"))
          InfoHost_HandleOkAck(HOST_SLOTS_GENERIC_OK);
      }

      goto parse_end;
    }

    //----------------------------------------
    // RepRap response handling (response NOT requested by the use of setRequestCommandInfo() function)
    //----------------------------------------

    // check for a possible json response and eventually parse and process it
    else if (!requestCommandInfo.inWaitResponse && infoMachineSettings.firmwareType == FW_REPRAPFW)
    {
      if (strchr(ack_cache, '{') != NULL)
        requestCommandInfo.inJson = true;
    }

    if (requestCommandInfo.inJson)
    {
      if (ack_seen(magic_warning))
        ackPopupInfo(magic_warning);
      else
        rrfParseACK(ack_cache);

      // proceed with generic OK response handling to update infoHost.tx_slots and infoHost.tx_count
      InfoHost_HandleOkAck(HOST_SLOTS_GENERIC_OK);

      goto parse_end;
    }

    //----------------------------------------
    // "ok" response handling
    //----------------------------------------

    // it is checked first (and not later on) because it is the most frequent response during printing
    if (ack_starts_with("ok"))
    {
      // if regular OK response ("ok\n")
      if (ack_cache[ack_index] == '\n')
      {
        InfoHost_HandleOkAck(infoSettings.tx_slots);

        goto parse_end;  // nothing else to do
      }

      // if ADVANCED_OK response (e.g. "ok N10 P15 B3\n"). Required "ADVANCED_OK" in Marlin
      if (ack_continue_seen("P") && ack_continue_seen("B"))
      {
        InfoHost_HandleOkAck(ack_value());

        goto parse_end;  // nothing else to do
      }

      // if here, it is a temperature response (e.g. "ok T:16.13 /0.00 B:16.64 /0.00 @:0 B@:0\n").
      // Proceed with generic OK response handling to update infoHost.tx_slots and infoHost.tx_count
      // and then continue applying the next matching patterns to handle the temperature response
      InfoHost_HandleOkAck(HOST_SLOTS_GENERIC_OK);
    }

    //----------------------------------------
    // "wait" response handling
    //----------------------------------------

    // it is checked second (and not later on) because it is the most frequent response during printer idle
    if (ack_starts_with("wait"))
    {
      avoid_terminal = !infoSettings.terminal_ack;  // suppress "wait" from terminal
    }

    //----------------------------------------
    // Pushed / polled / on printing parsed responses
    //----------------------------------------

    // parse and store temperatures (e.g. "ok T:16.13 /0.00 B:16.64 /0.00 @:0 B@:0\n")
    else if ((ack_seen("@") && ack_seen("T:")) || ack_seen("T0:"))
    {
      uint8_t heaterIndex = NOZZLE0;

      if (infoSettings.hotend_count == 1)
      {
        heatSetCurrentTemp(heaterIndex, ack_value() + 0.5f);
        heatSetTargetTemp(heaterIndex, ack_second_value() + 0.5f, FROM_HOST);
        heaterIndex = BED;
      }

      while (heaterIndex < MAX_HEATER_COUNT)
      {
        if (heaterDisplayIsValid(heaterIndex) && ack_seen(heaterID[heaterIndex]))
        {
          heatSetCurrentTemp(heaterIndex, ack_value() + 0.5f);
          heatSetTargetTemp(heaterIndex, ack_second_value() + 0.5f, FROM_HOST);
        }

        heaterIndex++;
      }

      avoid_terminal = !infoSettings.terminal_ack;
      updateNextHeatCheckTime();
    }
    // parse and store M114, current position
    else if (ack_starts_with("X:") || ack_seen("C: X:"))  // Smoothieware axis position starts with "C: X:"
    {
      coordinateSetAxisActual(X_AXIS, ack_value());

      if (ack_continue_seen("Y:"))
      {
        coordinateSetAxisActual(Y_AXIS, ack_value());

        if (ack_continue_seen("Z:"))
        {
          coordinateSetAxisActual(Z_AXIS, ack_value());

          if (ack_continue_seen("E:"))
            coordinateSetAxisActual(E_AXIS, ack_value());
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
    else if (ack_seen("FR:"))
    {
      speedSetCurPercent(0, ack_value());
      speedQuerySetWait(false);
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
    else if (ack_starts_with("M106"))
    {
      fanSetCurSpeed(ack_continue_seen("P") ? ack_value() : 0, ack_seen("S") ? ack_value() : 100);
    }
    // parse and store M710, controller fan
    else if (ack_starts_with("M710"))
    {
      if (ack_seen("S"))
        fanSetCurSpeed(MAX_COOLING_FAN_COUNT, ack_value());

      if (ack_seen("I"))
        fanSetCurSpeed(MAX_COOLING_FAN_COUNT + 1, ack_value());

      ctrlFanQuerySetWait(false);
    }
    // parse pause message
    else if (!infoMachineSettings.promptSupport && ack_seen("paused for user"))
    {
      popupDialog(DIALOG_TYPE_QUESTION, (uint8_t *)"Printer is Paused", (uint8_t *)"Paused for user\ncontinue?",
                  LABEL_CONFIRM, LABEL_NULL, breakAndContinue, NULL, NULL);
    }
    // parse host action commands. Required "HOST_ACTION_COMMANDS" and other settings in Marlin
    else if (ack_starts_with("//action:"))
    {
      hostActionCommands();
    }
    // parse and store M118, filament data update
    else if (ack_seen("filament_data"))
    {
      if (ack_continue_seen("L:"))
        ack_values_sum(&infoPrintSummary.length);
      else if (ack_continue_seen("W:"))
        ack_values_sum(&infoPrintSummary.weight);
      else if (ack_continue_seen("C:"))
        ack_values_sum(&infoPrintSummary.cost);

      infoPrintSummary.hasFilamentData = true;
    }
    // parse and store M23, select SD file
    else if (infoMachineSettings.onboardSD == ENABLED && ack_starts_with("File opened:"))
    {
      // NOTE: this block is not reached in case of printing from onboard media because startPrint() in Printing.c will
      //       call request_M23_M36() that will be managed in parseAck() by the block "Onboard media response handling"
      //       provided at the beginning of this funtion

      // parse file name.
      // Format: "File opened: <file name> Size: <YYYY>" (e.g. "File opened: 1A29A~1.GCO Size: 6974")
      //
      char file_name[MAX_PATH_LEN];
      char * end_string = " Size:";

      uint16_t start_index = ack_index;
      uint16_t end_index = ack_continue_seen(end_string) ? (ack_index - strlen(end_string)) : start_index;
      uint16_t path_len = MIN(end_index - start_index, MAX_PATH_LEN - 1);

      memcpy(file_name, ack_cache + start_index, path_len);
      file_name[path_len] = '\0';

      // start print originated by remote host but hosted by onboard
      // (print from remote onboard media) and open Printing menu
      startPrintingFromRemoteHost(file_name);
    }
    // parse and store M24, M27 and M73, if printing from (remote) onboard media
    else if (infoMachineSettings.onboardSD == ENABLED && WITHIN(infoFile.source, FS_ONBOARD_MEDIA, FS_ONBOARD_MEDIA_REMOTE) &&
             (ack_starts_with("Done printing file") || ack_seen("SD printing") || ack_starts_with("echo: M73")))
    {
      // NOTE FOR "M73": Required "SET_PROGRESS_MANUALLY" and "M73_REPORT" settings in Marlin

      // parse and store M24, received "Done printing file" (printing from (remote) onboard media completed)
      if (ack_starts_with("Done"))
      {
        endPrint();
      }
      // parse and store M27, received "SD printing byte" or "Not SD printing"
      else if (ack_seen("SD"))
      {
        if (infoHost.status == HOST_STATUS_RESUMING)
          setPrintResume(HOST_STATUS_PRINTING);
        else if (infoHost.status == HOST_STATUS_PAUSING)
          setPrintPause(HOST_STATUS_PAUSED, PAUSE_EXTERNAL);

        if (infoHost.status == HOST_STATUS_PRINTING)
        {
          if (ack_continue_seen("byte"))  // received "SD printing byte"
          {
            // parse file data progress.
            // Format: "SD printing byte <XXXX>/<YYYY>" (e.g. "SD printing byte 123/12345")
            //
            setPrintProgressData(ack_value(), ack_second_value());
          }
          else  // received "Not SD printing"
          {
            setPrintAbort();
          }
        }
      }
      // parse and store M73
      else
      {
        // parse progress percentage and remaining time.
        // Format: "M73 Progress: <XX>%; Time left: <YY>m;" (e.g. "M73 Progress: 40%; Time left: 2m;")

        if (ack_seen("Progress:"))
        {
          setPrintProgressSource(PROG_SLICER);
          setPrintProgressPercentage(ack_value());
        }

        if (ack_seen("Time left:"))
        {
          setPrintRemainingTime(ack_value() * 60);
          setTimeFromSlicer(true);  // disable parsing remaning time from gcode comments

          if (getPrintProgressSource() < PROG_TIME && infoSettings.prog_source == 1)
            setPrintProgressSource(PROG_TIME);
        }
      }
    }

    //----------------------------------------
    // Tuning parsed responses
    //----------------------------------------

    // parse and store build volume size
    else if (ack_seen("work:"))
    {
      if (ack_continue_seen("min:"))
      {
        if (ack_continue_seen("x:")) infoSettings.machine_size_min[X_AXIS] = ack_value();
        if (ack_continue_seen("y:")) infoSettings.machine_size_min[Y_AXIS] = ack_value();
        if (ack_continue_seen("z:")) infoSettings.machine_size_min[Z_AXIS] = ack_value();
      }

      if (ack_continue_seen("max:"))
      {
        if (ack_continue_seen("x:")) infoSettings.machine_size_max[X_AXIS] = ack_value();
        if (ack_continue_seen("y:")) infoSettings.machine_size_max[Y_AXIS] = ack_value();
        if (ack_continue_seen("z:")) infoSettings.machine_size_max[Z_AXIS] = ack_value();
      }
    }
    // parse M48, repeatability test
    else if (ack_starts_with("Mean:"))
    {
      char tmpMsg[100];

      sprintf(tmpMsg, "Mean: %0.5f", ack_value());

      if (ack_continue_seen("Min: "))
        sprintf(&tmpMsg[strlen(tmpMsg)], "\nMin: %0.5f", ack_value());
      if (ack_continue_seen("Max: "))
        sprintf(&tmpMsg[strlen(tmpMsg)], "\nMax: %0.5f", ack_value());
      if (ack_continue_seen("Range: "))
        sprintf(&tmpMsg[strlen(tmpMsg)], "\nRange: %0.5f", ack_value());

      popupReminder(DIALOG_TYPE_INFO, (uint8_t *)"Repeatability Test", (uint8_t *)tmpMsg);
    }
    // parse M48, standard deviation
    else if (ack_seen("Standard Deviation: "))
    {
      char tmpMsg[100];
      char * dialogMsg = (char *)getDialogMsgStr();

      if (memcmp(dialogMsg, "Mean: ", 6) == 0)
      {
        levelingSetProbedPoint(-1, -1, ack_value());  // save probed Z value
        sprintf(tmpMsg, "%s\nStandard Deviation: %0.5f", dialogMsg, ack_value());

        popupReminder(DIALOG_TYPE_INFO, (uint8_t *)"Repeatability Test", (uint8_t *)tmpMsg);
      }
    }
    // parse and store M211 or M503, software endstops state (e.g. from Probe Offset, MBL, Mesh Editor menus)
    else if (ack_starts_with("M211") || ack_seen("Soft endstops"))
    {
      uint8_t curValue = infoMachineSettings.softwareEndstops;
      infoMachineSettings.softwareEndstops = ack_continue_seen("ON");

      if (curValue != infoMachineSettings.softwareEndstops)  // send a notification only if status is changed
        addToast(DIALOG_TYPE_INFO, ack_cache);
    }
    // parse M303, PID autotune finished message
    else if (ack_starts_with("PID Autotune finished"))
    {
      pidUpdateStatus(PID_SUCCESS);
    }
    // parse M303, PID autotune failed message
    else if (ack_starts_with("PID Autotune failed"))
    {
      pidUpdateStatus(PID_FAILED);
    }
    // parse M303, PID autotune finished message in case of Smoothieware
    else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("PID Autotune Complete!"))
    {
      //ack_index += 84; -> need length check
      pidUpdateStatus(PID_SUCCESS);
    }
    // parse M303, PID autotune failed message in case of Smoothieware
    else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("// WARNING: Autopid did not resolve within"))
    {
      pidUpdateStatus(PID_FAILED);
    }
    // parse M306, model predictive temperature control tuning end message (interrupted or finished)
    else if (ack_seen("MPC Autotune"))
    {
      if (ack_continue_seen("finished"))
        setMpcTuningResult(FINISHED);
      else if (ack_continue_seen("interrupted"))
        setMpcTuningResult(INTERRUPTED);
    }
    // parse and store M355, case light message
    else if (ack_seen("Case light:"))
    {
      if (ack_continue_seen("OFF"))
      {
        caseLightSetState(false);
      }
      else
      {
        caseLightSetState(true);
        caseLightSetPercent(ack_value());
      }
    }
    // parse and store M420 V1 T1, mesh data (e.g. from Mesh Editor menu)
    //
    // IMPORTANT: It must be placed before the following keywords:
    //            1) echo:Bed Leveling
    //            2) mesh. Z offset:
    //
    else if (meshIsWaitingData())
    {
      meshUpdateData(ack_cache);  // update mesh data
    }
    // parse and store M420 V1 T1 or G29 S0 (mesh. Z offset:) or M503 (G29 S4 Zxx), MBL Z offset value (e.g. from Babystep menu)
    else if (ack_seen("mesh. Z offset:") || ack_seen("G29 S4 Z"))
    {
      setParameter(P_MBL_OFFSET, 0, ack_value());
    }
    // parse and store M290 (Probe Offset) or M503 (M851), probe offset value (e.g. from Babystep menu) and
    // X an Y probe offset for LevelCorner position limit
    else if (ack_seen("Probe Offset") || ack_starts_with("M851"))
    {
      if (ack_seen("X")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z") || (ack_seen("Z:"))) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Z, ack_value());
    }
    // parse G29 (ABL) + M118, ABL completed message (ABL, BBL, UBL) (e.g. from ABL menu)
    else if (ack_starts_with("ABL Completed"))
    {
      ablUpdateStatus(true);
    }
    // parse G29 (MBL), MBL completed message (e.g. from MBL menu)
    else if (ack_seen("Mesh probing done"))
    {
      mblUpdateStatus(true);
    }
    // parse G30, feedback to get the 4 corners Z value returned by Marlin for LevelCorner menu
    else if (ack_seen("Bed X: "))
    {
      float x = ack_value();
      float y = 0;

      if (ack_continue_seen("Y: "))
        y = ack_value();

      if (ack_continue_seen("Z: "))
        levelingSetProbedPoint(x, y, ack_value());  // save probed Z value
    }
    #if DELTA_PROBE_TYPE != 0
      // parse and store Delta calibration settings
      else if (ack_seen("Calibration OK"))
      {
        BUZZER_PLAY(SOUND_SUCCESS);

        if (infoMachineSettings.EEPROM == 1)
          popupDialog(DIALOG_TYPE_SUCCESS, LABEL_DELTA_CONFIGURATION, LABEL_EEPROM_SAVE_INFO,
                      LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
        else
          popupReminder(DIALOG_TYPE_SUCCESS, LABEL_DELTA_CONFIGURATION, LABEL_PROCESS_COMPLETED);
      }
    #endif

    //----------------------------------------
    // Parameter / M503 / M115 parsed responses
    //----------------------------------------

    // parse and store filament diameter
    else if (ack_starts_with("M200"))
    {
      if (ack_starts_with("M200 S") || ack_seen("D0")) setParameter(P_FILAMENT_DIAMETER, 0, ack_value());

      uint8_t i = (ack_seen("T")) ? ack_value() : 0;

      if (ack_seen("D")) setParameter(P_FILAMENT_DIAMETER, 1 + i, ack_value());

      if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      {
        // filament_diameter > 0.01 to enable volumetric extrusion. Otherwise (<= 0.01), disable volumetric extrusion
        setParameter(P_FILAMENT_DIAMETER, 0, getParameter(P_FILAMENT_DIAMETER, 1) > 0.01f ? 1 : 0);
      }
    }
    // parse and store axis steps-per-unit (steps/mm) (M92), max acceleration (units/s2) (M201) and max feedrate (units/s) (M203)
    else if (ack_starts_with("M92 ") || ack_starts_with("M201") || ack_starts_with("M203"))  // "M92 " to not trigger if "M928" received
    {
      PARAMETER_NAME param = P_STEPS_PER_MM;  // default value

      // using consecutive "if" instead of "if else if" on the following two lines just to reduce code
      // instead of optimizing performance (code typically not executed during a print)
      if (ack_starts_with("M201")) param = P_MAX_ACCELERATION;
      if (ack_starts_with("M203")) param = P_MAX_FEED_RATE;

      if (ack_seen("X")) setParameter(param, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(param, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(param, AXIS_INDEX_Z, ack_value());

      uint8_t i = (ack_seen("T")) ? ack_value() : 0;

      if (ack_seen("E")) setParameter(param, AXIS_INDEX_E0 + i, ack_value());
    }
    // parse and store acceleration (units/s2)
    else if (ack_starts_with("M204 P"))
    {
                         setParameter(P_ACCELERATION, 0, ack_value());
      if (ack_seen("R")) setParameter(P_ACCELERATION, 1, ack_value());
      if (ack_seen("T")) setParameter(P_ACCELERATION, 2, ack_value());
    }
    // parse and store advanced settings
    else if (ack_starts_with("M205"))
    {
      if (ack_seen("X")) setParameter(P_JERK, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(P_JERK, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(P_JERK, AXIS_INDEX_Z, ack_value());
      if (ack_seen("E")) setParameter(P_JERK, AXIS_INDEX_E0, ack_value());
      if (ack_seen("J")) setParameter(P_JUNCTION_DEVIATION, 0, ack_value());
    }
    // parse and store home offset (M206) and hotend offset (M218)
    else if (ack_starts_with("M206 X") || ack_starts_with("M218 T1 X"))
    {
      PARAMETER_NAME param = ack_starts_with("M206") ? P_HOME_OFFSET : P_HOTEND_OFFSET;

      if (ack_seen("X")) setParameter(param, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(param, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(param, AXIS_INDEX_Z, ack_value());
    }
    // parse and store FW retraction (M207) and FW recover (M208)
    else if (ack_starts_with("M207 S") || ack_starts_with("M208 S"))
    {
      PARAMETER_NAME param = ack_starts_with("M207") ? P_FWRETRACT : P_FWRECOVER;

      if (ack_seen("S")) setParameter(param, 0, ack_value());
      if (ack_seen("W")) setParameter(param, 1, ack_value());
      if (ack_seen("F")) setParameter(param, 2, ack_value());

      if (param == P_FWRETRACT)
      {
        if (ack_seen("Z")) setParameter(param, 3, ack_value());
      }
      else  // P_FWRECOVER
      {
        if (ack_seen("R")) setParameter(param, 3, ack_value());
      }
    }
    // parse and store auto retract
    else if (ack_starts_with("M209 S"))
    {
      setParameter(P_AUTO_RETRACT, 0, ack_value());
    }
    // parse and store hotend PID (M301) and bed PID (M304)
    else if (ack_starts_with("M301") || ack_starts_with("M304"))
    {
      PARAMETER_NAME param = ack_starts_with("M301") ? P_HOTEND_PID : P_BED_PID;

      if (ack_seen("P")) setParameter(param, 0, ack_value());
      if (ack_seen("I")) setParameter(param, 1, ack_value());
      if (ack_seen("D")) setParameter(param, 2, ack_value());
    }
    // parse and store model predictive temperature control (only for Marlin)
    else if (ack_starts_with("M306") && infoMachineSettings.firmwareType == FW_MARLIN)
    {
      if (ack_continue_seen("E"))
      {
        uint8_t index = ack_value();

        if (ack_continue_seen("P"))
          setMpcHeaterPower(index, ack_value());

        if (ack_continue_seen("H"))
          setMpcFilHeatCapacity(index, ack_value());
      }
    }
    // parse and store input shaping parameters (only for Marlin)
    else if (ack_starts_with("M593") && infoMachineSettings.firmwareType == FW_MARLIN)
    {
      // M593 accepts its parameters in any order,
      // if both X and Y axis are missing than the rest
      // of the parameters are referring to each axis

      enum
      {
        SET_NONE = 0B00,
        SET_X = 0B01,
        SET_Y = 0B10,
        SET_BOTH = 0B11
      } setAxis = SET_NONE;

      float pValue;

      if (ack_seen("X")) setAxis |= SET_X;
      if (ack_seen("Y")) setAxis |= SET_Y;
      if (setAxis == SET_NONE) setAxis = SET_BOTH;

      if (ack_seen("F"))
      {
        pValue = ack_value();

        if (setAxis & SET_X) setParameter(P_INPUT_SHAPING, 0, pValue);
        if (setAxis & SET_Y) setParameter(P_INPUT_SHAPING, 2, pValue);
      }

      if (ack_seen("D"))
      {
        pValue = ack_value();

        if (setAxis & SET_X) setParameter(P_INPUT_SHAPING, 1, pValue);
        if (setAxis & SET_Y) setParameter(P_INPUT_SHAPING, 3, pValue);
      }
    }
    // parse and store Delta configuration / Delta tower angle (M665) and Delta endstop adjustments (M666)
    //
    // IMPORTANT: It must be placed before the following keywords:
    //            1) M420 S
    //
    else if (ack_starts_with("M665") || ack_starts_with("M666"))
    {
      PARAMETER_NAME param = ack_starts_with("M665") ? P_DELTA_TOWER_ANGLE : P_DELTA_ENDSTOP;

      if (param < P_DELTA_ENDSTOP)  // options not supported by M666
      {
        if (ack_seen("H")) setParameter(P_DELTA_CONFIGURATION, 0, ack_value());
        if (ack_seen("S")) setParameter(P_DELTA_CONFIGURATION, 1, ack_value());
        if (ack_seen("R")) setParameter(P_DELTA_CONFIGURATION, 2, ack_value());
        if (ack_seen("L")) setParameter(P_DELTA_CONFIGURATION, 3, ack_value());
        if (ack_seen("A")) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_X, ack_value());
        if (ack_seen("B")) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_Y, ack_value());
        if (ack_seen("C")) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_Z, ack_value());
      }

      if (ack_seen("X")) setParameter(param, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(param, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(param, AXIS_INDEX_Z, ack_value());
    }
    // parse and store ABL on/off state & Z fade value on M503
    else if (ack_starts_with("M420 S"))
    {
                                  setParameter(P_ABL_STATE, 0, ack_value());
      if (ack_continue_seen("Z")) setParameter(P_ABL_STATE, 1, ack_value());
    }
    // parse and store TMC stepping mode
    else if (ack_seen("Driver stepping mode:"))  // poll stelthchop settings separately
    {
      storeCmd("M569\n");
    }
    else if (ack_seen("driver mode:"))
    {
      float isStealthChop = ack_continue_seen("stealthChop");  // boolean type value also casted to float type
      STEPPER_INDEX stepperIndex = 0;

      if (ack_seen("X")) stepperIndex = STEPPER_INDEX_X;
      else if (ack_seen("Y")) stepperIndex = STEPPER_INDEX_Y;
      else if (ack_seen("Z")) stepperIndex = STEPPER_INDEX_Z;
      else if (ack_seen("E")) stepperIndex = STEPPER_INDEX_E0;

      if (stepperIndex < STEPPER_INDEX_E0)  // if "X", "X1", "X2", "Y", "Y1", "Y2", "Z", "Z1", "Z2", "Z3", "Z4"
      {
        if (ack_value() > 0)  // if "X"->0, "X1"->0, "X2"->1, "Y"->2, "Y1"->2, "Y2"->3, "Z"->4, "Z1"->4, "Z2"->5, "Z3"->6, "Z4"->7
          stepperIndex += ack_value() - 1;
      }

      setParameter(P_STEALTH_CHOP, stepperIndex, isStealthChop);
    }
    // parse and store linear advance factor
    else if (ack_starts_with("M900"))
    {
      uint8_t i = (ack_seen("T")) ? ack_value() : 0;

      if (ack_seen("K")) setParameter(P_LIN_ADV, i, ack_value());
    }
    // parse and store stepper motor current (M906), TMC hybrid threshold speed (M913) and TMC bump sensitivity (M914)
    else if (ack_starts_with("M906") || ack_starts_with("M913") || ack_starts_with("M914"))
    {
      PARAMETER_NAME param = P_CURRENT;  // default value

      // using consecutive "if" instead of "if else if" on the following two lines just to reduce code
      // instead of optimizing performance (code typically not executed during a print)
      if (ack_starts_with("M913")) param = P_HYBRID_THRESHOLD;
      if (ack_starts_with("M914")) param = P_BUMPSENSITIVITY;

      int8_t stepperIndex = (ack_seen("I")) ? ack_value() : 0;

      // if index is missing or set to -1 (meaning all indexes) then it must be converted to 0
      // to make sure array index is never negative
      if (stepperIndex < 0)
        stepperIndex = 0;

      // for M913 and M914, provided index is:
      //   1->"X1", 2->"X2", 1->"Y1", 2->"Y2", 1->"Z1", 2->"Z2", 3->"Z3", 4->"Z4"
      // and it must be converted to:
      //   0->"X1", 1->"X2", 0->"Y1", 1->"Y2", 0->"Z1", 1->"Z2", 2->"Z3", 3->"Z4"
      // to make sure array index is properly accessed
      if (param > P_CURRENT && stepperIndex > 0)
        stepperIndex--;

      if (ack_seen("X")) setParameter(param, STEPPER_INDEX_X + stepperIndex, ack_value());
      if (ack_seen("Y")) setParameter(param, STEPPER_INDEX_Y + stepperIndex, ack_value());
      if (ack_seen("Z")) setParameter(param, STEPPER_INDEX_Z + stepperIndex, ack_value());

      if (param < P_BUMPSENSITIVITY)  // T and E options not supported by M914
      {
        stepperIndex = (ack_seen("T")) ? ack_value() : 0;

        // if index is missing or set to -1 (meaning all indexes) then it must be converted to 0
        // to make sure array index is never negative
        if (stepperIndex < 0)
          stepperIndex = 0;

        if (ack_seen("E")) setParameter(param, STEPPER_INDEX_E0 + stepperIndex, ack_value());
      }
    }
    // parse and store ABL type if auto-detect is enabled
    #if BED_LEVELING_TYPE == 1
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
      uint8_t * string = (uint8_t *)&ack_cache[ack_index];
      uint16_t string_start = ack_index;
      uint16_t string_end = string_start;

      if (ack_continue_seen("Marlin"))
        setupMachine(FW_MARLIN);
      else if (ack_continue_seen("RepRapFirmware"))
        setupMachine(FW_REPRAPFW);
      else if (ack_continue_seen("Smoothieware"))
        setupMachine(FW_SMOOTHIEWARE);
      else
        setupMachine(FW_UNKNOWN);

      if (ack_seen("FIRMWARE_URL:"))  // for Smoothieware
        string_end = ack_index - sizeof("FIRMWARE_URL:");
      else if (ack_seen("SOURCE_CODE_URL:"))  // for Marlin
        string_end = ack_index - sizeof("SOURCE_CODE_URL:");

      infoSetFirmwareName(string, string_end - string_start);  // set firmware name

      if (ack_seen("MACHINE_TYPE:"))
      {
        string = (uint8_t *)&ack_cache[ack_index];
        string_start = ack_index;

        if (ack_seen("EXTRUDER_COUNT:"))
        {
          if (MIXING_EXTRUDER == 0)
            infoSettings.ext_count = ack_value();

          string_end = ack_index - sizeof("EXTRUDER_COUNT:");
        }

        infoSetMachineType(string, string_end - string_start);  // set firmware name
      }
    }
    else if (ack_starts_with("Cap:"))
    {
      if (ack_continue_seen("EEPROM:"))
      {
        infoMachineSettings.EEPROM = ack_value();
      }
      else if (ack_continue_seen("AUTOREPORT_TEMP:"))
      {
        infoMachineSettings.autoReportTemp = ack_value();

        if (infoMachineSettings.autoReportTemp)
          storeCmd("M155 S%u\n", heatGetUpdateSeconds());
      }
      else if (ack_continue_seen("AUTOREPORT_POS:"))
      {
        infoMachineSettings.autoReportPos = ack_value();
      }
      else if (ack_continue_seen("AUTOLEVEL:") && infoMachineSettings.leveling == BL_DISABLED)
      {
        infoMachineSettings.leveling = ack_value();
      }
      else if (ack_continue_seen("Z_PROBE:"))
      {
        infoMachineSettings.zProbe = ack_value();
      }
      else if (ack_continue_seen("LEVELING_DATA:"))
      {
        infoMachineSettings.levelingData = ack_value();
      }
      else if (ack_continue_seen("SOFTWARE_POWER:"))
      {
        infoMachineSettings.softwarePower = ack_value();
      }
      else if (ack_continue_seen("TOGGLE_LIGHTS:"))
      {
        infoMachineSettings.toggleLights = ack_value();
      }
      else if (ack_continue_seen("CASE_LIGHT_BRIGHTNESS:"))
      {
        infoMachineSettings.caseLightsBrightness = ack_value();
      }
      else if (ack_continue_seen("EMERGENCY_PARSER:"))
      {
        infoMachineSettings.emergencyParser = ack_value();
      }
      else if (ack_continue_seen("PROMPT_SUPPORT:"))
      {
        infoMachineSettings.promptSupport = ack_value();
      }
      else if (ack_continue_seen("SDCARD:") && infoSettings.onboard_sd == AUTO)
      {
        infoMachineSettings.onboardSD = ack_value();
      }
      else if (ack_continue_seen("MULTI_VOLUME:"))
      {
        infoMachineSettings.multiVolume = ack_value();
      }
      else if (ack_continue_seen("AUTOREPORT_SD_STATUS:"))
      {
        infoMachineSettings.autoReportSDStatus = ack_value();
      }
      else if (ack_continue_seen("LONG_FILENAME:") && infoSettings.long_filename == AUTO)
      {
        infoMachineSettings.longFilename = ack_value();
      }
      else if (ack_continue_seen("BABYSTEPPING:"))
      {
        infoMachineSettings.babyStepping = ack_value();
      }
      else if (ack_continue_seen("BUILD_PERCENT:"))  // M73 support. Required "SET_PROGRESS_MANUALLY" in Marlin
      {
        infoMachineSettings.buildPercent = ack_value();
      }
      else if (ack_continue_seen("CHAMBER_TEMPERATURE:") && infoSettings.chamber_en == DISABLED)  // auto-detect only if set to disabled
      {
        infoSettings.chamber_en = ack_value();
      }
    }

    //----------------------------------------
    // Error / echo parsed responses
    //----------------------------------------

    // parse error messages
    else if (ack_seen(magic_error))
    {
      ackPopupInfo(magic_error);
    }
    // parse echo messages
    else if (ack_starts_with(magic_echo))
    {
      // parse and store M401 H, BLTouch HighSpeed mode
      if (ack_continue_seen("BLTouch HS mode"))
      {
        setHSmode(ack_continue_seen("ON") ? HS_ON : HS_OFF);
      }
      // parse and store M420 V1 T1 or M420 Sxx, ABL state (e.g. from Bed Leveling menu)
      else if (ack_continue_seen("Bed Leveling"))
      {
        setParameter(P_ABL_STATE, 0, ack_continue_seen("ON") ? ENABLED : DISABLED);
      }
      else if (ack_continue_seen("Fade Height"))
      {
        setParameter(P_ABL_STATE, 1, ack_value());
      }
      // newer Marlin (e.g. 2.0.9.3) returns this ACK for M900 command
      else if (ack_continue_seen("Advance K="))
      {
        setParameter(P_LIN_ADV, heatGetToolIndex(), ack_value());
      }
      else if (!processKnownEcho())  // if no known echo was found and processed, then popup the echo message
      {
        ackPopupInfo(magic_echo);
      }
    }
    else if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
    {
      if (ack_seen("ZProbe triggered before move"))  // smoothieboard ZProbe triggered before move, aborting command
      {
        ackPopupInfo("ZProbe triggered before move.\nAborting Print!");
      }
      // parse and store volumetric extrusion M200 response of Smoothieware
      else if (ack_seen("Volumetric extrusion is disabled"))
      {
        setParameter(P_FILAMENT_DIAMETER, 0, 0);
        setParameter(P_FILAMENT_DIAMETER, 1, 0.0f);
      }
      // parse and store volumetric extrusion M200 response of Smoothieware
      else if (ack_seen("Filament Diameter:"))
      {
        setParameter(P_FILAMENT_DIAMETER, 1, ack_value());
        // filament_diameter > 0.01 to enable volumetric extrusion. Otherwise (<= 0.01), disable volumetric extrusion
        setParameter(P_FILAMENT_DIAMETER, 0, getParameter(P_FILAMENT_DIAMETER, 1) > 0.01f ? 1 : 0);
      }
    }
    // check for motherboard reset (external, software, etc)
    else if (ack_seen("Reset"))
    {
      if (ack_seen("External") || ack_seen("Software") || ack_seen("Watchdog") || ack_seen("Brown out"))
      {
        // proceed to reset the command queue, host status, fan speeds and load default machine settings.
        // These functions will also trigger the query of temperatures which together with the resets
        // done will also trigger the query of the motherboard capabilities and settings. It is necessary
        // to do so because after the motherboard reset things might have changed (ex. FW update by M997)

        clearCmdQueue();
        InfoHost_Init(false);
        initMachineSettings();
        fanResetSpeed();
        coordinateSetKnown(false);
      }
    }

  parse_end:
    if (!avoid_terminal && MENU_IS(menuTerminal))
      terminalCache(ack_cache, ack_len, ack_port_index, SRC_TERMINAL_ACK);

    #ifdef SERIAL_PORT_2
      if (ack_port_index == PORT_1)
      {
        if (infoHost.tx_count == 0 && !ack_starts_with("ok"))
        { // if the ACK message is not related to a gcode originated by the TFT and it is not "ok", it is a spontaneous
          // ACK message so pass it to all the supplementary serial ports (since these messages came unrequested)
          Serial_Forward(SUP_PORTS, ack_cache);
        }
      }
      else
      { // if the ACK message is related to a gcode originated by a supplementary serial port,
        // forward the message to that supplementary serial port
        Serial_Forward(ack_port_index, ack_cache);

        // if no pending gcode (all "ok" have been received), reset ACK port index to avoid wrong relaying (in case no
        // more commands will be sent by interfaceCmd) of any successive spontaneous ACK message
        if (infoHost.tx_count == 0)
          ack_port_index = PORT_1;
      }
    #endif
  }
}
