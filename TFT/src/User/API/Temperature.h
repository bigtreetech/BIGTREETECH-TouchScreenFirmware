//TG MODIFIED BY T.GIOIOSA

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "Configuration.h"
#include "Settings.h"

#define TEMPERATURE_QUERY_FAST_SECONDS 1  // "M105" query temperature every 1s
#define TEMPERATURE_QUERY_SLOW_SECONDS 1  // "M155" vquery  //TG 2/21/21 set faster was 3

typedef enum {
  WAIT_NONE = 0,
  WAIT_HEATING,
  WAIT_COOLING_HEATING,
} HEATER_WAIT;

typedef enum {
  SETTLED = 0,
  HEATING,
  COOLING,
} HEATER_STATUS;

enum
{
  TOOL0 = 0,    //TG 2/4/21  reduced tools
  TOOL1,
  BED,
  CHAMBER,
};

typedef struct
{
  int16_t current;
  int16_t target;
  HEATER_WAIT waiting;
  HEATER_STATUS status;
} _HEATER;

typedef struct
{
  union {
    struct {
      _HEATER hotend[MAX_SPINDLE_COUNT];
      _HEATER bed;
      _HEATER chamber;
    };
    _HEATER T[MAX_TOOL_COUNT];
  };
  uint8_t toolIndex;
} HEATER;

extern const char *const heaterID[];
extern const char *const heatDisplayID[];
extern const char *const heatCmd[];
extern const char *const heatWaitCmd[];

void heatSetTargetTemp(uint8_t index, int16_t temp);
void heatSyncTargetTemp(uint8_t index, int16_t temp);
uint16_t heatGetTargetTemp(uint8_t index);
void heatSetCurrentTemp(uint8_t index, int16_t temp);
int16_t heatGetCurrentTemp(uint8_t index);
void heatCoolDown(void);

void heatSetCurrentTool(uint8_t tool);
uint8_t heatGetCurrentTool(void);
uint8_t heatGetCurrentHotend(void);
bool heaterIsValid(uint8_t index);

bool heatGetIsWaiting(uint8_t index);
bool heatHasWaiting(void);
void heatSetIsWaiting(uint8_t index,HEATER_WAIT isWaiting);
void heatClearIsWaiting(void);

void updateNextHeatCheckTime(void);
void heatSetUpdateSeconds(uint8_t seconds);
uint8_t heatGetUpdateSeconds(void);
void heatSyncUpdateSeconds(uint8_t seconds);
void heatSetUpdateWaiting(bool isWaiting);
void heatSetSendWaiting(uint8_t index, bool isWaiting);
bool heatGetSendWaiting(uint8_t index);

void loopCheckHeater(void);

#ifdef __cplusplus
}
#endif

#endif
