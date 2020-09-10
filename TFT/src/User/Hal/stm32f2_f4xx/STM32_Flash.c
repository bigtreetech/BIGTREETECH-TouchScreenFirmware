#include "STM32_Flash.h"
#include "my_misc.h"

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
#ifdef MKS_35_V1_0

#define SIGN_ADDRESS (0x08040000)  //reserve tha last 128k for user params or we damage the boot loader on this board 
//use darkspr1te alternative small loader to fix this issue and return to using 16kb lower flash for param storage or switch to I2C 

//if we use getsector routine it returns this value anyway, so FLASH_SECTOR is not used 
#define FLASH_SECTOR FLASH_Sector_6

#else
#define SIGN_ADDRESS (0x08004000)  //reserve the second sector (16KB) to save user parameters
#define FLASH_SECTOR FLASH_Sector_1
#endif

static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_Sector_11;  
  }

  return sector;
}

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
  FLASH_EraseSector(GetSector(SIGN_ADDRESS), VoltageRange_1);
  for(i = 0; i < len; i++)
  {
    FLASH_ProgramByte(SIGN_ADDRESS + i, data[i]);
  }
  FLASH_Lock();
}
