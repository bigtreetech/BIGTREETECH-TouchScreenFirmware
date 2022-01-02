#ifndef _PIN_TFT35_V1_0_H_  // modify to actual filename !!!
#define _PIN_TFT35_V1_0_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx)
#include "stm32f10x.h"

//#undef PORTRAIT_MODE  // comment this line in case the TFT variant supports Portrait Mode

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #ifdef PORTRAIT_MODE
    #include "./Resolution/TFT_320X480.h"
  #else
    #include "./Resolution/TFT_480X320.h"
  #endif
#endif

#ifndef ROOT_DIR
  #define ROOT_DIR "TFT35"
#endif

// Hardware manufacturer
#ifndef HARDWARE_MANUFACTURER
  #define HARDWARE_MANUFACTURER "BIGTREETECH_"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT35_V1.0"
#endif

// Software manufacturer
#ifndef SOFTWARE_MANUFACTURER
  #define SOFTWARE_MANUFACTURER HARDWARE_VERSION"."
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       RM68042  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558].
  #define TFTLCD_DRIVER_SPEED 0x03
#endif
#define STM32_HAS_FSMC  // FSMC 8080 interface(high speed), or normal IO interface(low speed)
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 0  // LCD data 16bit or 8bit
#endif

// Debug disable, free pins for other function
//#define DISABLE_JTAG   // free JTAG(PB3/PB4) for SPI3
//#define DISABLE_DEBUG  // free all pins

// LCD Backlight pin (PWM can adjust brightness)
//#define LCD_LED_PIN           PA8
//#define LCD_LED_PIN_ALTERNATE 0
//#define LCD_LED_PWM_CHANNEL   _TIM1_CH1

// SERIAL_PORT: communicating with host(Marlin, smoothieware, etc...)
// SERIAL_PORT_X: communicating with other controller(Octoprint, ESP3D, other UART Touch Screen, etc...)
#define SERIAL_PORT   _USART1  // default usart port
//#define SERIAL_PORT_2 _USART2
//#define SERIAL_PORT_3 _USART3
//#define SERIAL_PORT_4 _UART4

// XPT2046 Software SPI Pins (touch screen ic)
// need CS/SCK/MISO/MOSI for Software SPI, and TPEN for pen interrupt
#define XPT2046_CS   PC0
#define XPT2046_SCK  PC1
#define XPT2046_MISO PC2
#define XPT2046_MOSI PC3
#define XPT2046_TPEN PC4

// SD Card SPI pins
#define SD_SPI_SUPPORT
//#define SD_SDIO_SUPPORT
#ifdef SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7      // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED 0      // 2 frequency division
  #define SD_SPI        _SPI2
  #define SD_CS_PIN     PB12
#endif

// SD Card CD detect pin
//#define SD_CD_PIN PB0

// W25Qxx SPI pins
#define W25Qxx_SPEED  1      // stm32f103 spi1 max 4 division
#define W25Qxx_SPI    _SPI1
#define W25Qxx_CS_PIN PA4

// ST7920 Emulator SPI pins
//#define ST7920_EMULATOR  // uncomment to enable Marlin mode
//#ifdef ST7920_EMULATOR
//  #define ST7920_SPI _SPI2
//#endif

// Buzzer support
//#define BUZZER_PIN PA14

// LCD Encoder support
//#define LCD_ENCA_PIN PB0
//#define LCD_ENCB_PIN PB1
//#define LCD_BTN_PIN  PB2

// U disk support
//#define U_DISK_SUPPORT
//#define USE_USB_OTG_FS

// Extend function(PS_ON, filament_detect)
#ifndef PS_ON_PIN
  //#define PS_ON_PIN PD12
#endif

#ifndef FIL_RUNOUT_PIN
  //#define FIL_RUNOUT_PIN PD11
#endif

//#define LED_COLOR_PIN PC7

#endif
