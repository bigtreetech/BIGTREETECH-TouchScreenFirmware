#include <string.h>
#include "flashStore.h"
#include "STM32_Flash.h"

#define TSC_SIGN  0x20200512  // DO NOT MODIFY
#define PARA_SIGN 0x20210130  // (YYYYMMDD) If a new setting parameter is added,
                              // modify here and initialize the initial value
                              // in the "infoSettingsReset()" function
enum
{
  PARA_TSC_EXIST = (1 << 0),
  PARA_WAS_RESTORED = (1<< 1),
};

extern int32_t TSC_Para[7];
extern SETTINGS infoSettings;

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

// Read settings parameter if exist, or reset settings parameter
void readStoredPara(void)
{
  uint8_t data[PARA_SIZE];
  uint32_t index = 0;
  uint32_t sign = 0;

  STM32_FlashRead(data, PARA_SIZE);

  sign = byteToWord(data + (index += 4), 4);
  if (sign == TSC_SIGN)
  {
    paraStatus |= PARA_TSC_EXIST;  // If the touch screen calibration parameter already exists
    for (int i = 0; i < sizeof(TSC_Para) / sizeof(TSC_Para[0]); i++)
    {
      TSC_Para[i] = byteToWord(data + (index += 4), 4);
    }
  }

  sign = byteToWord(data + (index += 4), 4);
  if (sign != PARA_SIGN)  // If the settings parameter is illegal, reset settings parameter
  {
    paraStatus = PARA_WAS_RESTORED;
    infoSettingsReset();
  }
  else
  {
    memcpy(&infoSettings, data + (index += 4), sizeof(SETTINGS));
    if ((paraStatus & PARA_TSC_EXIST) == 0) infoSettings.rotate_ui = DISABLED;
  }
}

void storePara(void)
{
  uint8_t data[PARA_SIZE];
  uint32_t index = 0;

  wordToByte(TSC_SIGN, data + (index += 4));
  for (int i = 0; i < sizeof(TSC_Para) / sizeof(TSC_Para[0]); i++)
  {
    wordToByte(TSC_Para[i], data + (index += 4));
  }
  wordToByte(PARA_SIGN, data + (index += 4));
  memcpy(data + (index += 4), &infoSettings, sizeof(SETTINGS));

  STM32_FlashWrite(data, PARA_SIZE);
}

bool readIsTSCExist(void)
{
  return ((paraStatus & PARA_TSC_EXIST) != 0);
}

bool readIsRestored(void)
{
  return ((paraStatus & PARA_WAS_RESTORED) != 0);
}
