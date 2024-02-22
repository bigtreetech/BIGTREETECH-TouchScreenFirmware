#include "Mainboard_FlowControl.h"
#include "includes.h"
#include "RRFStatusControl.h"

CLOCKS mcuClocks;
PRIORITY_COUNTER priorityCounter;
HOST infoHost;
MENU infoMenu;

void resetInfoQueries(void)
{
  fanResetSpeed();
  coordinateSetKnown(false);
  setRunoutAlarmFalse();
}

void resetPendingQueries(void)
{
  abortRequestCommandInfo();             // abort pending command query

  ctrlFanQueryClearSendingWaiting();     // clear sending waiting for controller fan query
  speedQueryClearSendingWaiting();       // clear sending waiting for speed query
  coordinateQueryClearSendingWaiting();  // clear sending waiting for coordinate query

  heatClearSendingWaiting();             // clear sending waiting for temperature query
  printClearSendingWaiting();            // clear sending waiting for printing query

  #ifdef FIL_RUNOUT_PIN
    FIL_PosE_ClearSendingWaiting();      // clear sending waiting for position query
  #endif
}

// non-UI background loop tasks
void loopBackEnd(void)
{
  UPD_SCAN_RATE();  // debug monitoring KPI

  // handle a print from TFT media, if any
  loopPrintFromTFT();

  // parse and send gcode commands in the queue
  sendQueueCmd();

  // parse the received slave response information
  parseAck();

  // retrieve and store (in command queue) the gcodes received from other UART, such as ESP3D etc...
  #ifdef SERIAL_PORT_2
    Serial_GetFromUART();
  #endif

  // handle USB communication
  #ifdef USB_FLASH_DRIVE_SUPPORT
    USB_LoopProcess();
  #endif

  if ((priorityCounter.be++ % BE_PRIORITY_DIVIDER) != 0)  // a divider value of 16 -> run 6% of the time only
    return;

  // handle ACK message timeout
  if (InfoHost_HandleAckTimeout())  // if ACK message timeout, unlock any pending query waiting for an update
    resetPendingQueries();

  // fan speed monitor
  loopCheckFan();

  // speed & flow monitor
  loopCheckSpeed();

  if (infoMachineSettings.firmwareType != FW_REPRAPFW)
    loopCheckHeater();  // temperature monitor
  else
    rrfStatusQuery();  // query RRF status

  // handle a print from (remote) onboard media, if any
  if (infoMachineSettings.onboardSD == ENABLED)
    loopPrintFromOnboard();

  // check filament runout status
  #ifdef FIL_RUNOUT_PIN
    FIL_BE_CheckRunout();
  #endif

  // check changes in encoder steps
  #if LCD_ENCODER_SUPPORT
    #ifdef HAS_EMULATOR
      if (MENU_IS_NOT(menuMarlinMode))
    #endif
    {
      LCD_Enc_CheckSteps();
    }
  #endif

  // check mode switching
  #ifdef HAS_EMULATOR
    Mode_CheckSwitching();
  #endif

  // handle screenshot capture
  #ifdef SCREEN_SHOT_TO_SD
    loopScreenShot();
  #endif

  // check if Back is pressed and held
  #ifdef SMART_HOME
    loopCheckBackPress();
  #endif

  // check LCD screen dimming
  #ifdef LCD_LED_PWM_CHANNEL
    LCD_CheckDimming();
  #endif

  // check LED Event
  if (GET_BIT(infoSettings.general_settings, INDEX_EVENT_LED) == 1)
    LED_CheckEvent();
}

// UI-related background loop tasks
void loopFrontEnd(void)
{
  // check if volume source (SD/USB) insert
  loopVolumeSource();

  // loop to check and run toast messages
  loopToast();

  // if there is a message in the status bar, timed clear
  loopReminderManage();

  // busy Indicator clear
  loopBusySignClear();

  // check update temperature status
  loopTemperatureStatus();

  // loop for filament runout detection
  #ifdef FIL_RUNOUT_PIN
    FIL_FE_CheckRunout();
  #endif

  // loop for popup menu
  loopPopup();
}

void loopProcess(void)
{
  loopBackEnd();

  if ((priorityCounter.fe++ % FE_PRIORITY_DIVIDER) != 0)  // a divider value of 16 -> run 6% of the time only
    return;

  loopFrontEnd();
}

void menuDummy(void)
{
  CLOSE_MENU();
}

void loopProcessAndGUI(void)
{
  uint8_t curMenu = infoMenu.cur;

  loopProcess();

  if (infoMenu.cur != curMenu)  // if a user interaction is needed (e.g. dialog box), handle it
  {
    (*infoMenu.menu[infoMenu.cur])();  // handle user interaction

    if (MENU_IS_NOT(menuDummy))  // avoid to nest menuDummy menu type
      OPEN_MENU(menuDummy);      // load a dummy menu just to force the redraw of the underlying menu (caller menu)
  }
}

