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
  //       the ADVANCED_OK feature on TFT and/or in case infoHost.tx_slots is beeing also managed by
  //       Marlin (if ADVANCED_OK is enabled in Marlin firmware)
  //
  if (GET_BIT(infoSettings.general_settings, INDEX_ADVANCED_OK) == 0)  // if ADVANCED_OK is disabled on TFT
    infoHost.tx_slots = 1;
  else if (tx_slots == -1)  // if ADVANCED_OK is enabled only on TFT, use the value for the static ADVANCED_OK provided by TFT
    infoHost.tx_slots = infoSettings.tx_slots >= infoHost.tx_count ? infoSettings.tx_slots - infoHost.tx_count : 1;
  else if (tx_slots >= 0)   // if ADVANCED_OK is enabled on both TFT and Marlin, use the value provided by Marlin
    infoHost.tx_slots = tx_slots;
  else                      // if < -2 (generic OK response handling (e.g. temperature response)), increment the current value
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
