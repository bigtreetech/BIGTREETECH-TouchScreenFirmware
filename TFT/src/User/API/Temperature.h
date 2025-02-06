#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "Settings.h"

#define TEMPERATURE_QUERY_FAST_SECONDS 1  // "M105" temperature query delay in heat menu or while heating.
#define TEMPERATURE_QUERY_SLOW_SECONDS 3  // temperature query delay when idle
#define TEMPERATURE_RANGE              2  // temperature difference to treat temperature reached target
#define NOZZLE_TEMP_LAG                5  // nozzle max allowed lag

typedef enum
{
  SETTLED = 0,
  HEATING,
  COOLING,
} HEATER_STATUS;

typedef enum
{
  FROM_HOST = 0,  // temperature status (actual/requested) from host (Marlin, RepRap, etc.)
  FROM_GUI,       // temperature requested from TFT's GUI
  FROM_CMD,       // temperature requested in command queue (from gcode file, TFT's GUI or external source connected to TFT) and ready to be sent to mainboard
} TEMP_SOURCE;

enum
{
  NOZZLE0 = 0,
  NOZZLE1,
  NOZZLE2,
  NOZZLE3,
  NOZZLE4,
  NOZZLE5,
  BED = MAX_HOTEND_COUNT,
  CHAMBER,
  INVALID_HEATER,
  LAST_NOZZLE,
};

typedef struct
{
  int16_t current;
  int16_t target;
  bool waiting;
  HEATER_STATUS status;
} _HEATER;

typedef struct
{
  union
  {
    struct
    {
      _HEATER hotend[MAX_HOTEND_COUNT];
      _HEATER bed;
      _HEATER chamber;
    };
    _HEATER T[MAX_HEATER_COUNT];
  };
  uint8_t toolIndex;
} HEATER;

extern const char * const heaterID[];
extern const char * const heatDisplayID[];
extern const char * const heatShortID[];
extern const char * const heatCmd[];
extern const char * const heatWaitCmd[];
extern const char * const extruderDisplayID[];
extern const char * const toolChange[];

void heatSetTargetTemp(uint8_t index, const int16_t temp, const TEMP_SOURCE tempSource);  // set target temperature
int16_t heatGetTargetTemp(uint8_t index);                    // get target temperature
void heatSetCurrentTemp(uint8_t index, const int16_t temp);  // set current temperature
int16_t heatGetCurrentTemp(uint8_t index);                   // get current temperature
void heatCoolDown(void);                                     // disable all heaters/hotends

void heatSetWaiting(uint8_t index, const bool isWaiting);  // called in sendQueueCmd(). Set heater waiting status
bool heatIsWaiting(void);                                  // called in loopPrintFromTFT(). Check for at least an heater waiting for target temperature
bool heatIsWaitingTimedout(void);                          // caslled in loopBackEnd()
void heatClearWaiting(void);                               // called in completePrint()

bool heatSetTool(const uint8_t tool);               // set current tool (extruder). Used when tool change command is from TFT
void heatSetToolIndex(const uint8_t toolIndex);     // set current Tool (extruder)
uint8_t heatGetToolIndex(void);                     // get current Tool (extruder)
uint8_t heatGetCurrentHotend(void);                 // get current hotend index in arry T[]
bool heaterDisplayIsValid(const uint8_t index);     // check whether the index is a valid heater index

void heatSetUpdateSeconds(const uint8_t seconds);   // set temperature query update time interval
uint8_t heatGetUpdateSeconds(void);                 // get temperature query seconds
void heatSyncUpdateSeconds(const uint8_t seconds);  // set temperature query seconds

void heatSetNextUpdateTime(void);                   // called in parseAck(). Set next temperature query time or timeout
void heatClearSendingWaiting(void);                 // called in sendQueueCmd(). Clear sending waiting for temperature query
void loopCheckHeater(void);                         // called in loopBackEnd(). Loop for check on Heater

#ifdef __cplusplus
}
#endif

#endif
