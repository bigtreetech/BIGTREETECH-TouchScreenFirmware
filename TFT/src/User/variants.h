#ifndef _VARIANTS_H_
#define _VARIANTS_H_

#define USE_STDPERIPH_DRIVER
/*
* hardware source
* TIM4 for os timer
* TIM3 for buzzer timer
*/

//MCU type (STM32F103VC - HD, STM32F105 - CL, STM32F207VC)
//HSE crystal frequency
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2)|| defined(TFT28_V1_0)  
  #define VECT_TAB_FLASH 0x08006000
  #define HSE_VALUE ((uint32_t)16000000) //16Mhz
  #define F_CPUM 72
  #define STM32F10X_HD
  #include "stm32f10x.h"
#elif defined(TFT35_V2_0)
  #define VECT_TAB_FLASH 0x08003000
  #define HSE_VALUE ((uint32_t)16000000) //16Mhz
  #define F_CPUM 72
  #define STM32F10X_HD
  #include "stm32f10x.h"
#elif defined(TFT24_V1_1)
  #define VECT_TAB_FLASH 0x08006000
  #define HSE_VALUE ((uint32_t)8000000) //8Mhz
  #define F_CPUM 72
  #define STM32F10X_CL
  #include "stm32f10x.h"
#elif defined(TFT35_V3_0)
  #define VECT_TAB_FLASH 0x08008000
  #define HSE_VALUE ((uint32_t)8000000) //8Mhz
  #define F_CPUM 120
  #define STM32F2XX
  #include "stm32f2xx.h"
#endif

//LCD interface
#if defined(TFT35_V1_0) || defined(TFT35_V1_1)
  #define RM68042
  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 0
#elif defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)
  #define ILI9488
  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 1
#elif defined(TFT28_V1_0)
  #define ILI9341
  #define STM32_HAS_FSMC
  #define LCD_DATA_16BIT 1
#elif defined(TFT24_V1_1)
  #define ILI9341
  #define LCD_DATA_16BIT 1
#endif

//LCD Backlight pin (PWM can adjust brightness)
#if defined(TFT24_V1_1)
  #define LCD_LED_PIN   PA8
#elif defined(TFT35_V3_0)
  #define LCD_LED_PIN   PD12
#endif

#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28_V1_0) || defined(TFT24_V1_1)
  #define SERIAL_PORT   _USART1  //default usart port
#elif defined(TFT35_V2_0)
  #define SERIAL_PORT   _USART1  //default usart port
  #define SERIAL_PORT_2 _USART2
  #define SERIAL_PORT_3 _USART3
#elif defined(TFT35_V3_0)
  #define SERIAL_PORT   _USART2  //default usart port
  #define SERIAL_PORT_2 _USART1
  #define SERIAL_PORT_3 _USART3
  #define SERIAL_PORT_4 _UART4
#endif

//XPT2046 Software SPI Pins (touch screen ic)
//need CS/SCK/MISO/MOSI for Software SPI, and TPEN for pen interrupt
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28_V1_0)
  #define XPT2046_CS    PC0
  #define XPT2046_SCK   PC1
  #define XPT2046_MISO  PC2
  #define XPT2046_MOSI  PC3
  #define XPT2046_TPEN  PC4
#elif defined(TFT24_V1_1)
  #define XPT2046_CS    PA3
  #define XPT2046_SCK   PA2
  #define XPT2046_MISO  PA1
  #define XPT2046_MOSI  PA0
  #define XPT2046_TPEN  PA15
#elif defined(TFT35_V2_0)
  #define XPT2046_CS    PC0
  #define XPT2046_SCK   PB3
  #define XPT2046_MISO  PB4
  #define XPT2046_MOSI  PB5
  #define XPT2046_TPEN  PC1
#elif defined(TFT35_V3_0)
  #define XPT2046_CS    PE6
  #define XPT2046_SCK   PE5
  #define XPT2046_MISO  PE4
  #define XPT2046_MOSI  PE3
  #define XPT2046_TPEN  PC13
#else
  #error "xpt2046 need to Implement here by yourself."
#endif

//SD Card SPI pins
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28_V1_0)
  #define SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7
  #define SD_HIGH_SPEED 0
  #define SD_SPI        _SPI2
  #define SPI2_CS_PIN   PB12
#elif defined(TFT24_V1_1) || defined(TFT35_V3_0)
  #define SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7
  #define SD_HIGH_SPEED 1
  #define SD_SPI        _SPI1
  #define SPI1_CS_PIN   PA4
#elif defined(TFT35_V2_0)
  #define SD_SDIO_SUPPORT
#endif

//SD Card CD detect pin
#if defined(TFT28_V1_0)
  #define SD_CD_PIN     PB0
#elif defined(TFT24_V1_1)
  #define SD_CD_PIN     PA13
#elif defined(TFT35_V2_0)
  #define SD_CD_PIN     PC6
