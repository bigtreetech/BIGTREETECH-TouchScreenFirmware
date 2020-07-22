#ifndef _PIN_TFT43_V3_0_H_ // modify to actual filename !!!
#define _PIN_TFT43_V3_0_H_ // modify to actual filename !!!

#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #include "./Resolution/TFT_480X272.h"
#endif

#ifndef ROOT_DIR
  #define ROOT_DIR "TFT43"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT43_V3.0"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER SSD1963  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558, SSD1963].
  #define TFTLCD_DRIVER_SPEED         0x10 // SSD1963 needs slower speed
  #define TFTLCD_0_DEGREE_REG_VALUE   0x00
  #define TFTLCD_180_DEGREE_REG_VALUE 0x03
#endif

// HD44780 Simulator pins
#define LCD2004_simulator
#ifdef LCD2004_simulator
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

#ifndef SSD1963_LCD_PARA
  #define SSD1963_LCD_PARA
  #define SSD_DCLK_FREQUENCY   12 // 12Mhz

  #define SSD_HOR_PULSE_WIDTH  1
  #define SSD_HOR_BACK_PORCH   43
  #define SSD_HOR_FRONT_PORCH  2

  #define SSD_VER_PULSE_WIDTH  1
  #define SSD_VER_BACK_PORCH   12
  #define SSD_VER_FRONT_PORCH  1
#endif

#ifndef NEOPIXEL_PIXELS
  #define NEOPIXEL_PIXELS  2
#endif

#include "pin_TFT35_E3_V3_0.h"

#endif
