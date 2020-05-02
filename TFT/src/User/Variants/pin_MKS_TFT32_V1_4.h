#ifndef _PIN_MKS_TFT32_V1_4_H_ // modify to actual filename !!!
#define _PIN_MKS_TFT32_V1_4_H_ // modify to actual filename !!!

//MCU type (STM32F10x, STM32F2xx)
#include "stm32f10x.h"

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #include "./Resolution/TFT_320X240.h"
#endif

#ifndef ROOT_DIR
  #define ROOT_DIR "MKS"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT32_V4.0"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER HX8558  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558].
  #define TFTLCD_0_DEGREE_REG_VALUE   0xA4
  #define TFTLCD_180_DEGREE_REG_VALUE 0XA4
#endif
//#define STM32_HAS_FSMC // FSMC 8080 interface(high speed), or normal IO interface(low speed)
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1 // LCD data 16bit or 8bit
#endif

// Debug disable, free pins for other function
//#define DISABLE_JTAG  // free JTAG(PB3/PB4) for SPI3
//#define DISABLE_DEBUG // free all pins

// LCD Backlight pin (PWM can adjust brightness)
#define LCD_LED_PIN            PD14
//#define LCD_LED_PIN_ALTERNATE  0
//#define LCD_LED_PWM_CHANNEL    _TIM4_CH3

/*
 * SERIAL_PORT: communicating with host(Marlin, smoothieware, etc...)
 * SERIAL_PORT_X: communicating with other controller(Octoprint, ESP3D, other UART Touch Screen, etc...)
 */
#define SERIAL_PORT   _USART2
#define USART2_TX_PIN PD5
#define USART2_RX_PIN PD6
#define SERIAL_PORT_2 _USART1
#define SERIAL_PORT_3 _USART3
#define USART3_TX_PIN PD8
#define USART3_RX_PIN PD9
//#define SERIAL_PORT_4 _UART4

//XPT2046 Software SPI Pins (touch screen ic)
//need CS/SCK/MISO/MOSI for Software SPI, and TPEN for pen interrupt
#define XPT2046_CS    PC9
#define XPT2046_SCK   PC10
#define XPT2046_MISO  PC11
#define XPT2046_MOSI  PC12
#define XPT2046_TPEN  PC5

// SD Card SPI pins
#define SD_SPI_SUPPORT
//#define SD_SDIO_SUPPORT
#ifdef SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7 // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED 1 // 2 frequency division
  #define SD_SPI        _SPI1
  #define SD_CS_PIN     PD11
#endif

// SD Card CD detect pin
#define SD_CD_PIN     PB15

// W25Qxx SPI pins
#define W25Qxx_SPEED  1
#define W25Qxx_SPI    _SPI1
#define W25Qxx_CS_PIN PB9

// ST7920 Simulator SPI pins
//#define ST7920_SPI    _SPI1

// Buzzer support
#define BUZZER_PIN    PA2

// LCD Encoder support
//#define LCD_ENCA_PIN  PB0
//#define LCD_ENCB_PIN  PB1
//#define LCD_BTN_PIN   PB5

// U disk support
#define U_DISK_SUPPROT
#define USE_USB_OTG_FS

// Extend function(PS_ON, filament_detect)
#ifndef PS_ON_PIN
  //#define PS_ON_PIN      PD12
#endif
#ifndef FIL_RUNOUT_PIN
  //#define FIL_RUNOUT_PIN PD11
#endif

//#define LED_COLOR_PIN PC7

#endif
