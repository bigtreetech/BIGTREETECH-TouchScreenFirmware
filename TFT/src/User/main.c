#include "main.h"
#include "includes.h"

int main(void)
{
  #if defined GD32F3XX
    // Required due to enabling interrupt after vector table relocation
    __enable_irq();
  #endif

  SystemClockInit();  // it depends on "variants.h" included in "includes.h"

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
