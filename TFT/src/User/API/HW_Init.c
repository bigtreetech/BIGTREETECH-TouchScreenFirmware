#include "HW_Init.h"
#include "includes.h"

#ifdef I2C_EEPROM  // added I2C_EEPROM support for MKS_TFT35_V1_0
  #include "i2c_eeprom.h"
#endif

void HW_GetClocksFreq(CLOCKS *clk)
{
#ifdef GD32F2XX
  RCU_GetClocksFreq(&clk->rccClocks);
#else
  RCC_GetClocksFreq(&clk->rccClocks);
#endif

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
#ifdef GD32F2XX
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
#else
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#endif
  Delay_init();

  #ifdef DISABLE_JTAG
    DISABLE_JTAG();  // disable JTAG, enable SWD
  #endif

  #ifdef DISABLE_DEBUG
    DISABLE_DEBUG();  // disable JTAG & SWD
  #endif

  #if defined(MKS_TFT) && !defined (MKS_TFT35_V1_0)  // not used by MKS_TFT35_V1_0
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
  #endif

  XPT2046_Init();
  OS_TimerInitMs();  // system clock timer, cycle 1ms, called after XPT2046_Init()
  W25Qxx_Init();
  LCD_Init();

  #ifdef I2C_EEPROM  // added I2C_EEPROM support for MKS_TFT35_V1_0
    i2C_Init_EEPROM();
  #endif

  readStoredPara();  // read settings parameter

  #if defined(SERIAL_DEBUG_PORT) && defined(SERIAL_DEBUG_ENABLED)
    Serial_Init(ALL_PORTS);  // initialize serial ports first if debugging is enabled
  #endif

  #ifdef USB_FLASH_DRIVE_SUPPORT
    USB_Init();
  #endif

  LCD_RefreshDirection(infoSettings.rotated_ui);  // refresh display direction after reading settings
  scanUpdates();                                  // scan icon, fonts and config files
  checkflashSign();                               // check font/icon/config signature in SPI flash for update
  initMachineSettings();                          // load default machine settings

  #if !defined(MKS_TFT)
    // causes hang if we deinit spi1
    SD_DeInit();
  #endif

  #ifdef PS_ON_PIN
    PS_ON_Init();
  #endif

  #ifdef FIL_RUNOUT_PIN
    FIL_Runout_Init();
  #endif

  #ifdef BUZZER_PIN
    Buzzer_Config();
  #endif

  #ifdef KNOB_LED_COLOR_PIN
    knob_LED_Init();
    Knob_LED_SetColor(knob_led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);  // set last saved color after initialization
  #endif

  #if LCD_ENCODER_SUPPORT
    LCD_Enc_Init();
  #endif

  #if ENC_ACTIVE_SIGNAL
    if (infoSettings.marlin_type == LCD12864)
      LCD_Enc_InitActiveSignal();
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

  LED_SetColor(&infoSettings.led_color, false);  // set (neopixel) LED light current color to configured color

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

    #if KNOB_LED_COLOR_PIN  // enable knob LED only in Touch mode
      #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE  // set last saved color after initialization
        knob_LED_Init();
        Knob_LED_SetColor(led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
      #endif
    #endif

    #if ENC_ACTIVE_SIGNAL  // set encoder inactive signal if Touch mode is active
      if (infoSettings.marlin_type == LCD12864)
        LCD_Enc_SetActiveSignal(0);
    #endif
  }
  else
  {
    if (infoSettings.serial_always_on == ENABLED)  // enable serial comm if `serial_always_on` is enabled
      Serial_Init(ALL_PORTS);
    else  // disable serial comm if `serial_always_on` is disabled
      Serial_DeInit(ALL_PORTS);

    #if !defined(MKS_TFT)
      // causes hang if we deinit spi1
      SD_DeInit();
    #endif

    #ifdef BUZZER_PIN  // disable buzzer in Marlin mode
      Buzzer_DeConfig();
    #endif

    #if KNOB_LED_COLOR_PIN  // disable knob LED in Marlin mode
      #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE
        knob_LED_DeInit();
      #endif
    #endif

    #if ENC_ACTIVE_SIGNAL  // set encoder active signal if Marlin mode is active
      if (infoSettings.marlin_type == LCD12864)
        LCD_Enc_SetActiveSignal(1);
    #endif
  }
}
