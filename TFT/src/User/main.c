#include "includes.h"

HOST  infoHost;  // Information interaction with Marlin
MENU  infoMenu;  // Menu structure


void Hardware_GenericInit(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_init(72);  
  OS_TimerInit(9999,71);  // System clock timer, cycle 10ms
  
#ifdef DISABLE_DEBUG 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //disable JTAG & SWD
#endif
 
#ifdef DISABLE_JTAG
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
#endif
  
  XPT2046_Init();
  W25Qxx_Init();
  LCD_Init();
  
#ifdef LCD_ENCODER_SUPPORT
  LCD_EncoderInit();
#endif
    
  if(readStoredPara() == false)
  {    
    TSC_Calibration();
    infoSettings.baudrate = 115200;
    infoSettings.language = ENGLISH;
    infoSettings.mode = SERIAL_TSC;
    storePara();
  }
  
  infoMenuSelect();
}

int main(void)
{
  SCB->VTOR = FLASH_BASE | 0x6000;
 
  Hardware_GenericInit();
  
  for(;;)
  {
    (*infoMenu.menu[infoMenu.cur])();
  }
}
