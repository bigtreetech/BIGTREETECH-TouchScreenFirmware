#ifndef _PIN_TFT50_V3_0_H_ // modify to actual filename !!!
#define _PIN_TFT50_V3_0_H_ // modify to actual filename !!!

#ifndef ROOT_DIR
  #define ROOT_DIR "TFT50"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT50_V3.0"
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
  #define SSD_DCLK_FREQUENCY   9 // 9Mhz

  #define SSD_HOR_PULSE_WIDTH  1
  #define SSD_HOR_BACK_PORCH   41
  #define SSD_HOR_FRONT_PORCH  2

  #define SSD_VER_PULSE_WIDTH  1
  #define SSD_VER_BACK_PORCH   10
  #define SSD_VER_FRONT_PORCH  2
#endif

#include "pin_TFT43_V3_0.h"

#endif