#elif defined(TFT35_V3_0)
  #define SD_CD_PIN     PC4
#endif

//W25Qxx SPI pins
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28_V1_0) || defined(TFT35_V2_0)
  #define W25Qxx_SPEED  1
  #define W25Qxx_SPI    _SPI1
  #define SPI1_CS_PIN   PA4
#elif defined(TFT24_V1_1)
  #define W25Qxx_SPEED  0
  #define W25Qxx_SPI    _SPI3
  #define SPI3_CS_PIN   PD2
#elif defined(TFT35_V3_0)
  #define W25Qxx_SPEED  1
  #define W25Qxx_SPI    _SPI3
  #define SPI3_CS_PIN   PB6
#endif

//ST7920 Simulator SPI pins
#if defined(TFT24_V1_1) || defined(TFT35_V3_0)
  #define ST7920_SPI    _SPI2
#endif

//buzzer support
#if defined(TFT24_V1_1)
  #define BUZZER_PIN    PA14
#elif defined(TFT35_V2_0)
  #define BUZZER_PIN    PB2
#elif defined(TFT35_V3_0)
  #define BUZZER_PIN    PD13
#endif

//LCD Encoder support
#if defined(TFT24_V1_1)
  //PB0:ENC-A PB1:ENC-B PB2:BTN 
  #define LCD_ENCA_PIN  PB0
  #define LCD_ENCB_PIN  PB1
  #define LCD_BTN_PIN   PB2
#elif defined(TFT35_V3_0)
  //PA8:ENC-A PC9:ENC-B PC8:BTN 
  #define LCD_ENCA_PIN  PA8
  #define LCD_ENCB_PIN  PC9
  #define LCD_BTN_PIN   PC8
#endif
#define LCD_ENCODER_SUPPORT (defined(LCD_ENCA_PIN) && defined(LCD_ENCB_PIN) && defined(LCD_BTN_PIN))

//U disk support
#if defined(TFT24_V1_1) || defined(TFT35_V3_0)
  #define U_DISK_SUPPROT
  #define USE_USB_OTG_FS
#endif

//extend function(PS_ON, filament_detect)
#if defined(TFT35_V2_0)
  #define PS_ON_PIN      PD12
  #define FIL_RUNOUT_PIN PD11
#elif defined(TFT35_V3_0)
  #define PS_ON_PIN      PA12
  #define FIL_RUNOUT_PIN PA15
#endif
    
//Debug disable, free pins for other function
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT28_V1_0) || defined(TFT35_V2_0)
  #define DISABLE_JTAG    //free JTAG(PB3/PB4) for SPI3
#elif defined(TFT24_V1_1)
  #define DISABLE_DEBUG   //
#elif defined(TFT35_V3_0)
  //stm32f207 needn't this
#endif

//LCD resolution, font and icon size
#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)
  #define LCD_WIDTH	  480
  #define LCD_HEIGHT	320
  
  #define BYTE_HEIGHT 24
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
  
  #define ICON_WIDTH  95
  #define ICON_HEIGHT 95
  #define TITLE_END_Y 60
  
  #define LIST_ICON_WIDTH   85
  #define LIST_ICON_HEIGHT  70

  #define selecticonw 95
#elif defined(TFT28_V1_0) || defined(TFT24_V1_1)
  #define LCD_WIDTH	  320
  #define LCD_HEIGHT	240
  
  #define BYTE_HEIGHT 16
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
  
  #define ICON_WIDTH  70
  #define ICON_HEIGHT 70
  #define TITLE_END_Y 40
  
  #define LIST_ICON_WIDTH   55
  #define LIST_ICON_HEIGHT  50
  
  #define selecticonw 70
#endif

#if defined(TFT35_V1_0) || defined(TFT35_V1_1) || defined(TFT35_V1_2)|| defined(TFT35_V2_0) || defined(TFT35_V3_0)
  #define ROOT_DIR "TFT35"
#elif defined(TFT28_V1_0)
  #define ROOT_DIR "TFT28"
#elif defined(TFT24_V1_1)
  #define ROOT_DIR "TFT24"
#endif

//hardware version config
#ifndef HARDWARE_VERSION
  #if defined(TFT35_V1_0)
    #define HARDWARE_VERSION "TFT35_V1.0"   
  #elif defined(TFT35_V1_1)
    #define HARDWARE_VERSION "TFT35_V1.1"
  #elif defined(TFT35_V1_2)
    #define HARDWARE_VERSION "TFT35_V1.2"
  #elif defined(TFT28_V1_0)
    #define HARDWARE_VERSION "TFT28_V1_0"
  #elif defined(TFT24_V1_1)
    #define HARDWARE_VERSION "TFT24_V1.1"
  #elif defined(TFT35_V2_0)
    #define HARDWARE_VERSION "TFT35_V2.0"
  #elif defined(TFT35_V3_0)
    #define HARDWARE_VERSION "TFT35_V3.0"
  #endif
#endif

#endif
