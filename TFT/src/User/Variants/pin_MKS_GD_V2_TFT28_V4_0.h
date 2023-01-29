#ifndef _PIN_MKS_GD_V2_TFT28_V4_0_H_  // modify to actual filename !!!
#define _PIN_MKS_GD_V2_TFT28_V4_0_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx, STM32F4xx)
#ifndef MCU_TYPE
  #define MCU_TYPE
  #include "gd32f30x.h"
  
#endif



// Debug support (free pins for other functions)
// Free JTAG (PB3/PB4) for SPI3
#define DISABLE_JTAG() rcu_periph_clock_enable(RCU_AF); \
                      gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE)



// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #ifdef PORTRAIT_MODE
    #include "./Resolution/TFT_240X320.h"
  #else
    #include "./Resolution/TFT_320X240.h"
  #endif
#endif

// Update folder for fonts and icons
#ifndef UPDATE_DIR
  #define UPDATE_DIR "TFT28"
#endif

// Hardware manufacturer
#ifndef HARDWARE_MANUFACTURER
  #define HARDWARE_MANUFACTURER "MKS_"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "GD_V2_TFT28_V4.0"
#endif

// Software manufacturer
#ifndef SOFTWARE_MANUFACTURER
  #define SOFTWARE_MANUFACTURER HARDWARE_VERSION"."
#endif

// XPT2046 Software SPI pins for touch screen
// It needs CS/SCK/MISO/MOSI for Software SPI and TPEN for pen interrupt
#define XPT2046_CS   PC9
#define XPT2046_SCK  PC10
#define XPT2046_MISO PC11
#define XPT2046_MOSI PC12
#define XPT2046_TPEN PC5

// W25Qxx SPI Flash Memory pins
#define W25Qxx_SPEED  1
#define W25Qxx_SPI    _SPI1
#define W25Qxx_CS_PIN PB9

// LCD interface
// Supported LCD drivers: [ST7789, SSD1963, RM68042, NT35310, ILI9488, ILI9341, ILI9325, HX8558]
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       (ILI9488 | NT35310 | ST7796S)
  #define TFTLCD_DRIVER_SPEED 0x03
#endif

// FSMC 8080 interface (high speed) or normal IO interface (low speed)
#ifndef STM32_HAS_FSMC
  // #define STM32_HAS_FSMC
#endif

// LCD data 16bit or 8bit
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1
#endif

// SERIAL_PORT:   communicating with host (Marlin, RRF etc...)
// SERIAL_PORT_X: communicating with other controllers (OctoPrint, ESP3D, other UART Touch Screen etc...)
#ifndef SERIAL_PORT
  #define SERIAL_PORT   _USART2  // default USART port
  #define SERIAL_PORT_2 _USART1
  #define SERIAL_PORT_3 _USART3
  //#define SERIAL_PORT_4 _UART4
#endif

// Serial port for debugging
#ifdef SERIAL_DEBUG_ENABLED
  #define SERIAL_DEBUG_PORT SERIAL_PORT_3
#endif

// SD Card SPI pins
#ifndef SD_SPI_SUPPORT
  #define SD_SPI_SUPPORT
  #define SD_LOW_SPEED   7      // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED  1      // 4 frequency division
  #define SD_SPI         _SPI1
  #define SD_CS_PIN      PA4
#endif

// SD Card CD Detect pin
//#ifndef SD_CD_PIN
 // #define SD_CD_PIN PB15
//#endif

// USB Disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_FS
  #define USE_USB_OTG_FS
#endif


// Filament Runout Detection pin
//#ifndef FIL_RUNOUT_PIN
 // #define FIL_RUNOUT_PIN PA15    // extruder T0
  //#define FIL_RUNOUT_PIN_1 PC12  // extruder T1. Attention: It's the same pin as PS_ON
  //#define FIL_RUNOUT_PIN_2 PB10  // extruder T2.            It's the same pin as USART3 TX
  //#define FIL_RUNOUT_PIN_3 PB11  // extruder T3.            It's the same pin as USART3 RX
  //#define FIL_RUNOUT_PIN_4 PA0   // extruder T4.            It's the same pin as USART4 TX
  //#define FIL_RUNOUT_PIN_5 PA1   // extruder T5.            It's the same pin as USART4 RX
//#endif

// Buzzer PWM pin
//#ifndef BUZZER_PIN
 // #define BUZZER_PIN PA2
//#endif


// ST7920 Emulator SPI pins
#define ST7920_EMULATOR  // uncomment to enable Marlin mode
#ifdef ST7920_EMULATOR
  #define ST7920_SPI _SPI3
#endif

// HD44780 Emulator pins
#define LCD2004_EMULATOR
#ifdef LCD2004_EMULATOR
  #define LCD_EN      PB15
  #define LCD_RS      PB12
  #define LCD_D4      PB13
  #define LCD_D5      PB14
  #define LCD_D6      PC7
  #define LCD_D7      PC6
  #define LCD_D4_PIN  GPIO_Pin_13
  #define LCD_D5_PIN  GPIO_Pin_14
  #define LCD_D6_PIN  GPIO_Pin_7
  #define LCD_D7_PIN  GPIO_Pin_6
  #define LCD_D4_PORT GPIOB
  #define LCD_D5_PORT GPIOB
  #define LCD_D6_PORT GPIOC
  #define LCD_D7_PORT GPIOC
#endif

// Filament Runout Detection pin
#ifndef FIL_RUNOUT_PIN
  #define FIL_RUNOUT_PIN PB1
#endif


// Neopixel LEDs number
#ifndef NEOPIXEL_PIXELS
  #define NEOPIXEL_PIXELS 1
#endif

#endif