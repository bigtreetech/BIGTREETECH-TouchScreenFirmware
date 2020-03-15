#ifndef _HEAT_H_
#define _HEAT_H_

#include "stdint.h"
#include "stdbool.h"
#include "Configuration.h"

typedef enum
{
  BED = 0,
  NOZZLE0 = 1,
  HEATER_NUM = TOOL_NUM + NOZZLE0,
}TOOL;

typedef struct
{
  int16_t current,
          target;
  bool    waiting;
}_HEATER;

typedef struct
{
  _HEATER T[HEATER_NUM];
  TOOL    tool;
  TOOL    nozzle;
}HEATER;


extern const uint16_t   heat_max_temp[];
extern const char* toolID[];
extern const char* const heatDisplayID[];
extern const char* heatCmd[];
extern const char* heatWaitCmd[];

void menuHeat(void);
void updateLastHeatCheckTime(void);
void heatSetTargetTemp(TOOL tool, uint16_t temp);
void heatSyncTargetTemp(TOOL tool, uint16_t temp);
uint16_t heatGetTargetTemp(TOOL tool);
void heatSetCurrentTemp(TOOL tool, int16_t temp);
int16_t heatGetCurrentTemp(TOOL tool);
bool heatGetIsWaiting(TOOL tool);
bool heatHasWaiting(void);
void heatSetIsWaiting(TOOL tool,bool isWaiting);
void heatClearIsWaiting(void);
void heatSetCurrentTool(TOOL tool);
TOOL heatGetCurrentTool(void);
void heatSetCurrentToolNozzle(TOOL tool);
TOOL heatGetCurrentToolNozzle(void);
void heatSetUpdateTime(uint32_t time);
void heatSetUpdateWaiting(bool isWaiting);
void heatSetSendWaiting(TOOL tool, bool isWaiting);
void loopCheckHeater(void);

#endif
