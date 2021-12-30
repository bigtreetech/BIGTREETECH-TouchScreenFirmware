#include "HW_Init.h"
#include "includes.h"

void HW_GetClocksFreq(CLOCKS *clk)
{
  RCC_GetClocksFreq(&clk->rccClocks);

  if (clk->rccClocks.PCLK1_Frequency < clk->rccClocks.HCLK_Frequency)  // if (APBx presc = 1) x1 else x2
    clk->PCLK1_Timer_Frequency = clk->rccClocks.PCLK1_Frequency * 2;
  else
    clk->PCLK1_Timer_Frequency = clk->rccClocks.PCLK1_Frequency;

  if (clk->rccClocks.PCLK2_Frequency < clk->rccClocks.HCLK_Frequency)
    clk->PCLK2_Timer_Frequency = clk->rccClocks.PCLK2_Frequency * 2;
  else
    clk->PCLK2_Timer_Frequency = clk->rccClocks.PCLK2_Frequency;
}

void HW_Init(void)
{
  HW_GetClocksFreq(&mcuClocks);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_init();

  #ifdef DISABLE_JTAG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // disable JTAG, enable SWD
  #endif

  #ifdef DISABLE_DEBUG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  // disable JTAG & SWD
  #endif

  #if defined(MKS_TFT)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
  #endif

  XPT2046_Init();
  OS_TimerInitMs();  // system clock timer, cycle 1ms, called after XPT2046_Init()
  W25Qxx_Init();
  LCD_Init();
  readStoredPara();  // read settings parameter

  #if defined(SERIAL_DEBUG_PORT) && defined(SERIAL_DEBUG_ENABLED)
    Serial_Init(ALL_PORTS);  // Initialize serial ports first if debugging is enabled
  #endif

  LCD_RefreshDirection(infoSettings.rotated_ui);  // refresh display direction after reading settings
  scanUpdates();                                  // scan icon, fonts and config files
  checkflashSign();                               // check font/icon/config signature in SPI flash for update
  initMachineSettings();                          // load default machine settings

  #ifdef LED_COLOR_PIN
    knob_LED_Init();
    Knob_LED_SetColor(led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);  // set last saved color after initialization
  #endif

  #ifdef BUZZER_PIN
    Buzzer_Config();
  #endif

  #if !defined(MKS_TFT)
    // causes hang if we deinit spi1
    SD_DeInit();
  #endif

  #if LCD_ENCODER_SUPPORT
    LCD_Enc_Init();
  #endif

  #if ENC_ACTIVE_SIGNAL
    LCD_Enc_InitActiveSignal(infoSettings.marlin_type == LCD12864);
  #endif

  #ifdef PS_ON_PIN
    PS_ON_Init();
  #endif

  #ifdef FIL_RUNOUT_PIN
    FIL_Runout_Init();
  #endif

  #ifdef U_DISK_SUPPORT
    USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
  #endif

  if (readIsTSCExist() == false)  // read settings parameter
  {
    LCD_RefreshDirection(infoSettings.rotated_ui);
    TSC_Calibration();
    storePara();
  }
  else if (readIsNotStored())
  {
    storePara();
  }

  LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_brightness]);

  Mode_Switch();
}

void HW_InitMode(uint8_t mode)
{
  if (mode == MODE_SERIAL_TSC)
  {
    Serial_Init(ALL_PORTS);  // enable serial comm in Touch mode

    #ifdef BUZZER_PIN  // enable buzzer in Touch mode
      Buzzer_Config();
    #endif

    #if LED_COLOR_PIN  // enable knob LED only in Touch mode
      #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE  // set last saved color after initialization
        knob_LED_Init();
        Knob_LED_SetColor(led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
      #endif
    #endif

    #if ENC_ACTIVE_SIGNAL  // set encoder inactive signal if Touch mode is active
      LCD_Enc_SetActiveSignal(infoSettings.marlin_type == LCD12864, 0);
    #endif
  }
  else
  {
    if (infoSettings.serial_always_on == ENABLED)  // enable serial comm if `serial_always_on` is enabled
      Serial_Init(ALL_PORTS);
    else  // disable serial comm if `serial_always_on` is disabled
      Serial_DeInit(ALL_PORTS);

    #ifdef BUZZER_PIN  // disable buzzer in Marlin mode
      Buzzer_DeConfig();
    #endif

    #if LED_COLOR_PIN  // disable knob LED in Marlin mode
      #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE
        knob_LED_DeInit();
      #endif
    #endif

    #if ENC_ACTIVE_SIGNAL  // set encoder active signal if Marlin mode is active
      LCD_Enc_SetActiveSignal(infoSettings.marlin_type == LCD12864, 1);
    #endif

    #if !defined(MKS_TFT)
      // causes hang if we deinit spi1
      SD_DeInit();
    #endif
  }
}
