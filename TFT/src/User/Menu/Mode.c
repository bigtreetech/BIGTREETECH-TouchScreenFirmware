#include "Mode.h"
#include "includes.h"


void Serial_ReSourceDeInit(void)
{
  memset(&infoHost, 0, sizeof(infoHost));
  resetInfoFile();
  SD_DeInit();
#ifdef BUZZER_PIN
  Buzzer_DeConfig();
#endif
  Serial_DeConfig();
}

void Serial_ReSourceInit(void)
{
#ifdef BUZZER_PIN
  Buzzer_Config();
#endif
  Serial_Config(infoSettings.baudrate);
  
#ifdef U_DISK_SUPPROT
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
#endif
}

void infoMenuSelect(void)
{
  infoMenu.cur = 0;
  switch(infoSettings.mode)
  {
    case SERIAL_TSC:
    {
      Serial_ReSourceInit();
      scanUpdates();
      GUI_SetColor(FK_COLOR);
      GUI_SetBkColor(BK_COLOR);
      infoMenu.menu[infoMenu.cur] = menuMain;
      
      #ifdef SHOW_BTT_BOOTSCREEN
        u32 startUpTime = OS_GetTime();
        heatSetUpdateTime(100);
        LOGO_ReadDisplay();
        while(OS_GetTime() - startUpTime < 300)  //Display 3s logo
        {                                                                                                                     
          loopProcess();	
        }
        heatSetUpdateTime(300);
      #endif
      break;
    }
      
    #ifdef ST7920_SPI
    case LCD12864:
      GUI_SetColor(ST7920_FNCOLOR);
      GUI_SetBkColor(ST7920_BKCOLOR);
      infoMenu.menu[infoMenu.cur] = menuST7920;      
      break;
    #endif
  }
}

#if LCD_ENCODER_SUPPORT
void menuMode(void)
{  
  RADIO modeRadio = {
    {(u8*)"Serial Touch Screen", (u8*)ST7920_BANNER_TEXT, (u8*)"LCD2004 Simulator"},
    SIMULATOR_XSTART, SIMULATOR_YSTART,
    BYTE_HEIGHT*2, 2,
    0
  };
  int16_t nowEncoder = encoderPosition = 0;
  int8_t  nowMode = modeRadio.select = infoSettings.mode;
  
  GUI_Clear(BLACK);
  RADIO_Create(&modeRadio);
  Serial_ReSourceDeInit();
  while(LCD_ReadBtn(LCD_BUTTON_INTERVALS));      //wait for button release
  
  while(infoMenu.menu[infoMenu.cur] == menuMode)
  {
    if(LCD_ReadBtn(LCD_BUTTON_INTERVALS))
    {
      break;
    }
    if(encoderPosition)
    {
      nowMode = limitValue(0, nowMode + encoderPosition, modeRadio.num - 1);
      RADIO_Select(&modeRadio, nowMode);
      encoderPosition = 0;    
    }
    
    LCD_LoopEncoder();
  }
  if(infoSettings.mode != nowMode)
  {
    infoSettings.mode = nowMode;
    storePara();
  }
  infoMenuSelect();
}
#endif
