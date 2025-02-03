#ifndef _MAINBOARD_FLOW_CONTROL_H_
#define _MAINBOARD_FLOW_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for RCC_ClocksTypeDef

#define BE_PRIORITY_DIVIDER 16     // a divider value of 16 -> run 6% of the time only. Use a power of 2 for performance reasons!
#define FE_PRIORITY_DIVIDER 16     // a divider value of 16 -> run 6% of the time only. Use a power of 2 for performance reasons!
#define CMD_PREFETCH_COUNT  1      // maximum number of gcodes to prefetch from TFT media and to copy into command queue
#define ACK_TIMEOUT         15000  // 15 seconds (1 sec is 1000)
#define MAX_MENU_DEPTH      10     // max sub menu depth

// menu macros
#define OPEN_MENU(x)    infoMenu.menu[++infoMenu.cur] = x
#define REPLACE_MENU(x) infoMenu.menu[infoMenu.cur] = x
#define CLOSE_MENU()    infoMenu.cur--
#define MENU_IS(x)      infoMenu.menu[infoMenu.cur] == x
#define MENU_IS_NOT(x)  infoMenu.menu[infoMenu.cur] != x

typedef struct
{
  RCC_ClocksTypeDef rccClocks;
  uint32_t PCLK1_Timer_Frequency;
  uint32_t PCLK2_Timer_Frequency;
} CLOCKS;

typedef struct
{
  uint32_t be;  // back end
  uint32_t fe;  // front end
} PRIORITY_COUNTER;

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
  uint8_t target_tx_slots;      // keep track of target gcode tx slots (e.g. if ADVANCED_OK feature is enabled on both mainboard and TFT)
  uint8_t cur_target_tx_slots;  // keep track of current target gcode tx slots (e.g. if ADVANCED_OK feature is enabled on both mainboard and TFT)
  uint8_t tx_slots;             // keep track of available gcode tx slots (e.g. if ADVANCED_OK feature is enabled on both mainboard and TFT)
  uint8_t tx_count;             // keep track of pending gcode tx count
  uint32_t tx_delay;            // Keep track of minimum delay (in ms) to apply between last sent gcode and next one to be sent to mainboard
  uint32_t rx_timestamp;        // keep track of last received ACK message timestamp
  uint32_t rx_ok_timestamp;     // keep track of last received ACK message OK response timestamp
  bool connected;               // TFT is connected to Marlin
  bool listening_mode;          // TFT is in listening mode from Marlin
  HOST_STATUS status;           // host is busy in printing execution (printing from USB serial, (remote) onboard media or remote host)
} HOST;

typedef void (* FP_MENU)(void);

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  // menu function buffer
  uint8_t cur;                   // current menu index in buffer
} MENU;

extern CLOCKS mcuClocks;                  // system clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2
extern PRIORITY_COUNTER priorityCounter;  // priority counter
extern HOST infoHost;                     // information interaction with Marlin
extern MENU infoMenu;                     // menu structure

void resetPendingQueries(void);
void loopBackEnd(void);
void loopFrontEnd(void);
void loopProcess(void);
void loopProcessAndGUI(void);

void InfoHost_Init(bool isConnected);

void InfoHost_UpdateTargetTxSlots(uint8_t target_tx_slots);  // update target tx slots to the value detected by TFT
void InfoHost_UpdateTxDelay(void);                           // update tx delay to infoSettings.tx_delay
void InfoHost_UpdateListeningMode(void);                     // update listening mode to infoSettings.general_settings

// test if minimum delay for next gcode to send is elapsed
//
// minimum delay for the next gcode to send depends on ADVANCED_OK feature status:
//   - if disabled: the delay is applied to the last received ACK message OK response timestamp
//   - if enabled: the delay is applied to the last sent gcode timestamp (timestamp taken when the
//     gcode transmission on serial line is completed)
bool InfoHost_IsCmdDelayElapsed(void);

// test if next gcode from TFT media is sendable
//
// sendability of the next gcode from TFT media depends on TX_PREFETCH feature status:
//   - if disabled: the gcode is sendable if the command queue is empty and there is at least one free tx slot
//   - if enabled: the gcode is sendable if the count of commands in the command queue has not reached CMD_PREFETCH_COUNT
bool InfoHost_IsCmdFromTFTSendable(void);

// handle ACK message OK response:
//   - target_tx_slots (used/effective only in case ADVANCED_OK feature is also enabled in TFT):
//     - < 0 (HOST_SLOTS_GENERIC_OK): to increase infoHost.tx_slots up to current target and decrease infoHost.tx_count by 1
//     - >= 0: to handle static ADVANCED_OK and Marlin ADVANCED_OK
void InfoHost_HandleAckOk(int16_t target_tx_slots);

void InfoHost_UpdateAckTimestamp(void);  // update last received ACK message timestamp

#ifdef __cplusplus
}
#endif

#endif
