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
  if (infoHost.tx_count > 0)
    infoHost.tx_count--;

  // NOTE: the following code always allows to align infoHost.tx_slots even in case of switching ON/OFF
  //       the ADVANCED_OK feature in TFT and/or in case infoHost.tx_slots is beeing also managed by
  //       Marlin (if ADVANCED_OK is enabled in Marlin firmware)
  //

  // if ADVANCED_OK is disabled in TFT
  if (GET_BIT(infoSettings.general_settings, INDEX_ADVANCED_OK) == 0)
    infoHost.tx_slots = 1;
  // if ADVANCED_OK is enabled in TFT but not in Marlin, use the value for the static ADVANCED_OK provided by TFT
  else if (tx_slots == HOST_SLOTS_REGULAR_OK)
  {
    // the following check should always be matched unless a bug is present in the code
    if (infoSettings.tx_slots >= infoHost.tx_count)
    {
      infoHost.tx_slots = infoSettings.tx_slots - infoHost.tx_count;
    }
    // in case of bug, reset infoSettings.tx_slots and infoHost.tx_count to try to avoid a possible TFT freeze
    else
    {
      char str[100];

      sprintf(str, "tx_slots=%d tx_count=%d", infoHost.tx_slots, infoHost.tx_count);
      addNotification(DIALOG_TYPE_ERROR, "OK mismatch", str, false);

      infoHost.tx_slots = 1;  // set to 1 just to allow a soft start
      infoHost.tx_count = 0;
    }
  }
  // if ADVANCED_OK is enabled in both TFT and Marlin, use the value provided by Marlin
  else if (tx_slots >= 0)
    infoHost.tx_slots = tx_slots;
  // if generic OK response handling (e.g. temperature response), increment the current value
  else
    infoHost.tx_slots++;
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
