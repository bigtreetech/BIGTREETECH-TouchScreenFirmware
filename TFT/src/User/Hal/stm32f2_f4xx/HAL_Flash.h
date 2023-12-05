#ifndef _HAL_FLASH_H_
#define _HAL_FLASH_H_

#include "variants.h"  // for uint8_t etc...

#ifdef MKS_TFT35_V1_0  // added for MKS TFT 35 V1.0 support
  #define ADDR_FLASH_SECTOR_0  ((uint32_t)0x08000000)  // Base @ of Sector 0, 16 Kbytes
  #define ADDR_FLASH_SECTOR_1  ((uint32_t)0x08004000)  // Base @ of Sector 1, 16 Kbytes
  #define ADDR_FLASH_SECTOR_2  ((uint32_t)0x08008000)  // Base @ of Sector 2, 16 Kbytes
  #define ADDR_FLASH_SECTOR_3  ((uint32_t)0x0800C000)  // Base @ of Sector 3, 16 Kbytes
  #define ADDR_FLASH_SECTOR_4  ((uint32_t)0x08010000)  // Base @ of Sector 4, 64 Kbytes
  #define ADDR_FLASH_SECTOR_5  ((uint32_t)0x08020000)  // Base @ of Sector 5, 128 Kbytes
  #define ADDR_FLASH_SECTOR_6  ((uint32_t)0x08040000)  // Base @ of Sector 6, 128 Kbytes
  #define ADDR_FLASH_SECTOR_7  ((uint32_t)0x08060000)  // Base @ of Sector 7, 128 Kbytes
  #define ADDR_FLASH_SECTOR_8  ((uint32_t)0x08080000)  // Base @ of Sector 8, 128 Kbytes
  #define ADDR_FLASH_SECTOR_9  ((uint32_t)0x080A0000)  // Base @ of Sector 9, 128 Kbytes
  #define ADDR_FLASH_SECTOR_10 ((uint32_t)0x080C0000)  // Base @ of Sector 10, 128 Kbytes
  #define ADDR_FLASH_SECTOR_11 ((uint32_t)0x080E0000)  // Base @ of Sector 11, 128 Kbytes
  #define ADDR_FLASH_SECTOR_12 ((uint32_t)0x08100000)  // Base @ of Sector 0, 16 Kbytes
  #define ADDR_FLASH_SECTOR_13 ((uint32_t)0x08104000)  // Base @ of Sector 1, 16 Kbytes
  #define ADDR_FLASH_SECTOR_14 ((uint32_t)0x08108000)  // Base @ of Sector 2, 16 Kbytes
  #define ADDR_FLASH_SECTOR_15 ((uint32_t)0x0810C000)  // Base @ of Sector 3, 16 Kbytes
  #define ADDR_FLASH_SECTOR_16 ((uint32_t)0x08110000)  // Base @ of Sector 4, 64 Kbytes
  #define ADDR_FLASH_SECTOR_17 ((uint32_t)0x08120000)  // Base @ of Sector 5, 128 Kbytes
  #define ADDR_FLASH_SECTOR_18 ((uint32_t)0x08140000)  // Base @ of Sector 6, 128 Kbytes
  #define ADDR_FLASH_SECTOR_19 ((uint32_t)0x08160000)  // Base @ of Sector 7, 128 Kbytes
  #define ADDR_FLASH_SECTOR_20 ((uint32_t)0x08180000)  // Base @ of Sector 8, 128 Kbytes
  #define ADDR_FLASH_SECTOR_21 ((uint32_t)0x081A0000)  // Base @ of Sector 9, 128 Kbytes
  #define ADDR_FLASH_SECTOR_22 ((uint32_t)0x081C0000)  // Base @ of Sector 10, 128 Kbytes
  #define ADDR_FLASH_SECTOR_23 ((uint32_t)0x081E0000)  // Base @ of Sector 11, 128 Kbytes
#endif

void HAL_FlashRead(uint8_t *data, uint32_t len);
void HAL_FlashWrite(uint8_t *data, uint32_t len);

#endif
