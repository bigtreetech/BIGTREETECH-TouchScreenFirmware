#ifndef _VARIANTS_H_
#define _VARIANTS_H_

#include "../Configuration.h"

/*
* hardware source
* TIM3 for Buzzer timer
* TIM6 for Neopixel RGB
* TIM7 for OS Timer
*/

// Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ILI9325, ST7789, HX8558, SSD1963].
#define RM68042 (1 << 0)
#define ILI9488 (1 << 1)
#define ILI9341 (1 << 2)
#define ST7789  (1 << 3)
#define HX8558  (1 << 4)
#define SSD1963 (1 << 5)
#define ILI9325 (1 << 6)
#define NT35310 (1 << 7)
#define ST7796S (1 << 8)

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
#elif defined(TFT35_E3_V3_0)
  #include "pin_TFT35_E3_V3_0.h"
#elif defined(TFT35_B1_V3_0)
  #include "pin_TFT35_B1_V3_0.h"
#elif defined(TFT43_V3_0)
  #include "pin_TFT43_V3_0.h"
#elif defined(TFT50_V3_0)
  #include "pin_TFT50_V3_0.h"
#elif defined(TFT70_V3_0)
  #include "pin_TFT70_V3_0.h"
#elif defined(GD_TFT35_V3_0)
  #include "pin_GD_TFT35_V3_0.h"
#elif defined(GD_TFT35_E3_V3_0)
  #include "pin_GD_TFT35_E3_V3_0.h"
#elif defined(GD_TFT35_B1_V3_0)
  #include "pin_GD_TFT35_B1_V3_0.h"
#elif defined(GD_TFT43_V3_0)
  #include "pin_GD_TFT43_V3_0.h"
#elif defined(GD_TFT50_V3_0)
  #include "pin_GD_TFT50_V3_0.h"
#elif defined(MKS_TFT28_V3_0)
  #include "pin_MKS_TFT28_V3_0.h"
#elif defined(MKS_TFT28_V4_0)
  #include "pin_MKS_TFT28_V4_0.h"
#elif defined(MKS_TFT28_NEW_GENIUS)
  #include "pin_MKS_TFT28_NEW_GENIUS.h"
#elif defined(MKS_TFT32_V1_3)
  #include "pin_MKS_TFT32_V1_3.h"
#elif defined(MKS_TFT32_V1_4)
  #include "pin_MKS_TFT32_V1_4.h"
#elif defined(MKS_TFT32L_V3_0)
  #include "pin_MKS_TFT32L_V3_0.h"
#elif defined(MKS_TFT35_V1_0)
  #include "pin_MKS_TFT35_V1_0.h"
#endif

#define LCD_ENCODER_SUPPORT (defined(LCD_ENCA_PIN) && defined(LCD_ENCB_PIN) && defined(LCD_BTN_PIN))
#define ENC_ACTIVE_SIGNAL (defined(LCD_ENC_EN_PIN) && defined(ST7920_EMULATOR) && defined(LCD_ENCODER_SUPPORT))

#if (defined(ST7920_EMULATOR) || defined(LCD2004_EMULATOR))
  #define HAS_EMULATOR
#endif

#define LCD_DRIVER_IS(n)  ((TFTLCD_DRIVER) == (n))
#define LCD_DRIVER_HAS(n) (((TFTLCD_DRIVER) & (n)) == (n))

#define _FONT_SIZE(h, w) ((h << 8) | w)
#define _FONT_H(size) (size >> 8)
#define _FONT_W(size) (size & 0xFF)

#define FONT_SIZE_NORMAL _FONT_SIZE(BYTE_HEIGHT, BYTE_WIDTH)              // Normal font for common menu etc...
#define FONT_SIZE_LARGE  _FONT_SIZE(LARGE_BYTE_HEIGHT, LARGE_BYTE_WIDTH)  // Large font for numpad menu etc...

#endif
