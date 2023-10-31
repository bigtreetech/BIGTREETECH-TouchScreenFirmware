#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for RCC_ClocksTypeDef
#include "uart.h"      // for _UART_CNT

#define MAX_MENU_DEPTH      10  // max sub menu depth
#define BE_PRIORITY_DIVIDER 16  // a divider value of 16 -> run 6% of the time only. Use a power of 2 for performance reasons!
#define FE_PRIORITY_DIVIDER 16  // a divider value of 16 -> run 6% of the time only. Use a power of 2 for performance reasons!

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

typedef enum
{
  HOST_SLOTS_GENERIC_OK = -1,
} HOST_SLOTS;

typedef struct
{
  uint8_t target_tx_slots;  // keep track of target gcode tx slots (e.g. if ADVANCED_OK feature is enabled on both mainboard and TFT)
  uint8_t tx_slots;         // keep track of available gcode tx slots (e.g. if ADVANCED_OK feature is enabled on both mainboard and TFT)
  uint8_t tx_count;         // keep track of pending gcode tx count
  bool connected;           // TFT is connected to Marlin
  bool listening_mode;      // TFT is in listening mode from Marlin
  HOST_STATUS status;       // host is busy in printing execution. (USB serial printing and gcode print from onboard)
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
extern uint32_t bePriorityCounter;
extern uint32_t fePriorityCounter;

void InfoHost_Init(bool isConnected);
void InfoHost_UpdateListeningMode(void);

// handle OK response:
//   - tx_slots (used/effective only in case "advanced_ok" configuration setting is also enabled in TFT):
//     - < 0 (HOST_SLOTS_GENERIC_OK): to increase infoHost.tx_slots up to current target and decrease infoHost.tx_count by 1
//     - >= 0: to handle static ADVANCED_OK and Marlin ADVANCED_OK
void InfoHost_HandleOkAck(int16_t tx_slots);

#ifdef __cplusplus
}
#endif

#endif
