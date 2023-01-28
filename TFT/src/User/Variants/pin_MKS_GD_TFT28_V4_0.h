#ifndef _PIN_MKS_GD_TFT28_V4_0_H_  // modify to actual filename !!!
#define _PIN_MKS_GD_TFT28_V4_0_H_  // modify to actual filename !!!

// Update folder for fonts and icons
#ifndef UPDATE_DIR
  #define UPDATE_DIR "TFT28"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "GD_TFT28_V4.0"
#endif

#ifndef TFTLCD_DRIVER
  // Works
  #define TFTLCD_DRIVER HX8558
  #define TFTLCD_DRIVER_SPEED 0x10     // SSD1963 needs slower speed
  // Wrong render
  // #define TFTLCD_DRIVER HX8558
  // White screen
  // #define TFTLCD_DRIVER ILI9325
  // Crash
  // #define TFTLCD_DRIVER       (ILI9488 | NT35310 | ST7796S)
  // #define TFTLCD_DRIVER_SPEED 0x03
  // Reversed
  // #define TFTLCD_DRIVER       ST7789
  // #define TFTLCD_DRIVER_SPEED 0x05
#endif

#include "pin_MKS_TFT32_V1_4.h"

#endif
