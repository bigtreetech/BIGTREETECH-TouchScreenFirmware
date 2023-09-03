#include "main.h"
#include "includes.h"

MENU infoMenu;     // Menu structure
HOST infoHost;     // Information interaction with Marlin
CLOCKS mcuClocks;  // System clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2

void InfoHost_Init(bool isConnected)
{
  infoHost.tx_slots = 1;  // set to 1 just to allow a soft start
  infoHost.tx_count = 0;
  infoHost.connected = isConnected;
  infoHost.status = HOST_STATUS_IDLE;
}

void InfoHost_HandleOkAck(int16_t tx_slots)
{
  // the following check should always be matched unless:
  // - an ACK message not related to a gcode originated by the TFT is received
  // - an ACK message for an out of band gcode (e.g. emergency gcode) is received
  // - a bug is present in the TFT code or in the mainboard reply
  //
  if (infoHost.tx_count > 0)
  {
    infoHost.tx_count--;
  }
  else
  {
    // if the following check is matched, a bug is present.
    // Reset infoSettings.tx_slots and infoHost.tx_count to try to avoid a possible TFT freeze
    //
    if (infoHost.tx_slots == 0)
    {
    handle_error:

      #ifdef DEBUG_MONITORING
        BUZZER_PLAY(SOUND_ERROR);

        char str[100];

        sprintf(str, "tx_slots=%d tx_count=%d", infoHost.tx_slots, infoHost.tx_count);
        addNotification(DIALOG_TYPE_ERROR, "OK mismatch", str, false);
      #endif

      infoHost.tx_slots = 1;  // set to 1 just to allow a soft start
      infoHost.tx_count = 0;

      return;  // nothing else to do
    }

    // in case of generic OK response handling such as temperature response
    // (e.g. "ok T:16.13 /0.00 B:16.64 /0.00 @:0 B@:0\n"), nothing else to do
    //
    if (tx_slots == HOST_SLOTS_GENERIC_OK)
      return;  // nothing else to do

    // if here, proceed with the selected OK response handling to update infoHost.tx_slots and infoHost.tx_count
  }

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
  // if ADVANCED_OK is enabled in TFT but not in Marlin, use the value for the static ADVANCED_OK provided by TFT
  //
  else if (tx_slots == HOST_SLOTS_REGULAR_OK)
  {
  handle_static_ok:

    // the following check should always be matched unless a bug is present in the TFT code or in the mainboard reply
    //
    if (infoSettings.tx_slots >= infoHost.tx_count)
      infoHost.tx_slots = infoSettings.tx_slots - infoHost.tx_count;
    // in case of bug, reset infoSettings.tx_slots and infoHost.tx_count to try to avoid a possible TFT freeze
    //
    else
      goto handle_error;
  }
  // if ADVANCED_OK is enabled in both TFT and Marlin, use the value provided by Marlin
  //
  else if (tx_slots >= 0)
  {
    // the following check should always be matched unless the mainboard reply
    // is out of synch with the current pending gcodes (it happens sometimes)
    //
    if (infoSettings.tx_slots >= tx_slots + infoHost.tx_count)
    {
      // if printing from onboard media tx_slots is always reported as 0 by Marlin even if there are no pending gcodes so
      // just set infoHost.tx_slots to 1 to allow the transmission of one gcode per time avoiding a possible TFT freeze
      //
      if (tx_slots != 0 || infoHost.tx_count != 0)
        infoHost.tx_slots = tx_slots;
      // if printing from onboard media
      //
      else
        infoHost.tx_slots = 1;
    }
    // in case of the mainboard reply is out of synch with the current pending gcodes
    //
    else
    {
      goto handle_static_ok;
    }
  }
  // if generic OK response handling (e.g. temperature response), increment the current value
  //
  else
  {
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