void InfoHost_Init(bool isConnected)
{
  infoHost.target_tx_slots = infoSettings.tx_slots;
  infoHost.tx_slots = 1;  // set to 1 just to allow a soft start
  infoHost.tx_count = 0;
  infoHost.rx_timestamp = OS_GetTimeMs();
  infoHost.connected = isConnected;
  infoHost.listening_mode = false;  // temporary disable listening mode. It will be later set by InfoHost_UpdateListeningMode()
  infoHost.status = HOST_STATUS_IDLE;

  if (!isConnected)
  {
    clearCmdQueue();
    resetPendingQueries();
    resetInfoQueries();

    setReminderMsg(LABEL_UNCONNECTED, SYS_STATUS_DISCONNECTED);  // set the no printer attached reminder
  }
}

void InfoHost_HandleAckOk(int16_t target_tx_slots)
{
  // the following check should always be matched unless:
  // - an ACK message not related to a gcode originated by the TFT is received
  // - an ACK message for an out of band gcode (e.g. emergency gcode) is received
  //
  if (infoHost.tx_count > 0)
    infoHost.tx_count--;

  // NOTE: the following code always allows to align infoHost.tx_slots even in case of switching ON/OFF
  //       the ADVANCED_OK feature in TFT and/or in case infoHost.tx_slots is beeing also managed by
  //       Marlin (if ADVANCED_OK is enabled in Marlin firmware)
  //

  // if ADVANCED_OK is disabled in TFT
  //
  if (GET_BIT(infoSettings.general_settings, INDEX_ADVANCED_OK) == 0)
  {
    infoHost.tx_slots = 1;
  }
  //
  // if ADVANCED_OK is enabled in TFT or Marlin
  //
  else if (target_tx_slots >= 0)
  {
    // UPPER LIMITER
    //
    // the following check is matched in case:
    // - ADVANCED_OK is enabled in TFT. infoSettings.tx_slots for static ADVANCED_OK configured in TFT is used
    // - ADVANCED_OK is enabled in Marlin but the mainboard reply (target_tx_slots) is out of sync (above) with the current
    //   pending gcodes (it happens sometimes). infoSettings.tx_slots for Marlin ADVANCED_OK detected at TFT boot is used
    //
    if (target_tx_slots + infoHost.tx_count >= infoSettings.tx_slots)
      infoHost.tx_slots = infoSettings.tx_slots - infoHost.tx_count;
    //
    // LOWER LIMITER (only for Marlin ADVANCED_OK)
    //
    // if printing from onboard media target_tx_slots is always reported as 0 by Marlin even if there are no pending gcodes
    // so just set infoHost.tx_slots to 1 to allow the transmission of one gcode per time avoiding a possible TFT freeze
    //
    else if (target_tx_slots != 0 || infoHost.tx_count != 0)  // if not printing from onboard media
      infoHost.tx_slots = target_tx_slots;
    else                                                      // if printing from onboard media
      infoHost.tx_slots = 1;

    infoHost.target_tx_slots = infoHost.tx_slots;  // set new current target
  }
  //
  // if generic OK response handling (e.g. temperature response), increment the current value up to current target
  //
  else
  {
    // UPPER AND LOWER LIMITER
    //
    // limit the current value up to current target or to 1 if current target was set to 0 and there are no more pending gcodes
    //
    if (infoHost.tx_slots < infoHost.target_tx_slots || (infoHost.tx_slots == 0 && infoHost.tx_count == 0))
      infoHost.tx_slots++;
  }
}

bool InfoHost_HandleAckTimeout(void)
{
  if (OS_GetTimeMs() - infoHost.rx_timestamp < ACK_TIMEOUT || infoHost.tx_count == 0)  // if no timeout or no pending gcode
    return false;

  infoHost.rx_timestamp = OS_GetTimeMs();  // update timestamp

  InfoHost_HandleAckOk(HOST_SLOTS_GENERIC_OK);  // release pending gcode

  //addNotification(DIALOG_TYPE_ERROR, "ACK timed out", "Pending gcode released", true);

  return true;
}

void InfoHost_UpdateAckTimestamp(void)
{
  infoHost.rx_timestamp = OS_GetTimeMs();
}

void InfoHost_UpdateListeningMode(void)
{
  infoHost.listening_mode = (GET_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE) == 1);

  if (infoHost.listening_mode)
    setReminderMsg(LABEL_LISTENING, SYS_STATUS_LISTENING);  // if TFT in listening mode, display a reminder message
}
