#include "HAL_Flash.h"
#include "variants.h"  // for FLASH_Unlock etc.
#include "my_misc.h"

#define SIGN_ADDRESS (0x08040000 - 0x800)  // reserve the last page (2KB) to save user parameters

void HAL_FlashRead(uint8_t * data, uint32_t len)
{
  uint32_t i = 0;

  for (i = 0; i < len; i++)
  {
    data[i] = *(volatile uint8_t *)(SIGN_ADDRESS + i);
  }
}

void HAL_FlashWrite(uint8_t * data, uint32_t len)
{
  uint32_t i = 0;

  FLASH_Unlock();
  FLASH_ErasePage(SIGN_ADDRESS);

  for (i = 0; i < len; i += 2)
  {
    uint16_t data16 = data[i] | (data[MIN(i + 1, len - 1)] << 8);  // stm32f10x needs to write at least 16 bits at a time

    FLASH_ProgramHalfWord(SIGN_ADDRESS + i, data16);
  }

  FLASH_Lock();
}
