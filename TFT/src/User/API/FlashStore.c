#include "FlashStore.h"
#include "Touch_Screen.h"
#include "Settings.h"
#include "HAL_Flash.h"
#include <string.h>

#ifdef I2C_EEPROM  // added I2C_EEPROM suppport for MKS_TFT35_V1_0
  #include "i2c_eeprom.h"
#endif

#define TSC_SIGN  0x20200512  // (YYYYMMDD) DO NOT MODIFY (Touch Screem Calibration sign)
#define PARA_SIGN 0x20240203  // (YYYYMMDD) if a new setting parameter is added, modify here and
                              // initialize the initial value in the "initSettings()" function
enum
{
  PARA_TSC_EXIST = (1 << 0),
  PARA_NOT_STORED = (1 << 1),
};

uint8_t paraStatus = 0;

void wordToByte(uint32_t word, uint8_t *bytes)
{
  uint8_t len = 4;
  uint8_t i = 0;
  for (i = 0; i < len; i++)
  {
    bytes[i] = (word >> 24) & 0xFF;
    word <<= 8;
  }
}

uint32_t byteToWord(uint8_t *bytes, uint8_t len)
{
  uint32_t word = 0;
  uint8_t i = 0;
  for (i = 0; i < len; i++)
  {
    word <<= 8;
    word |= bytes[i];
  }
  return word;
}

void readStoredPara(void)
{
  uint8_t data[PARA_SIZE];
  uint32_t index = 0;
  uint32_t sign = 0;

#ifdef I2C_EEPROM  // added I2C_EEPROM suppport for MKS_TFT35_V1_0
  EEPROM_FlashRead(data, PARA_SIZE);
#else
  HAL_FlashRead(data, PARA_SIZE);
#endif

  sign = byteToWord(data + (index += 4), 4);
  if (sign == TSC_SIGN)
  {
    paraStatus |= PARA_TSC_EXIST;  // if the touch screen calibration parameter already exists
    for (int i = 0; i < sizeof(TS_CalPara) / sizeof(TS_CalPara[0]); i++)
    {
      TS_CalPara[i] = byteToWord(data + (index += 4), 4);
    }
  }

  sign = byteToWord(data + (index += 4), 4);
  if (sign != PARA_SIGN)  // if the settings parameter is illegal, reset settings parameter
  {
    paraStatus |= PARA_NOT_STORED;
    initSettings();
  }
  else
  {
    memcpy(&infoSettings, data + (index += 4), sizeof(SETTINGS));
    //if ((paraStatus & PARA_TSC_EXIST) == 0) infoSettings.rotated_ui = DISABLED;  // unecessarily rotates UI to Default?
  }
}

void storePara(void)
{
  uint8_t data[PARA_SIZE];
  uint32_t index = 0;

  wordToByte(TSC_SIGN, data + (index += 4));
  for (int i = 0; i < sizeof(TS_CalPara) / sizeof(TS_CalPara[0]); i++)
  {
    wordToByte(TS_CalPara[i], data + (index += 4));
  }

  wordToByte(PARA_SIGN, data + (index += 4));
  memcpy(data + (index += 4), &infoSettings, sizeof(SETTINGS));

#ifdef I2C_EEPROM                      // added I2C_EEPROM suppport for MKS_TFT35_V1_0
  EEPROM_FlashWrite(data, PARA_SIZE);  // store settings in I2C_EEPROM
#else
  HAL_FlashWrite(data, PARA_SIZE);
#endif
}

bool readIsTSCExist(void)
{
  return ((paraStatus & PARA_TSC_EXIST) != 0);
}

bool readIsNotStored(void)
{
  return ((paraStatus & PARA_NOT_STORED) != 0);
}
