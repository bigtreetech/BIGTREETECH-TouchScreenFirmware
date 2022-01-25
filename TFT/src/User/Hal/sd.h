#ifndef _SD_H_
#define _SD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for uint8_t etc...

// SDCard type definition
#define SD_TYPE_ERR  0X00
#define SD_TYPE_MMC  0X01
#define SD_TYPE_V1   0X02
#define SD_TYPE_V2   0X04
#define SD_TYPE_V2HC 0X06

// SDCard instruction table
#define CMD0  0   //Card reset
#define CMD1  1
#define CMD8  8   //Command 8, SEND_IF_COND
#define CMD9  9   //Command 9 to read CSD data
#define CMD10 10  //Command 10, read CID data
#define CMD12 12  //Command 12, stop data transmission
#define CMD16 16  //Command 16, set sector size should return 0x00
#define CMD17 17  //Command 17, read sector
#define CMD18 18  //Command 18, read Multi sector
#define CMD23 23  //Command 23, set N blocks to be erased before multi-sector write
#define CMD24 24  //Command 24, write sector
#define CMD25 25  //Command 25, write multiple sectors
#define CMD41 41  //Command 41, should return 0x00
#define CMD55 55  //Command 55, should return 0x01
#define CMD58 58  //Command 58, read OCR information
#define CMD59 59  //Command 59, enable / disable CRC, should return 0x00��

// Data write response word meaning
#define SD_DATA_OK          0x05
#define SD_DATA_CRC_ERROR   0x0B
#define SD_DATA_WRITE_ERROR 0x0D
#define SD_DATA_OTHER_ERROR 0xFF

// SD Card Response Marker
#define SD_RESPONSE_NO_ERROR    0x00  // response error
#define SD_IN_IDLE_STATE        0x01  // Idle state
#define SD_ERASE_RESET          0x02  // Erase reset
#define SD_ILLEGAL_COMMAND      0x04
#define SD_COM_CRC_ERROR        0x08
#define SD_ERASE_SEQUENCE_ERROR 0x10
#define SD_ADDRESS_ERROR        0x20
#define SD_PARAMETER_ERROR      0x40
#define SD_RESPONSE_FAILURE     0xFF

uint8_t SD_CD_Inserted(void);
uint8_t SD_Init(void);                               //initialization
void SD_DeInit(void);
uint8_t SD_ReadDisk(uint8_t * buf, uint32_t sector, uint8_t cnt);   //Read SD card, fatfs / usb call
uint8_t SD_WriteDisk(uint8_t * buf, uint32_t sector, uint8_t cnt);  //Write SD card, fatfs / usb call

uint8_t SD_Wait_Ready(void);                         //Waiting for SD card preparation
uint8_t SD_Get_Ack(uint8_t Response);                     //Get the answer
uint32_t SD_Get_Sector_Count(void);                  //Number of sectors read
uint8_t SD_GetCID(uint8_t * cid_data);                    //Read SD card CID
uint8_t SD_GetCSD(uint8_t * csd_data);                    //Read SD card CSD

#ifdef __cplusplus
}
#endif

#endif
