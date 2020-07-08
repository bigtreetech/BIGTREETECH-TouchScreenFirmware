#ifndef _PIN_TFT35_V3_0_H_ // modify to actual filename !!!
#define _PIN_TFT35_V3_0_H_ // modify to actual filename !!!

//MCU type (STM32F10x, STM32F2xx)
#include "stm32f2xx.h"

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #include "./Resolution/TFT_480X320.h"
#endif

#ifndef ROOT_DIR
  #define ROOT_DIR "TFT35"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT35_V3.0"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER ILI9488  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558].
  #define TFTLCD_DRIVER_SPEED         0x03
  #define TFTLCD_0_DEGREE_REG_VALUE   0X28
  #define TFTLCD_180_DEGREE_REG_VALUE 0XE8
#endif
#define STM32_HAS_FSMC // FSMC 8080 interface(high speed), or normal IO interface(low speed)
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1 // LCD data 16bit or 8bit
#endif

// Debug disable, free pins for other function
//#define DISABLE_JTAG  // free JTAG(PB3/PB4) for SPI3
//#define DISABLE_DEBUG // free all pins

// LCD Backlight pin (PWM can adjust brightness)
#define LCD_LED_PIN            PD12
#define LCD_LED_PIN_ALTERNATE  GPIO_AF_TIM4
#define LCD_LED_PWM_CHANNEL    _TIM4_CH1

/*
 * SERIAL_PORT: communicating with host(Marlin, smoothieware, etc...)
 * SERIAL_PORT_X: communicating with other controller(Octoprint, ESP3D, other UART Touch Screen, etc...)
 */
#define SERIAL_PORT   _USART2  //default usart port
#define SERIAL_PORT_2 _USART1
#define SERIAL_PORT_3 _USART3
#define SERIAL_PORT_4 _UART4

//XPT2046 Software SPI Pins (touch screen ic)
//need CS/SCK/MISO/MOSI for Software SPI, and TPEN for pen interrupt
#define XPT2046_CS    PE6
#define XPT2046_SCK   PE5
#define XPT2046_MISO  PE4
#define XPT2046_MOSI  PE3
#define XPT2046_TPEN  PC13

// SD Card SPI pins
#define SD_SPI_SUPPORT
//#define SD_SDIO_SUPPORT
#ifdef SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7 // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED 0 // 2 frequency division
  #define SD_SPI        _SPI1
  #define SD_CS_PIN     PA4
#endif

// SD Card CD detect pin
#define SD_CD_PIN     PC4

// W25Qxx SPI pins
#define W25Qxx_SPEED  0
#define W25Qxx_SPI    _SPI3
#define W25Qxx_CS_PIN PB6

// ST7920 Simulator SPI pins
#define ST7920_SPI    _SPI2

// Buzzer support
#define BUZZER_PIN    PD13

// LCD Encoder support
#define LCD_ENCA_PIN  PA8
#define LCD_ENCB_PIN  PC9
#define LCD_BTN_PIN   PC8

// U disk support
#define U_DISK_SUPPORT
#define USE_USB_OTG_FS

// Extend function(PS_ON, filament_detect)
#ifndef PS_ON_PIN
  #define PS_ON_PIN      PC12 // The string on TFT35 V3.0 board(PA12) is wrong, PC12 is the correct IO
#endif
#ifndef FIL_RUNOUT_PIN
  #define FIL_RUNOUT_PIN PA15
#endif

//#define LED_COLOR_PIN PC7

#endif
