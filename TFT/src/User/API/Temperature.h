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
  FROM_HOST = 0,  // temperature status (actual/requested) from host (Marlin, Reprap, etc.)
  FROM_GUI,       // temperature requested from the TFT's GUI
  FROM_CMD,       // temperature requested in the command queue (from gcode or external source connected to the TFT)
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

extern const char *const heaterID[];
extern const char *const heatDisplayID[];
extern const char *const heatShortID[];
extern const char *const heatCmd[];
extern const char *const heatWaitCmd[];

void heatSetTargetTemp(uint8_t index, int16_t temp, TEMP_SOURCE tempSource);
uint16_t heatGetTargetTemp(uint8_t index);
void heatSetCurrentTemp(uint8_t index, int16_t temp);
int16_t heatGetCurrentTemp(uint8_t index);
void heatCoolDown(void);

bool heatGetIsWaiting(uint8_t index);
bool heatHasWaiting(void);
void heatSetIsWaiting(uint8_t index, bool isWaiting);
void heatClearIsWaiting(void);

void heatSetCurrentTool(uint8_t tool);
uint8_t heatGetCurrentTool(void);
uint8_t heatGetCurrentHotend(void);
bool heaterDisplayIsValid(uint8_t index);

void heatSetUpdateSeconds(uint8_t seconds);
uint8_t heatGetUpdateSeconds(void);
void heatSyncUpdateSeconds(uint8_t seconds);
void heatSetUpdateWaiting(bool isWaiting);

void updateNextHeatCheckTime(void);
void loopCheckHeater(void);

#ifdef __cplusplus
}
#endif

#endif
