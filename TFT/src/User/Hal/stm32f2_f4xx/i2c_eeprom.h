#ifndef _I2C_EEPROM__H_
#define _I2C_EEPROM__H_

#include <stdint.h>

void i2C_Init_EEPROM(void);
void EEPROM_FlashRead(uint8_t * data, uint16_t len);
void EEPROM_FlashWrite(uint8_t * data, uint16_t len);

#endif
