#ifndef _PIN_MKS_GD_TFT28_V1_2_4_H_  // modify to actual filename !!!
#define _PIN_MKS_GD_TFT28_V1_2_4_H_  // modify to actual filename !!!

// MCU type (STM32F10x, STM32F2xx, STM32F4xx, GD32F20x, GD32F30x)
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
  #define HARDWARE_VERSION "GD_TFT28_V1.2-4"
#endif

// SERIAL_PORT:   communicating with host (Marlin, RRF etc.)
// SERIAL_PORT_X: communicating with other controllers (OctoPrint, ESP3D, other UART Touch Screen etc.)
#ifndef SERIAL_PORT
  #define SERIAL_PORT   _USART2  // default USART port
  #define SERIAL_PORT_2 _USART3
  #define SERIAL_PORT_3 _USART1
  #define USART2_TX_PIN PD5
  #define USART2_RX_PIN PD6
  #define USART3_TX_PIN PD8
  #define USART3_RX_PIN PD9
  #define SERIAL_PORT_4 _UART4
#endif

// USB Disk support
#ifndef USB_FLASH_DRIVE_SUPPORT
  #define USB_FLASH_DRIVE_SUPPORT
  #define USE_USB_FS
  #define USE_USB_OTG_FS
#endif

#include "pin_MKS_TFT32_V1_4.h"

#endif
