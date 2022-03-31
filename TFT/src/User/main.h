#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for RCC_ClocksTypeDef
#include "uart.h"      // for _UART_CNT

#define MAX_MENU_DEPTH 10        // Max sub menu depth
typedef void (* FP_MENU)(void);

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // Menu function buffer
  uint8_t cur;                   // Current menu index in buffer
} MENU;

typedef enum
{
  HOST_STATUS_IDLE = 0,
  HOST_STATUS_STOPPING,
  HOST_STATUS_PRINTING,
  HOST_STATUS_RESUMING,
  HOST_STATUS_PAUSED,
  HOST_STATUS_PAUSING
} HOST_STATUS;

typedef struct
{
  bool wait;              // Whether wait for Marlin's response
  bool rx_ok[_UART_CNT];  // Whether receive Marlin's response or get gcode by other UART (ESP3D/OctoPrint)
  bool connected;         // Whether have connected to Marlin
  HOST_STATUS status;     // Whether the host is busy in printing execution. (USB serial printing and gcode print from onboard)
} HOST;

typedef struct
{
  RCC_ClocksTypeDef rccClocks;
  uint32_t PCLK1_Timer_Frequency;
  uint32_t PCLK2_Timer_Frequency;
} CLOCKS;

extern MENU infoMenu;
extern HOST infoHost;
extern CLOCKS mcuClocks;

#ifdef __cplusplus
}
#endif

#endif
