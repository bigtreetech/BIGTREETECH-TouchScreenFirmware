#ifndef _PIN_TFT35_E3_V3_0_H_ // modify to actual filename !!!
#define _PIN_TFT35_E3_V3_0_H_ // modify to actual filename !!!

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT35_E3_V3.0"
#endif

#define LED_COLOR_PIN PC7
#define WS2812_FAST_WRITE_HIGH() GPIOC->BSRRL = 1<<7
#define WS2812_FAST_WRITE_LOW()  GPIOC->BSRRH = 1<<7
#ifndef NEOPIXEL_PIXELS
  #define NEOPIXEL_PIXELS  4
#endif

#include "pin_TFT35_V3_0.h"

#endif
