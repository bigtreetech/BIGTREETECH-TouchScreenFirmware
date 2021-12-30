#ifndef _PIN_TFT35_V1_2_H_  // modify to actual filename !!!
#define _PIN_TFT35_V1_2_H_  // modify to actual filename !!!

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "TFT35_V1.2"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       ILI9488  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558].
  #define TFTLCD_DRIVER_SPEED 0x03
#endif
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1  // LCD data 16bit or 8bit
#endif

#include "pin_TFT35_V1_0.h"

#endif
