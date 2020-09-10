#ifndef _I2C_EEPROM__H_
#define _I2C_EEPROM__H_
#include "includes.h"

/* EEPROM hardware address and page size */ 
#define EEPROM_PAGESIZE             16
#define EEPROM_MAX_SIZE             0x800 /* 64Kbit*/

#define EEPROM_OK      0
#define EEPROM_FAIL    1
#define EEPROM_TIMEOUT 3

#define EEPROM_READ_TIMEOUT         ((uint32_t)(1000))
/* Timeout for write based on max write which is EEPROM_PAGESIZE bytes: EEPROM_PAGESIZE * BSP_I2C_SPEED (320us)*/
#define EEPROM_WRITE_TIMEOUT        ((uint32_t)(1000))

#define AT24C01		127		//PAGE_SIZE	8	byte
#define AT24C02		255		//PAGE_SIZE	8	byte
#define AT24C04		511		//PAGE_SIZE	16	byte
#define AT24C08		1023	//PAGE_SIZE	16	byte
#define AT24C16		2047	//PAGE_SIZE	16	byte
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define AT24CXX_TYPE		AT24C16
#define AT24CXX_ADDRESS		0XA0
#define AT24CXX_PAGE_SIZE	16//8
#define I2C_MEMADD_SIZE_8BIT            ((uint32_t)0x00000001U)
//#if (AT24CXX_TYPE < AT24C04)        
#if (AT24CXX_TYPE < AT24C32)        
#define AT24CXX_MEMADD_SIZE	I2C_MEMADD_SIZE_8BIT	
#else
#define AT24CXX_MEMADD_SIZE	I2C_MEMADD_SIZE_16BIT	
#endif




u8 EEPROM_WriteData(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);


/***
 * 
 * #define EEPROM_PAGESIZE             32
#define EEPROM_MAX_SIZE             0x2000 /* 64Kbit*/

#endif
