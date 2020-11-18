#ifndef _VARIANTS_H_
#define _VARIANTS_H_

#include "../Configuration.h"

/*
* hardware source
* TIM3 for Buzzer timer
* TIM6 for Neopixel RGB
* TIM7 for OS Timer
*/

// Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558, SSD1963].
#define RM68042 0
#define ILI9488 1
#define ILI9341 2
#define ST7789  3
#define HX8558  4
#define SSD1963 5

#if defined(TFT24_V1_1)
  #include "pin_TFT24_V1_1.h"
#elif defined(TFT28_V1_0)
  #include "pin_TFT28_V1_0.h"
#elif defined(TFT28_V3_0)
  #include "pin_TFT28_V3_0.h"
#elif defined(TFT35_V1_0)
  #include "pin_TFT35_V1_0.h"
#elif defined(TFT35_V1_1)
  #include "pin_TFT35_V1_1.h"
#elif defined(TFT35_V1_2)
  #include "pin_TFT35_V1_2.h"
#elif defined(TFT35_V2_0)
  #include "pin_TFT35_V2_0.h"
#elif defined(TFT35_V3_0)
  #include "pin_TFT35_V3_0.h"
#elif defined(TFT43_V3_0)
  #include "pin_TFT43_V3_0.h"
#elif defined(TFT50_V3_0)
  #include "pin_TFT50_V3_0.h"
#elif defined(TFT70_V3_0)
  #include "pin_TFT70_V3_0.h"
#elif defined(TFT35_E3_V3_0)
  #include "pin_TFT35_E3_V3_0.h"
#elif defined(MKS_32_V1_4)
  #include "pin_MKS_TFT32_V1_4.h"
#elif defined(MKS_28_V1_0)
  #include "pin_MKS_TFT28_V1_0.h"
#endif

#define LCD_ENCODER_SUPPORT (defined(LCD_ENCA_PIN) && defined(LCD_ENCB_PIN) && defined(LCD_BTN_PIN))
#define ENC_ACTIVE_SIGNAL (defined(LCD_ENC_EN_PIN) && defined(ST7920_SPI) && defined(LCD_ENCODER_SUPPORT))

#define LCD_DRIVER_IS(n) (TFTLCD_DRIVER == n)

#endif
