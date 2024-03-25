#ifndef _PIN_TFT24_V1_1_H_  // modify to actual filename !!!
#define _PIN_TFT24_V1_1_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx, STM32F4xx, GD32F20x, GD32F30x)
#ifndef MCU_TYPE
  #define MCU_TYPE
  #include "stm32f10x.h"
#endif

// Portrait Mode support
// Comment the following line in case the TFT variant supports Portrait Mode
//#undef PORTRAIT_MODE

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
  #define UPDATE_DIR "TFT24"
#endif

// Hardware manufacturer
#ifndef HARDWARE_MANUFACTURER
  #define HARDWARE_MANUFACTURER "BIGTREETECH_"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT24_V1.1"
#endif

// Software manufacturer
#ifndef SOFTWARE_MANUFACTURER
  #define SOFTWARE_MANUFACTURER HARDWARE_VERSION"."
#endif

// Debug support (free pins for other functions)
// Free all pins
#define DISABLE_DEBUG() RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); \
                        GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE)

// XPT2046 Software SPI pins for touch screen
// It needs CS/SCK/MISO/MOSI for Software SPI and TPEN for pen interrupt
#define XPT2046_CS   PA3
#define XPT2046_SCK  PA2
#define XPT2046_MISO PA1
#define XPT2046_MOSI PA0
#define XPT2046_TPEN PA15

// W25Qxx SPI Flash Memory pins
#define W25Qxx_SPEED  0
#define W25Qxx_SPI    _SPI3
#define W25Qxx_CS_PIN PD2

// LCD interface
// Supported LCD drivers: [ST7789, SSD1963, RM68042, NT35310, ILI9488, ILI9341, ILI9325, HX8558, ST7796S]
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       ILI9341
  #define TFTLCD_DRIVER_SPEED 0x03
#endif

// FSMC 8080 interface (high speed) or normal IO interface (low speed)
#ifndef STM32_HAS_FSMC
  //#define STM32_HAS_FSMC
#endif

// LCD data 16bit or 8bit
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1
#endif

// SERIAL_PORT:   communicating with host (Marlin, RRF etc.)
// SERIAL_PORT_X: communicating with other controllers (OctoPrint, ESP3D, other UART Touch Screen etc.)
#ifndef SERIAL_PORT
  #define SERIAL_PORT   _USART1  // default USART port
  //#define SERIAL_PORT_2 _USART2
  //#define SERIAL_PORT_3 _USART3
  //#define SERIAL_PORT_4 _UART4
#endif

// Serial port for debugging
#ifdef SERIAL_DEBUG_ENABLED
  //#define SERIAL_DEBUG_PORT SERIAL_PORT_3
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
#ifndef SD_CD_PIN
  #define SD_CD_PIN PA13
#endif

// USB Disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_OTG_FS
#endif

// Buzzer PWM pin
#ifndef BUZZER_PIN
  #define BUZZER_PIN PA14
#endif

// LCD Backlight pins (adjust brightness with LED PWM)
#ifndef LCD_LED_PIN
  #define LCD_LED_PIN           PA8
  #define LCD_LED_PIN_ALTERNATE 0
  #define LCD_LED_PWM_CHANNEL   _TIM1_CH1
#endif

// LCD Encoder pins
#ifndef LCD_ENCA_PIN
  #define LCD_ENCA_PIN   PB0
  #define LCD_ENCB_PIN   PB1
  #define LCD_BTN_PIN    PB2
  #define LCD_ENC_EN_PIN PB11
#endif

// ST7920 Emulator SPI pins
#define ST7920_EMULATOR  // uncomment to enable Marlin mode
#ifdef ST7920_EMULATOR
  #define ST7920_SPI _SPI2
#endif

// HD44780 Emulator pins
#define LCD2004_EMULATOR
#ifdef LCD2004_EMULATOR
  #define LCD_EN      PB15
  #define LCD_RS      PB12
  #define LCD_D4      PB13
  #define LCD_D5      PB14
  #define LCD_D6      PB10
  #define LCD_D7      PB11
  #define LCD_EN_PIN  GPIO_Pin_15
  #define LCD_RS_PIN  GPIO_Pin_12
  #define LCD_D4_PIN  GPIO_Pin_13
  #define LCD_D5_PIN  GPIO_Pin_14
  #define LCD_D6_PIN  GPIO_Pin_10
  #define LCD_D7_PIN  GPIO_Pin_11
  #define LCD_EN_PORT GPIOB
  #define LCD_RS_PORT GPIOB
  #define LCD_D4_PORT GPIOB
  #define LCD_D5_PORT GPIOB
  #define LCD_D6_PORT GPIOB
  #define LCD_D7_PORT GPIOB
#endif

#endif
