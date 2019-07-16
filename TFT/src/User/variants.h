#ifndef _VARIANTS_H_
#define _VARIANTS_H_

/*
* hardware source
* TIM4 for os timer
* TIM3 for buzzer timer
*/

//MCU type (STM32F103VC - HD, STM32F105 - CL)
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  #define STM32F10X_HD
#elif defined(TFT24_V1_1)
  #define STM32F10X_CL
#endif

//HSE crystal frequency
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2)|| defined(TFT28)  
  #define HSE_VALUE ((uint32_t)16000000) //16Mhz
#elif defined(TFT24_V1_1)
  #define HSE_VALUE ((uint32_t)8000000) //8Mhz
#endif

//LCD interface
#if defined(TFT35_V1_0) || defined(TFT35_V1_1)
  #define RM68042
  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 0
#elif defined(TFT35_V1_2)
  #define ILI9488
  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 1
#elif defined(TFT28)
  #define ILI9341
  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 1
#elif defined(TFT24_V1_1)
  #define ILI9341
//  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 1
#endif

//LCD Backlight pin (PWM can adjust brightness)
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  //don't support LCD Backlight pin
#elif defined(TFT24_V1_1)
  #define LCD_LED_SUPPORT
  #define LCD_LED_RCC  RCC_APB2Periph_GPIOA
  #define LCD_LED_PORT GPIOA
  #define LCD_LED_PIN  GPIO_Pin_8
#endif

#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28) || defined(TFT24_V1_1)
  #define SERIAL_PORT   _USART1  //default usart port
#endif

//XPT2046 Software SPI Pins (touch screen ic)
//need CS/SCK/MISO/MOSI for Software SPI, and TPEN for pen interrupt
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  #define XPT2046_GPIO_RCC  RCC_APB2Periph_GPIOC
  #define XPT2046_CS_PORT   GPIOC
  #define XPT2046_CS_PIN    GPIO_Pin_0
  #define XPT2046_SCK_PORT  GPIOC
  #define XPT2046_SCK_PIN   GPIO_Pin_1
  #define XPT2046_MISO_PORT GPIOC
  #define XPT2046_MISO_PIN  GPIO_Pin_2
  #define XPT2046_MOSI_PORT GPIOC
  #define XPT2046_MOSI_PIN  GPIO_Pin_3
  #define XPT2046_TPEN_PORT GPIOC
  #define XPT2046_TPEN_PIN  GPIO_Pin_4
#elif defined(TFT24_V1_1)  
  #define XPT2046_GPIO_RCC  RCC_APB2Periph_GPIOA
  #define XPT2046_CS_PORT   GPIOA
  #define XPT2046_CS_PIN    GPIO_Pin_3
  #define XPT2046_SCK_PORT  GPIOA
  #define XPT2046_SCK_PIN   GPIO_Pin_2
  #define XPT2046_MISO_PORT GPIOA
  #define XPT2046_MISO_PIN  GPIO_Pin_1
  #define XPT2046_MOSI_PORT GPIOA
  #define XPT2046_MOSI_PIN  GPIO_Pin_0
  #define XPT2046_TPEN_PORT GPIOA
  #define XPT2046_TPEN_PIN  GPIO_Pin_15
#else
  #error "xpt2046 need to Implement here by yourself."
#endif

//SD Card SPI pins
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  #define SD_LOW_SPEED  7
  #define SD_HIGH_SPEED 0
  #define SD_SPI        _SPI2
  #define SPI2_CS_RCC   RCC_APB2Periph_GPIOB
  #define SPI2_CS_PORT  GPIOB
  #define SPI2_CS_PIN   GPIO_Pin_12
#elif defined(TFT24_V1_1)
  #define SD_LOW_SPEED  7
  #define SD_HIGH_SPEED 1
  #define SD_SPI        _SPI1
  #define SPI1_CS_RCC   RCC_APB2Periph_GPIOA
  #define SPI1_CS_PORT  GPIOA
  #define SPI1_CS_PIN   GPIO_Pin_4
#endif

