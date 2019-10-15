#include "flashStore.h"
#include "STM32_Flash.h"

#define PARA_SIZE 256  //bytes
#define PARA_SIGN 0x20190826

extern u32 TSC_Para[7];        //触摸屏校准系数
extern SETTINGS infoSettings;  //用户设置

void wordToByte(u32 word, u8 *bytes)  //大端模式
{
  u8 len = 4;
  u8 i = 0;
  for(i = 0; i < len; i++)
  {
    bytes[i] = (word >> 24) & 0xFF;
    word <<= 8;
  }
}

u32 byteToWord(u8 *bytes, u8 len)
{
  u32 word = 0;
  u8 i = 0;
  for(i = 0; i < len; i++)
  {
    word <<= 8;
    word |= bytes[i];
  }
  return word;
}

bool readStoredPara(void)
{
  u8 data[PARA_SIZE];
  u32 index = 0;
  u32 paraSign = 0;
  STM32_FlashRead(data, PARA_SIZE);
  
  paraSign = byteToWord(data + (index += 4), 4);
  if(paraSign != PARA_SIGN)   return false;
    
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    TSC_Para[i] = byteToWord(data + (index += 4), 4);
  }
  infoSettings.baudrate  = byteToWord(data + (index += 4), 4);
  infoSettings.language  = byteToWord(data + (index += 4), 4);
  infoSettings.mode      = byteToWord(data + (index += 4), 4);
  infoSettings.runout    = byteToWord(data + (index += 4), 4);
  infoSettings.rotate_ui = byteToWord(data + (index += 4), 4);
  infoSettings.bg_color  = byteToWord(data + (index += 4), 4);
  infoSettings.font_color  = byteToWord(data + (index += 4), 4);
  
  return true;
}

void storePara(void)
{
  u8 data[PARA_SIZE]; 
  u32 index = 0;
  
  wordToByte(PARA_SIGN, data + (index += 4));    
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    wordToByte(TSC_Para[i], data + (index += 4));
  }
  wordToByte(infoSettings.baudrate, data + (index += 4));
  wordToByte(infoSettings.language, data + (index += 4));
  wordToByte(infoSettings.mode, data + (index += 4));
  wordToByte(infoSettings.runout, data + (index += 4));
  wordToByte(infoSettings.rotate_ui, data + (index += 4));
  wordToByte(infoSettings.bg_color, data + (index += 4));
  wordToByte(infoSettings.font_color, data + (index += 4));
  
  STM32_FlashWrite(data, PARA_SIZE);
}
