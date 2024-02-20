#ifndef _PIN_MKS_TFT32_V1_3_H_  // modify to actual filename !!!
#define _PIN_MKS_TFT32_V1_3_H_  // modify to actual filename !!!

// Portrait Mode support
// Comment the following line in case the TFT variant supports Portrait Mode
#undef PORTRAIT_MODE

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT32_V1.3"
#endif

// LCD interface
// Supported LCD drivers: [ST7789, SSD1963, RM68042, NT35310, ILI9488, ILI9341, ILI9325, HX8558, ST7796S]
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER ILI9325
#endif

#include "pin_MKS_TFT32_V1_4.h"

#endif
