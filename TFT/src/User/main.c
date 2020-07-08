#include "includes.h"

HOST  infoHost;  // Information interaction with Marlin
MENU  infoMenu;  // Menu structure

void Hardware_GenericInit(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_init(F_CPUM);
  OS_TimerInit(1000-1, F_CPUM-1);  // System clock timer, cycle 1ms

  #ifdef DISABLE_JTAG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // disable JTAG, enable SWD
  #endif

  #ifdef DISABLE_DEBUG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //disable JTAG & SWD
  #endif

  #ifdef MKS_32_V1_4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
  #endif

  XPT2046_Init();
  W25Qxx_Init();
  LCD_Init();
  readStoredPara();
  LCD_RefreshDirection();  //refresh display direction after reading settings
  scanUpdates();
  #ifndef MKS_32_V1_4
    //causes hang if we deinit spi1
    SD_DeInit();
  #endif
  #if LCD_ENCODER_SUPPORT
    HW_EncoderInit();
  #endif

  #ifdef PS_ON_PIN
    PS_ON_Init();
  #endif

  #ifdef FIL_RUNOUT_PIN
    FIL_Runout_Init();
  #endif

  #ifdef LED_COLOR_PIN
    knob_LED_Init();
  #else
    #define STARTUP_KNOB_LED_COLOR 1
  #endif
  #ifdef U_DISK_SUPPROT
    USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
  #endif

  if(readStoredPara() == false) // Read settings parameter
  {
    TSC_Calibration();
    storePara();
  }
  #ifdef LCD_LED_PWM_CHANNEL
    Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
  #endif
  GUI_RestoreColorDefault();
  infoMenuSelect();
}

int main(void)
{
  SCB->VTOR = VECT_TAB_FLASH;

  Hardware_GenericInit();

  for(;;)
  {
    (*infoMenu.menu[infoMenu.cur])();
  }
}
