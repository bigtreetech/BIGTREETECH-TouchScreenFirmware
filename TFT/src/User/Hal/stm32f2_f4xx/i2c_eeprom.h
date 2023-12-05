#ifndef _I2C_EEPROM__H_
#define _I2C_EEPROM__H_

#include "includes.h"

#define EEPROM_OK         0
#define EEPROM_FAIL       1
#define EEPROM_TIMEOUT    3

#define AT24C01             127  // PAGE_SIZE  8 bytes
#define AT24C02             255  // PAGE_SIZE  8 bytes
#define AT24C04             511  // PAGE_SIZE 16 bytes
#define AT24C08            1023  // PAGE_SIZE 16 bytes
#define AT24C16            2047  // PAGE_SIZE 16 bytes
#define AT24C32            4095
#define AT24C64            8191
#define AT24C128          16383
#define AT24C256          32767

//#define AT24CXX_TYPE    EEPROM_TYPE
#define AT24CXX_ADDRESS   EEPROM_ADDRESS
#define AT24CXX_PAGE_SIZE 16              // 8 bytes

// Timeout for write based on max read/write which is
// AT24CXX_PAGE_SIZE bytes: 8 bits * AT24CXX_PAGE_SIZE / I2C_SPEED
//#define EEPROM_READ_TIMEOUT  ((uint32_t)(1000))
//#define EEPROM_WRITE_TIMEOUT ((uint32_t)(1000))

u8 EEPROM_WriteData(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
void EEPROM_FlashRead(u8 *data, u16 len);
void EEPROM_FlashWrite(u8 *data, u16 len);
void i2C_Init_EEPROM();

#endif
