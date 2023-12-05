#ifndef _PIN_GD_TFT43_V3_0_H_  // modify to actual filename !!!
#define _PIN_GD_TFT43_V3_0_H_  // modify to actual filename !!!

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #ifdef PORTRAIT_MODE
    #include "./Resolution/TFT_272X480.h"
  #else
    #include "./Resolution/TFT_480X272.h"
  #endif
#endif

// Update folder for fonts and icons
#ifndef UPDATE_DIR
  #define UPDATE_DIR "TFT43"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "GD_TFT43_V3.0"
#endif

// LCD interface
// Supported LCD drivers: [ST7789, SSD1963, RM68042, NT35310, ILI9488, ILI9341, ILI9325, HX8558]
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       SSD1963
  #define TFTLCD_DRIVER_SPEED 0x10     // SSD1963 needs slower speed
#endif

// Only for TFTLCD_DRIVER SSD1963
#ifndef SSD1963_LCD_PARA
  #define SSD1963_LCD_PARA
  #define SSD_DCLK_FREQUENCY  12  // 12Mhz

  #define SSD_HOR_PULSE_WIDTH 1
  #define SSD_HOR_BACK_PORCH  43
  #define SSD_HOR_FRONT_PORCH 2

  #define SSD_VER_PULSE_WIDTH 1
  #define SSD_VER_BACK_PORCH  12
  #define SSD_VER_FRONT_PORCH 1
#endif

#include "pin_GD_TFT35_V3_0.h"

#endif
