#ifndef _PIN_TFT35_V1_2_H_  // modify to actual filename !!!
#define _PIN_TFT35_V1_2_H_  // modify to actual filename !!!

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT35_V1.2"
#endif

// LCD interface
// Supported LCD drivers: [ST7789, SSD1963, RM68042, NT35310, ILI9488, ILI9341, ILI9325, HX8558]
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       ILI9488
  #define TFTLCD_DRIVER_SPEED 0x03
#endif

// LCD data 16bit or 8bit
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1
#endif

#include "pin_TFT35_V1_0.h"

#endif
