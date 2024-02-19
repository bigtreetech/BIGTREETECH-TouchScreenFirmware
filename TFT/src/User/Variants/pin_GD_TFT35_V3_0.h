#ifndef _PIN_GD_TFT35_V3_0_H_  // modify to actual filename !!!
#define _PIN_GD_TFT35_V3_0_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx, STM32F4xx, GD32F20x, GD32F30x)
#ifndef MCU_TYPE
  #define MCU_TYPE
  #include "gd32f20x.h"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "GD_TFT35_V3.0"
#endif

// Debug support (free pins for other functions)
// Free JTAG (PB3/PB4) for SPI3
#define DISABLE_JTAG() rcu_periph_clock_enable(RCU_AF); \
                       gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE)

// SD Card SPI pins
#ifndef SD_SPI_SUPPORT
  #define SD_SPI_SUPPORT
  #define SD_LOW_SPEED   7      // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED  1      // 4 frequency division, APB2 = 120MHz / 4 = 30Mhz
  #define SD_SPI         _SPI1
  #define SD_CS_PIN      PA4
#endif

// USB Disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_FS
  #define USE_USB_OTG_FS
#endif

// LCD Backlight pins (adjust brightness with LED PWM)
#ifndef LCD_LED_PIN
  #define LCD_LED_PIN           PD12
  #define LCD_LED_PIN_ALTERNATE GPIO_TIMER3_REMAP
  #define LCD_LED_PWM_CHANNEL   _TIM4_CH1
#endif

// Knob LED Color pins
#ifndef KNOB_LED_COLOR_PIN
  #define KNOB_LED_COLOR_PIN       PC7
  #define WS2812_FAST_WRITE_HIGH() GPIO_BOP(GPIOC) = (uint32_t)1 << 7
  #define WS2812_FAST_WRITE_LOW()  GPIO_BC(GPIOC) = (uint32_t)1 << 7
#endif

// Neopixel LEDs number
#ifndef NEOPIXEL_PIXELS
  #define NEOPIXEL_PIXELS 2
#endif

#include "pin_TFT35_V3_0.h"

#endif
