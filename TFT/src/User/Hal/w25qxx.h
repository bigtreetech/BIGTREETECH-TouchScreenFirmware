#ifndef _W25QXX_H_
#define _W25QXX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "variants.h"  // for u8 etc...

#define CMD_WRITE_ENABLE   0x06
#define CMD_WRITE_DISABLE  0x04
#define CMD_READ_REGISTER1 0x05
#define CMD_READ_REGISTER2 0x35
#define CMD_PAGE_PROGRAM   0x02
#define CMD_SECTOR_ERASE   0x20
#define CMD_BLOCK_ERASE    0xD8
#define CMD_FLASH__BE      0xC7

#define CMD_READ_ID         0x9F
#define CMD_READ_DATA       0x03
#define CMD_FAST_READ_DATA  0x0B
#define W25QXX_DUMMY_BYTE   0xFF
#define W25QXX_SPI_PAGESIZE 0x100

#define KB(x) (x * 1024l)
#define MB(x) (x * 1024l * 1024l)

uint8_t W25Qxx_SPI_Read_Write_Byte(uint8_t data);
void W25Qxx_SPI_CS_Set(u8 level);
void W25Qxx_Init(void);

void W25Qxx_WriteEnable(void);
void W25Qxx_WaitForWriteEnd(void);
void W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void W25Qxx_EraseSector(uint32_t SectorAddr);
void W25Qxx_EraseBlock(uint32_t BlockAddr);
void W25Qxx_EraseBulk(void);
uint32_t W25Qxx_ReadID(void);
uint32_t W25Qxx_ReadCapacity(void);

#ifdef __cplusplus
}
#endif

#endif
