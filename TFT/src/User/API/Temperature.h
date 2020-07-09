#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "stdint.h"
#include "stdbool.h"
#include "Configuration.h"
#include "Settings.h"

#define TEMPERATURE_QUERY_FAST_DURATION 1000  // "M105" query temperature every 1s
#define TEMPERATURE_QUERY_SLOW_DURATION 3000  // 3s

typedef enum {
  WAIT_NONE = 0,
  WAIT_HEATING,
  WAIT_COOLING_HEATING,
}HEATER_WAIT;

typedef enum
{
  BED = 0,
  NOZZLE0 = 1,
  HEATER_NUM = MAX_HEATER_COUNT
}TOOL;

typedef struct
{
  int16_t current,
          target;
  HEATER_WAIT waiting;
}_HEATER;

typedef struct
{
  _HEATER T[HEATER_NUM];
  TOOL    tool;
  TOOL    nozzle;
}HEATER;


extern const char* toolID[];
extern const char* const heatDisplayID[];
extern const char* heatCmd[];
extern const char* heatWaitCmd[];


void heatSetTargetTemp(TOOL tool, uint16_t temp);
void heatSyncTargetTemp(TOOL tool, uint16_t temp);
uint16_t heatGetTargetTemp(TOOL tool);
void heatSetCurrentTemp(TOOL tool, int16_t temp);
int16_t heatGetCurrentTemp(TOOL tool);

void heatSetCurrentTool(TOOL tool);
TOOL heatGetCurrentTool(void);
bool heatToolChanged(void);
void heatSetCurrentToolNozzle(TOOL tool);
TOOL heatGetCurrentToolNozzle(void);
bool heatNozzleChanged(void);

bool heatGetIsWaiting(TOOL tool);
bool heatHasWaiting(void);
void heatSetIsWaiting(TOOL tool,HEATER_WAIT isWaiting);
void heatClearIsWaiting(void);

void updateNextHeatCheckTime(void);
void heatSetUpdateTime(uint32_t time);
void heatSetUpdateWaiting(bool isWaiting);
void heatSetSendWaiting(TOOL tool, bool isWaiting);
bool heatGetSendWaiting(TOOL tool);
bool heatCurrentTempChanged(TOOL tool);
bool heatTargetTempChanged(TOOL tool);


void loopCheckHeater(void);



#endif
