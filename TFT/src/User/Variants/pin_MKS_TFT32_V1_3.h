#ifndef _PIN_MKS_TFT32_V1_3_H_ // modify to actual filename !!!
#define _PIN_MKS_TFT32_V1_3_H_ // modify to actual filename !!!

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT32_V1.3"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER ILI9325  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ILI9325, ST7789, HX8558].
#endif

#include "pin_MKS_TFT32_V1_4.h"

#endif
