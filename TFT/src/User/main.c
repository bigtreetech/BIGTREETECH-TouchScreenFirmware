#include "main.h"
#include "includes.h"

MENU infoMenu;               // Menu structure
HOST infoHost;               // Information interaction with Marlin
CLOCKS mcuClocks;            // System clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2
uint32_t bePriorityCounter;  // Back end priority counter
uint32_t fePriorityCounter;  // Front end priority counter

void InfoHost_Init(bool isConnected)
{
  infoHost.target_tx_slots = infoSettings.tx_slots;
  infoHost.tx_slots = 1;  // set to 1 just to allow a soft start
  infoHost.tx_count = 0;
  infoHost.connected = isConnected;
  infoHost.listening_mode = false;  // temporary disable listening mode. It will be later set by InfoHost_UpdateListeningMode()
  infoHost.status = HOST_STATUS_IDLE;

  if (!isConnected)
    setReminderMsg(LABEL_UNCONNECTED, SYS_STATUS_DISCONNECTED);  // set the no printer attached reminder
}

void InfoHost_UpdateListeningMode(void)
{
  infoHost.listening_mode = (GET_BIT(infoSettings.general_settings, INDEX_LISTENING_MODE) == 1);

  if (infoHost.listening_mode)
    setReminderMsg(LABEL_LISTENING, SYS_STATUS_LISTENING);  // if TFT in listening mode, display a reminder message
}

void InfoHost_HandleOkAck(int16_t target_tx_slots)
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

int main(void)
{
  SystemClockInit();

  SCB->VTOR = VECT_TAB_FLASH;

  HW_Init();

  #if defined(SERIAL_DEBUG_PORT) && defined(SERIAL_DEBUG_ENABLED)
    dbg_print("Main Startup: Generic debug output is enabled.\n");
  #endif

  for (; ;)
  {
    (*infoMenu.menu[infoMenu.cur])();
  }
}
