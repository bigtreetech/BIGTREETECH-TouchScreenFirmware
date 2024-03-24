#ifndef _SD_H_
#define _SD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>    // for uint8_t etc.
#include "variants.h"  // for SD_SPI_SUPPORT, SD_SDIO_SUPPORT etc.

#ifdef SD_SPI_SUPPORT
  uint8_t SD_CD_Inserted(void);

  uint8_t SD_Wait_Ready(void);                                        // Waiting for SD card preparation
  uint8_t SD_Get_Ack(uint8_t Response);                               // Get the answer
  uint8_t SD_GetCID(uint8_t * cid_data);                              // Read SD card CID
  uint8_t SD_GetCSD(uint8_t * csd_data);                              // Read SD card CSD
  uint32_t SD_Get_Sector_Count(void);                                 // Number of sectors read

  uint8_t SD_Init(void);                                              // initialization
  void SD_DeInit(void);
  uint8_t SD_ReadDisk(uint8_t * buf, uint32_t sector, uint8_t cnt);   // Read SD card, fatfs / usb call
  uint8_t SD_WriteDisk(uint8_t * buf, uint32_t sector, uint8_t cnt);  // Write SD card, fatfs / usb call
#else  // e.g. SD_SDIO_SUPPORT
  #include "sdio_sdcard.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
