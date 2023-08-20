#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for RCC_ClocksTypeDef
#include "uart.h"      // for _UART_CNT

// default available tx gcode slots used when mainboard connects to TFT or in case ADVANCED_OK feature is disabled on TFT
//
// NOTE: you can set this parameter to a value bigger than 1 in case you want to support a sort of static ADVANCED_OK
//       feature but you have to set it to a value not bigger than "BUFSIZE" configured in Marlin's Configuration_adv.h
//
#define DEFAULT_TX_SLOTS 1

#if DEFAULT_TX_SLOTS > 1
  #define MIN_TX_SLOTS() MIN(infoHost.tx_slots + 1, DEFAULT_TX_SLOTS)
#else
  #define MIN_TX_SLOTS() DEFAULT_TX_SLOTS
#endif

#define MAX_MENU_DEPTH 10  // max sub menu depth

typedef void (* FP_MENU)(void);

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // menu function buffer
  uint8_t cur;                   // current menu index in buffer
} MENU;

typedef enum
{
  HOST_STATUS_IDLE = 0,
  HOST_STATUS_PRINTING,
  HOST_STATUS_RESUMING,
  HOST_STATUS_PAUSED,
  HOST_STATUS_PAUSING
} HOST_STATUS;

typedef struct
{
  uint8_t tx_slots;    // keep track of available tx gcode slots (e.g. if ADVANCED_OK feature is enabled on both mainboard and TFT)
  bool wait;           // whether wait for Marlin's response
  bool connected;      // whether have connected to Marlin
  HOST_STATUS status;  // whether the host is busy in printing execution. (USB serial printing and gcode print from onboard)
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
