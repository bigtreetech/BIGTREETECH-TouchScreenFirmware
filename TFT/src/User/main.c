#include "includes.h"

HOST infoHost;     // Information interaction with Marlin
MENU infoMenu;     // Menu structure
CLOCKS mcuClocks;  // system clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2

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
