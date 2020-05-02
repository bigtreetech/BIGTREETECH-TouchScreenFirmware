#include "STM32_Flash.h"
#include "my_misc.h"
#include "stm32f2xx.h"

/*
Sector 0 0x0800 0000 - 0x0800 3FFF 16 Kbyte
Sector 1 0x0800 4000 - 0x0800 7FFF 16 Kbyte
Sector 2 0x0800 8000 - 0x0800 BFFF 16 Kbyte
Sector 3 0x0800 C000 - 0x0800 FFFF 16 Kbyte
Sector 4 0x0801 0000 - 0x0801 FFFF 64 Kbyte
Sector 5 0x0802 0000 - 0x0803 FFFF 128 Kbyte //256KByte
Sector 6 0x0804 0000 - 0x0805 FFFF 128 Kbyte
...
...
...
Sector 11 0x080E 0000 - 0x080F FFFF 128 Kbyte
*/

#define SIGN_ADDRESS (0x08004000)  //reserve the second sector (16KB) to save user parameters
#define FLASH_SECTOR FLASH_Sector_1

void STM32_FlashRead(u8 *data, u32 len)
{
  u32 i = 0;
  for(i = 0; i < len; i++)
  {
    data[i] = *(vu8*)(SIGN_ADDRESS + i);
  }
}

void STM32_FlashWrite(u8 *data, u32 len)
{
  u32 i = 0;
  FLASH_Unlock();
  FLASH_EraseSector(FLASH_SECTOR, VoltageRange_1);
  for(i = 0; i < len; i++)
  {
    FLASH_ProgramByte(SIGN_ADDRESS + i, data[i]);
  }
  FLASH_Lock();
}
