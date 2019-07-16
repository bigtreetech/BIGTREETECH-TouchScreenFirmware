#include "flashStore.h"

#define PARA_SIGN 0x20190710
#define SIGN_ADDRESS (0x08040000 - 0x800)  //保留最后一页(2KB),用来保存用户参数

extern u32 TSC_Para[7];        //触摸屏校准系数
#define TSC_ADDRESS (0x08040000 - 0x800 + 4)

extern SETTINGS infoSettings;  //用户设置
#define SETTING_ADDRESS (0x08040000 - 0x800 + 32) 


bool readStoredPara(void)
{
  if(*((vu32*)SIGN_ADDRESS) != PARA_SIGN)   return false;
    
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    TSC_Para[i] = *((vu32*)(TSC_ADDRESS + i*4));
  }
  infoSettings.baudrate = *((vu32*) SETTING_ADDRESS);
  infoSettings.language = *((vu32*)(SETTING_ADDRESS + 4));
  infoSettings.mode     = *((vu32*)(SETTING_ADDRESS + 8));
    
  return true;
}

void storePara(void)
{
  FLASH_Unlock();
    
  FLASH_ErasePage(SIGN_ADDRESS);
  FLASH_ProgramWord(SIGN_ADDRESS, PARA_SIGN);
    
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    FLASH_ProgramWord((TSC_ADDRESS + i*4), TSC_Para[i]);
  }
    
  FLASH_ProgramWord(SETTING_ADDRESS, infoSettings.baudrate);
  FLASH_ProgramWord(SETTING_ADDRESS + 4, infoSettings.language);
  FLASH_ProgramWord(SETTING_ADDRESS + 8, infoSettings.mode);

  FLASH_Lock();
}
