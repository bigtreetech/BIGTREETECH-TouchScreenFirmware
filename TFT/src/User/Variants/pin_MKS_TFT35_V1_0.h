#ifndef _PIN_TFT35_V1_0_H_  // modify to actual filename !!!
#define _PIN_TFT35_V1_0_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx, STM32F4xx, GD32F20x, GD32F30x)
#ifndef MCU_TYPE
  #define MCU_TYPE
  #include "stm32f4xx.h"
#endif

// Portrait Mode support
// Comment the following line in case the TFT variant supports Portrait Mode
//#undef PORTRAIT_MODE

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #ifdef PORTRAIT_MODE
    #include "./Resolution/TFT_320X480.h"
  #else
    #include "./Resolution/TFT_480X320.h"
  #endif
#endif

// Update folder for fonts and icons
#ifndef UPDATE_DIR
  #define UPDATE_DIR "TFT35"
#endif

// Hardware manufacturer
#ifndef HARDWARE_MANUFACTURER
  #define HARDWARE_MANUFACTURER "MAKERBASE_"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "MKSTFT35V1"
#endif

// Software manufacturer
#ifndef SOFTWARE_MANUFACTURER
  #define SOFTWARE_MANUFACTURER HARDWARE_VERSION"."
#endif

// XPT2046 Software SPI pins for touch screen
// It needs CS/SCK/MISO/MOSI for Software SPI and TPEN for pen interrupt
#define HW_SPI_TOUCH
#define XPT2046_SPI _SPI1

// Touch screen interface
#define XPT2046_CS   PC4
#define XPT2046_SCK  PA5
#define XPT2046_MISO PA6
#define XPT2046_MOSI PA7
#define XPT2046_TPEN PC5

// W25Qxx SPI Flash Memory pins
#define W25Qxx_SPEED  0      // affects touch screen (XPT2046), was 1 (0 is fastest, 7 is slowest)
#define W25Qxx_SPI    _SPI1
#define W25Qxx_CS_PIN PB9

// LCD interface
// Supported LCD drivers: [ST7789, SSD1963, RM68042, NT35310, ILI9488, ILI9341, ILI9325, HX8558, ST7796S]
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       (ILI9488 | NT35310)
  #define TFTLCD_DRIVER_SPEED 0x03                 // testing, was 0x10
#endif

// FSMC 8080 interface (high speed) or normal IO interface (low speed)
#ifndef STM32_HAS_FSMC
  #define STM32_HAS_FSMC
#endif

// LCD data 16bit or 8bit
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1
#endif

// SERIAL_PORT:   communicating with host (Marlin, RRF etc.)
// SERIAL_PORT_X: communicating with other controllers (OctoPrint, ESP3D, other UART Touch Screen etc.)
#ifndef SERIAL_PORT
  #define SERIAL_PORT       _USART3  // default USART port for host communication PB10 PB11
  #define SERIAL_PORT_2     _USART1  // ESP3D port PA9 PA10
  #define SERIAL_PORT_3     _USART6  // Optional debug serial port
  #define SERIAL_PORT_ESP3D _USART1  // ESP3D port
  #define SERIAL_PORT_HOST  _USART3  // default USART port for host communication
#endif

// Serial port for debugging
#ifdef SERIAL_DEBUG_ENABLED
  #define SERIAL_DEBUG_PORT SERIAL_PORT_3
#endif

// SD Card SDIO support
#ifndef SD_SDIO_SUPPORT
  #define SD_SDIO_SUPPORT
  //#define SD_D0  = PC8
  //#define SD_D1  = PC9
  //#define SD_D2  = PC10
  //#define SD_D3  = PC11
  //#define SD_CLK = PC12
  //#define SD_CMD = PD2
  //#define SD_CD  = PD3
#endif

// SD Card CD Detect pin
#ifndef SD_CD_PIN
  #define SD_CD_PIN PD3
#endif

// USB Disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_OTG_FS
#endif

// Power Supply Auto Shutdown Detection pin
#ifndef PS_ON_PIN
  #define PS_ON_PIN PE4
#endif

// Filament Runout Detection pin
#ifndef FIL_RUNOUT_PIN
  #define FIL_RUNOUT_PIN PE1
#endif

// Buzzer PWM pin
#ifndef BUZZER_PIN
  #define BUZZER_PIN           PA2
  #define BUZZER_PIN_ALTERNATE GPIO_AF_TIM5  // alternate function 2 for PWM
  #define BUZZER_PWM_CHANNEL   _TIM5_CH3
  #define BUZZER_PWM_TIMER     TIM5
#endif

// LCD Backlight pins (adjust brightness with LED PWM)
#ifndef LCD_LED_PIN
  #define LCD_LED_PIN           PA1
  #define LCD_LED_PIN_ALTERNATE GPIO_AF_TIM2
  #define LCD_LED_PWM_CHANNEL   _TIM2_CH2
  #define LCD_LED_PWM_TIMER     TIM2
#endif

// LCD Encoder pins
// It can be added using available pins (PB0, PB1, PB4, PB5). Switch encoder/button to ground
// Make sure to remap FIL_RUNOUT_PIN and PS_ON_PIN to unused pins to avoid conflicts (E.e. PE0, PE1, PE4, PE5)
// Mostly usefull for Marlin mode, which is not available because there are not EXP1/2 connectors on the display
#ifndef LCD_ENCA_PIN
  #define LCD_ENCA_PIN PB0
  #define LCD_ENCB_PIN PB1
  #define LCD_BTN_PIN  PB5
#endif

// Use AT24Cxx I2C EEPROM Serial Flash Memory for storing parameters and touch screen calibration data
// The program flash storage will be used if I2C_EEPROM is not defined
#define I2C_EEPROM
#ifdef I2C_EEPROM
  #define I2C_SPEED      400000  // 400kHz supported at 3.3V
  #define EEPROM_ADDRESS 0xA0    // address pins not used for AT24C16
  //#define I2C_PORT       I2C1    // currently hard coded to I2C1!!!
  //#define EEPROM_SCL     PB6
  //#define EEPROM_SDA     PB7
  //#define EEPROM_SIZE    (1024*16)
  //#define EEPROM_TYPE    AT24C16
#endif

#endif
