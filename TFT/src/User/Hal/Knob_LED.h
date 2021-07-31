#ifndef _KNOB_LED_H_
#define _KNOB_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "variants.h"  // for LED_COLOR_PIN

#ifdef LED_COLOR_PIN
  void knob_LED_Init(void);
  void knob_LED_DeInit(void);
  void Knob_LED_SetColor(uint32_t ws2812_dat, uint8_t neopixel_pixels);
#endif

#ifdef __cplusplus
}
#endif

#endif