//SD Card CD detect pin
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2)
  //don't support SD CD(insert/remove) detect
#elif defined(TFT28)
  #define SD_CD_SUPPROT
  #define SD_CD_RCC     RCC_APB2Periph_GPIOB
  #define SD_CD_PORT    GPIOB
  #define SD_CD_PIN     GPIO_Pin_0
#elif defined(TFT24_V1_1)  
  #define SD_CD_SUPPROT
  #define SD_CD_RCC     RCC_APB2Periph_GPIOA
  #define SD_CD_PORT    GPIOA
  #define SD_CD_PIN     GPIO_Pin_13
#endif

//W25Qxx SPI pins
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  #define W25Qxx_SPEED  1
  #define W25Qxx_SPI    _SPI1
  #define SPI1_CS_RCC   RCC_APB2Periph_GPIOA
  #define SPI1_CS_PORT  GPIOA
  #define SPI1_CS_PIN   GPIO_Pin_4
#elif defined(TFT24_V1_1)
  #define W25Qxx_SPEED  0
  #define W25Qxx_SPI    _SPI3
  #define SPI3_CS_RCC   RCC_APB2Periph_GPIOD
  #define SPI3_CS_PORT  GPIOD
  #define SPI3_CS_PIN   GPIO_Pin_2
#endif

//ST7920 Simulator SPI pins
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  //don't support ST7920 Simulator
#elif defined(TFT24_V1_1)
  #define ST7920_SPI _SPI2
#endif

//buzzer support
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  //don't support buzzer
#elif defined(TFT24_V1_1)
  #define BUZZER_SUPPORT
  #define BUZZER_RCC   RCC_APB2Periph_GPIOA
  #define BUZZER_PORT  GPIOA
  #define BUZZER_PIN   GPIO_Pin_14
#endif

//LCD Encoder support
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  //don't support LCD Encoder
#elif defined(TFT24_V1_1)
  #define LCD_ENCODER_SUPPORT
  //PB0:ENC-A PB1:ENC-B PB2:BTN 
  #define LCD_ENCODER_RCC RCC_APB2Periph_GPIOB
  #define LCD_ENCA_PORT   GPIOB
  #define LCD_ENCA_PIN    GPIO_Pin_0
  #define LCD_ENCB_PORT   GPIOB
  #define LCD_ENCB_PIN    GPIO_Pin_1
  #define LCD_BTN_PORT    GPIOB
  #define LCD_BTN_PIN     GPIO_Pin_2
#endif

//U disk support
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
#elif defined(TFT24_V1_1)
  #define U_DISK_SUPPROT
  #define USE_USB_OTG_FS
#endif

//LCD resolution, font and icon size
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2)
  #define LCD_WIDTH	  480
  #define LCD_HEIGHT	320
  #define BYTE_HEIGHT 24
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
  #define ICON_WIDTH  95
  #define ICON_HEIGHT 95
  #define TITLE_END_Y 60
#elif defined(TFT28) || defined(TFT24_V1_1)
  #define LCD_WIDTH	  320
  #define LCD_HEIGHT	240
  #define BYTE_HEIGHT 16
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
  #define ICON_WIDTH  70
  #define ICON_HEIGHT 70
  #define TITLE_END_Y 40
#endif

//Debug disable, free pins for other function
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28)
  #define DISABLE_JTAG    //free JTAG(PB3/PB4) for SPI3
#elif defined(TFT24_V1_1)
  #define DISABLE_DEBUG   //
#endif

//hardware version config
#ifndef HARDWARE_VERSION
  #if defined(TFT35_V1_0)
      #define HARDWARE_VERSION "TFT35_V1.0"   
  #elif defined(TFT35_V1_1)
    #define HARDWARE_VERSION "TFT35_V1.1"
  #elif defined(TFT35_V1_2)
    #define HARDWARE_VERSION "TFT35_V1.2"
  #elif defined(TFT28)
    #define HARDWARE_VERSION "TFT28"
  #elif defined(TFT24_V1_1)
    #define HARDWARE_VERSION "TFT24_V1.1"
  #endif
#endif

#endif
