#include "Mode.h"
#include "includes.h"

bool serialHasBeenInitialized = false;

void Serial_ReSourceDeInit(void)
{
  if (!serialHasBeenInitialized) return;
  serialHasBeenInitialized = false;
  memset(&infoHost, 0, sizeof(infoHost));
  resetInfoFile();
  SD_DeInit();
  Serial_DeInit();
}

void Serial_ReSourceInit(void)
{
  if (serialHasBeenInitialized) return;
  serialHasBeenInitialized = true;
  
  Serial_Init(infoSettings.baudrate);

#ifdef U_DISK_SUPPROT
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
#endif
}

void infoMenuSelect(void)
{
  #ifdef CLEAN_MODE_SWITCHING_SUPPORT
    Serial_ReSourceInit();
  #endif
  infoMenu.cur = 0;
  switch(infoSettings.mode)
  {
    case SERIAL_TSC:
    {
      #ifndef CLEAN_MODE_SWITCHING_SUPPORT
        Serial_ReSourceInit();
      #endif

      #ifdef BUZZER_PIN
        Buzzer_Config();
      #endif
      GUI_SetColor(FONT_COLOR);
      GUI_SetBkColor(BACKGROUND_COLOR);
      infoMenu.menu[infoMenu.cur] = menuStatus; //status screen as default screen on boot
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

      reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT); // reset connect status
      break;
    }

    #ifdef ST7920_SPI

    case LCD12864:

      #ifdef BUZZER_PIN
        Buzzer_DeConfig();  // Disable buzzer in LCD12864 Simulations mode.
      #endif
      
      #ifdef LED_color_PIN
        knob_LED_DeInit();
      #endif
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
  #if defined(ST7920_BANNER_TEXT)
    RADIO modeRadio = {
      {(u8*)"Serial Touch Screen", (u8*)ST7920_BANNER_TEXT, (u8*)"LCD2004 Simulator"},
      SIMULATOR_XSTART, SIMULATOR_YSTART,
      BYTE_HEIGHT*2, 2,
      0
      };
  #else
    RADIO modeRadio = {
      {(u8*)"Serial Touch Screen", (u8*)"12864 Simulator", (u8*)"LCD2004 Simulator"},
      SIMULATOR_XSTART, SIMULATOR_YSTART,
      BYTE_HEIGHT*2, 2,
      0
      };
  #endif

  MKEY_VALUES  key_num = MKEY_IDLE;
  MODEselect = 1;
  bool keyback = false;

  int16_t nowEncoder = encoderPosition = 0;
  int8_t  nowMode = modeRadio.select = infoSettings.mode;

  GUI_Clear(BACKGROUND_COLOR);
  //RADIO_Create(&modeRadio);
  #ifndef CLEAN_MODE_SWITCHING_SUPPORT  
    Serial_ReSourceDeInit();
  #endif

  show_selectICON();
  TSC_ReDrawIcon = NULL; // Disable icon redraw callback function

  selectmode(nowMode);

  while(!XPT2046_Read_Pen() || LCD_ReadBtn(LCD_BUTTON_INTERVALS));      //wait for button release

  while(infoMenu.menu[infoMenu.cur] == menuMode)
  {
    key_num = MKeyGetValue();

		if(keyback)
    {
			Touch_Sw(1);
			while(!XPT2046_Read_Pen());
			break;
    }

    if(LCD_ReadBtn(LCD_BUTTON_INTERVALS))
    {
      break;
    }
    if(encoderPosition)
    {
      nowMode = limitValue(0, nowMode + encoderPosition, modeRadio.num - 1);
      selectmode(nowMode);
      encoderPosition = 0;
    }

    LCD_LoopEncoder();
    #ifdef CLEAN_MODE_SWITCHING_SUPPORT
      loopBackEnd();
    #endif

    if(key_num==MKEY_1)
		{
			Touch_Sw(2);
			nowMode = SERIAL_TSC;
      keyback = true;
		}

		if(key_num==MKEY_0)
		{
			Touch_Sw(3);
			nowMode = LCD12864;
      keyback = true;
		}
  }
  if(infoSettings.mode != nowMode)
  {
    infoSettings.mode = nowMode;
    storePara();
  }

  MODEselect = 0;
  infoMenuSelect();
}
#endif
