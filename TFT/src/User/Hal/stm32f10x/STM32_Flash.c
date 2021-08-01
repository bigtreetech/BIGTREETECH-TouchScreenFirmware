#include "STM32_Flash.h"
#include "my_misc.h"

#define SIGN_ADDRESS (0x08040000 - 0x800)  // reserve the last page (2KB) to save user parameters

void STM32_FlashRead(u8 *data, u32 len)
{
  u32 i = 0;
  for (i = 0; i < len; i++)
  {
    data[i] = *(vu8*)(SIGN_ADDRESS + i);
  }
}

void STM32_FlashWrite(u8 *data, u32 len)
{
  u32 i = 0;
  FLASH_Unlock();
  FLASH_ErasePage(SIGN_ADDRESS);
  for (i = 0; i < len; i += 2)
  {
    u16 data16 = data[i] | (data[MIN(i+1, len-1)] << 8);  // stm32f10x needs to write at least 16 bits at a time
    FLASH_ProgramHalfWord(SIGN_ADDRESS + i, data16);
  }
  FLASH_Lock();
}
