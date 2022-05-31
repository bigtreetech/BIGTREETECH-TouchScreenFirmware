#ifndef _PIN_TFT28_V3_0_H_ // modify to actual filename !!!
#define _PIN_TFT28_V3_0_H_ // modify to actual filename !!!

// LCD resolution, font and icon size
#ifndef TFT_RESOLUTION
  #define TFT_RESOLUTION
  #include "./Resolution/TFT_320X240.h"
#endif

#ifndef ROOT_DIR
  #define ROOT_DIR "TFT28"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT28_V3.0"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER               ST7789  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558].
  #define TFTLCD_DRIVER_SPEED         0x05
  #define TFTLCD_0_DEGREE_REG_VALUE   0X60
  #define TFTLCD_180_DEGREE_REG_VALUE 0XA0
#endif

#include "pin_TFT35_E3_V3_0.h"

#endif
