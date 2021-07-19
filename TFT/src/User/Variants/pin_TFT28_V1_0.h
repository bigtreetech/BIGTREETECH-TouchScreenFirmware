#ifndef _PIN_TFT28_V1_0_H_ // modify to actual filename !!!
#define _PIN_TFT28_V1_0_H_ // modify to actual filename !!!

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
  #define HARDWARE_VERSION "TFT28_V1.0"
#endif

// LCD interface
#ifndef TFTLCD_DRIVER
  #define TFTLCD_DRIVER       ILI9341  // Type of LCD driver, now support[RM68042, ILI9488, ILI9341, ST7789, HX8558].
  #define TFTLCD_DRIVER_SPEED 0x03
#endif
#ifndef LCD_DATA_16BIT
  #define LCD_DATA_16BIT 1  // LCD data 16bit or 8bit
#endif

// SD Card CD detect pin
#define SD_CD_PIN PB0

#include "pin_TFT35_V1_0.h"

#endif
