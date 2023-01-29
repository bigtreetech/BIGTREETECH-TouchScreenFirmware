#ifndef _PIN_MKS_GD_V2_TFT28_V4_0_H_  // modify to actual filename !!!
#define _PIN_MKS_GD_V2_TFT28_V4_0_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx, STM32F4xx, gd32f20x, gd32f30x)
#ifndef MCU_TYPE
  #define MCU_TYPE
  #include "gd32f30x.h"
#endif

// Update folder for fonts and icons
#ifndef UPDATE_DIR
  #define UPDATE_DIR "TFT28"
#endif

// Hardware version config
#ifndef HARDWARE_VERSION
  #define HARDWARE_VERSION "GD_V2_TFT28_V4.0"
#endif

// USB Disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_FS
  #define USE_USB_OTG_FS
#endif

#include "pin_MKS_TFT32_V1_4.h"

#endif
