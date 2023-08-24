#include "main.h"
#include "includes.h"

MENU infoMenu;     // Menu structure
HOST infoHost;     // Information interaction with Marlin
CLOCKS mcuClocks;  // System clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2

void initInfoHost(bool isConnected)
{
  infoHost.tx_slots = infoSettings.tx_slots;
  infoHost.tx_count = 0;
  infoHost.connected = isConnected;
  infoHost.status = HOST_STATUS_IDLE;
}

void handleOkAck(void)
{
  // 1 buffer just became available
  infoHost.tx_slots = MIN(infoHost.tx_slots + 1, infoSettings.tx_slots);

  if (infoHost.tx_count > 0)
    infoHost.tx_count--;
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
