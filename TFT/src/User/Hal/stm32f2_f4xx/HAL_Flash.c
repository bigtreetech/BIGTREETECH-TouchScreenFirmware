#include "HAL_Flash.h"
#include "variants.h"  // for MKS_TFT35_V1_0, FLASH_Unlock etc.
#include "my_misc.h"

/*
 * Sector 0 0x0800 0000 - 0x0800 3FFF 16 Kbyte
 * Sector 1 0x0800 4000 - 0x0800 7FFF 16 Kbyte
 * Sector 2 0x0800 8000 - 0x0800 BFFF 16 Kbyte
 * Sector 3 0x0800 C000 - 0x0800 FFFF 16 Kbyte
 * Sector 4 0x0801 0000 - 0x0801 FFFF 64 Kbyte
 * Sector 5 0x0802 0000 - 0x0803 FFFF 128 Kbyte  // 256KByte
 * Sector 6 0x0804 0000 - 0x0805 FFFF 128 Kbyte
 * ...
 * ...
 * ...
 * Sector 11 0x080E 0000 - 0x080F FFFF 128 Kbyte
 */

#if defined(MKS_TFT35_V1_0)  // added for MKS TFT 35 V1.0 support. MKS_TFT35_V1_0 bootloader is 48KB, adjust flash mapping
  #define ADDR_FLASH_SECTOR_0  ((uint32_t)0x08000000)  // base @ of sector 0, 16 Kbytes
  #define ADDR_FLASH_SECTOR_1  ((uint32_t)0x08004000)  // base @ of sector 1, 16 Kbytes
  #define ADDR_FLASH_SECTOR_2  ((uint32_t)0x08008000)  // base @ of sector 2, 16 Kbytes
  #define ADDR_FLASH_SECTOR_3  ((uint32_t)0x0800C000)  // base @ of sector 3, 16 Kbytes
  #define ADDR_FLASH_SECTOR_4  ((uint32_t)0x08010000)  // base @ of sector 4, 64 Kbytes
  #define ADDR_FLASH_SECTOR_5  ((uint32_t)0x08020000)  // base @ of sector 5, 128 Kbytes
  #define ADDR_FLASH_SECTOR_6  ((uint32_t)0x08040000)  // base @ of sector 6, 128 Kbytes
  #define ADDR_FLASH_SECTOR_7  ((uint32_t)0x08060000)  // base @ of sector 7, 128 Kbytes
  #define ADDR_FLASH_SECTOR_8  ((uint32_t)0x08080000)  // base @ of sector 8, 128 Kbytes
  #define ADDR_FLASH_SECTOR_9  ((uint32_t)0x080A0000)  // base @ of sector 9, 128 Kbytes
  #define ADDR_FLASH_SECTOR_10 ((uint32_t)0x080C0000)  // base @ of sector 10, 128 Kbytes
  #define ADDR_FLASH_SECTOR_11 ((uint32_t)0x080E0000)  // base @ of sector 11, 128 Kbytes
  #define ADDR_FLASH_SECTOR_12 ((uint32_t)0x08100000)  // base @ of sector 0, 16 Kbytes
  #define ADDR_FLASH_SECTOR_13 ((uint32_t)0x08104000)  // base @ of sector 1, 16 Kbytes
  #define ADDR_FLASH_SECTOR_14 ((uint32_t)0x08108000)  // base @ of sector 2, 16 Kbytes
  #define ADDR_FLASH_SECTOR_15 ((uint32_t)0x0810C000)  // base @ of sector 3, 16 Kbytes
  #define ADDR_FLASH_SECTOR_16 ((uint32_t)0x08110000)  // base @ of sector 4, 64 Kbytes
  #define ADDR_FLASH_SECTOR_17 ((uint32_t)0x08120000)  // base @ of sector 5, 128 Kbytes
  #define ADDR_FLASH_SECTOR_18 ((uint32_t)0x08140000)  // base @ of sector 6, 128 Kbytes
  #define ADDR_FLASH_SECTOR_19 ((uint32_t)0x08160000)  // base @ of sector 7, 128 Kbytes
  #define ADDR_FLASH_SECTOR_20 ((uint32_t)0x08180000)  // base @ of sector 8, 128 Kbytes
  #define ADDR_FLASH_SECTOR_21 ((uint32_t)0x081A0000)  // base @ of sector 9, 128 Kbytes
  #define ADDR_FLASH_SECTOR_22 ((uint32_t)0x081C0000)  // base @ of sector 10, 128 Kbytes
  #define ADDR_FLASH_SECTOR_23 ((uint32_t)0x081E0000)  // base @ of sector 11, 128 Kbytes

  // use darkspr1te alternative small loader to fix this issue and return to using 16kb lower flash for param storage
  // or switch to I2C if we use getsector routine it returns this value anyway, so FLASH_SECTOR is not used
  #define SIGN_ADDRESS (0x08040000)    // reserve the last 128k for user params or we damage the boot loader on this board
  #define FLASH_SECTOR FLASH_Sector_6  // points to an available sector (0x08040000 - 0x0805FFFF)
#else
  #define SIGN_ADDRESS (0x08004000)    // reserve the second sector (16KB) to save user parameters
  #define FLASH_SECTOR FLASH_Sector_1
#endif

#if defined(MKS_TFT35_V1_0)

static inline uint32_t GetSector(uint32_t address)
{
  uint32_t sector = 0;

  if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    sector = FLASH_Sector_0;
  else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    sector = FLASH_Sector_1;
  else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    sector = FLASH_Sector_2;
  else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    sector = FLASH_Sector_3;
  else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    sector = FLASH_Sector_4;
  else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    sector = FLASH_Sector_5;
  else if ((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    sector = FLASH_Sector_6;
  else if ((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    sector = FLASH_Sector_7;
  else if ((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    sector = FLASH_Sector_8;
  else if ((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    sector = FLASH_Sector_9;
  else if ((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    sector = FLASH_Sector_10;
  else if ((address < ADDR_FLASH_SECTOR_12) && (address >= ADDR_FLASH_SECTOR_11))
    sector = FLASH_Sector_11;

  return sector;
}

#endif  // MKS_TFT35_V1_0

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

  #if defined(MKS_TFT35_V1_0)  // added for MKS_TFT35_V1_0 support
    FLASH_EraseSector(GetSector(SIGN_ADDRESS), VoltageRange_1);
  #else
    FLASH_EraseSector(FLASH_SECTOR, VoltageRange_1);
  #endif

  for (i = 0; i < len; i++)
  {
    FLASH_ProgramByte(SIGN_ADDRESS + i, data[i]);
  }

  FLASH_Lock();
}
