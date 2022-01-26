#ifndef _PIN_GD_TFT35_V3_0_H_ // modify to actual filename !!!
#define _PIN_GD_TFT35_V3_0_H_ // modify to actual filename !!!

#ifndef MCU_TYPE
  #define MCU_TYPE
  #include "gd32f20x.h"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "GD_TFT35_V3.0"
#endif

// free JTAG(PB3/PB4) for SPI3
#define DISABLE_JTAG() rcu_periph_clock_enable(RCU_AF); \
                       gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE)

// LCD Backlight pin (PWM can adjust brightness)
#ifndef LCD_LED_PIN
  #define LCD_LED_PIN           PD12
  #define LCD_LED_PWM_CHANNEL   _TIM4_CH1
  #define LCD_LED_PIN_ALTERNATE GPIO_TIMER3_REMAP
#endif

#define SD_SPI_SUPPORT
#ifdef SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7      // 2^(SPEED+1) = 256 frequency division
  #define SD_HIGH_SPEED 1      // 4 frequency division, APB2 = 120MHz / 4 = 30Mhz
  #define SD_SPI        _SPI1
  #define SD_CS_PIN     PA4
#endif

// U disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_FS
#endif

// #define FIL_RUNOUT_PIN_1 PC12  //Extruder T1  Attention: Is the same pin as PS_ON.
// #define FIL_RUNOUT_PIN_2 PB10  //Extruder T2             Is the same pin as USART3 TX.
// #define FIL_RUNOUT_PIN_3 PB11  //Extruder T3             Is the same pin as USART3 RX.
// #define FIL_RUNOUT_PIN_4 PA0   //Extruder T4             Is the same pin as USART4 TX.
// #define FIL_RUNOUT_PIN_5 PA1   //Extruder T5             Is the same pin as USART4 RX.

#ifndef LED_COLOR_PIN
  #define LED_COLOR_PIN PC7
  #define WS2812_FAST_WRITE_HIGH() GPIO_BOP(GPIOC) = (uint32_t)1 << 7
  #define WS2812_FAST_WRITE_LOW()  GPIO_BC(GPIOC) = (uint32_t)1 << 7
#endif

#include "pin_TFT35_V3_0.h"

#endif
