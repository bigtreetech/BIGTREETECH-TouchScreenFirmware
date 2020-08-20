#include "includes.h"
#include "Temperature.h"


const char* heaterID[MAX_HEATER_COUNT]            = HEAT_SIGN_ID;
const char* const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char* heatCmd[MAX_HEATER_COUNT]             = HEAT_CMD;
const char* heatWaitCmd[MAX_HEATER_COUNT]         = HEAT_WAIT_CMD;

static HEATER   heater = {{}, NOZZLE0};
static int16_t  lastTarget[MAX_HEATER_COUNT] = {0};
static uint32_t heat_update_time = TEMPERATURE_QUERY_SLOW_DURATION;
static bool     heat_update_waiting = false;
static bool     heat_send_waiting[MAX_HEATER_COUNT];

uint32_t nextHeatCheckTime = 0;

//Set target temperature
void heatSetTargetTemp(uint8_t index, int16_t temp)
{
  heater.T[index].target = NOBEYOND(0, temp, infoSettings.max_temp[index]);
}
//Sync target temperature
void heatSyncTargetTemp(uint8_t index, int16_t temp)
{
  lastTarget[index] = heater.T[index].target = temp;
}

//Get target temperature
u16 heatGetTargetTemp(uint8_t index)
{
  return heater.T[index].target;
}

// Set current temperature
void heatSetCurrentTemp(uint8_t index, int16_t temp)
{
  heater.T[index].current = NOBEYOND(-99, temp, 999);
}

// Get current temperature
int16_t heatGetCurrentTemp(uint8_t index)
{
  return heater.T[index].current;
}

// Is heating waiting to heat up
bool heatGetIsWaiting(uint8_t index)
{
  return (heater.T[index].waiting != WAIT_NONE);
}

// Check all heater if there is a heater waiting to be waited
bool heatHasWaiting(void)
{
  for(uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if(heater.T[i].waiting != WAIT_NONE)
      return true;
  }
  return false;
}

// Set heater waiting status
void heatSetIsWaiting(uint8_t tool, HEATER_WAIT isWaiting)
{
  heater.T[tool].waiting = isWaiting;
  if(isWaiting != WAIT_NONE) // wait heating now, query more frequently
  {
    heat_update_time = TEMPERATURE_QUERY_FAST_DURATION;
  }
  else if(heatHasWaiting() == false)
  {
    heat_update_time = TEMPERATURE_QUERY_SLOW_DURATION;
  }
}

void heatClearIsWaiting(void)
{
  for(uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heater.T[i].waiting = WAIT_NONE;
  }
  heat_update_time = TEMPERATURE_QUERY_SLOW_DURATION;
}

// Set current Tool (Extruder)
void heatSetCurrentTool(uint8_t tool)
{
  if(tool >= infoSettings.ext_count) return;
  heater.toolIndex = tool;
}

// Get current Tool (Extruder)
uint8_t heatGetCurrentTool(void)
{
  return heater.toolIndex;
}

// Get current hotend index in arry T[]
uint8_t heatGetCurrentHotend(void)
{
  return (infoSettings.hotend_count == 1) ? NOZZLE0 : heater.toolIndex;
}

// Check whether the index is a valid heater index.
bool heaterIsValid(uint8_t index)
{
  if (index >= infoSettings.hotend_count && index < MAX_HOTEND_COUNT) return false;
  if (!infoSettings.bed_en && index == BED) return false;
  if (!infoSettings.chamber_en && index == CHAMBER) return false;
  return true;
}

// Set temperature update time interval
void heatSetUpdateTime(u32 time)
{
  heat_update_time=time;
}

// Set whether we need to query the current temperature
void heatSetUpdateWaiting(bool isWaiting)
{
  heat_update_waiting = isWaiting;
}

// Set whether the heating command has been sent
void heatSetSendWaiting(uint8_t index, bool isWaiting)
{
  heat_send_waiting[index] = isWaiting;
}

// Get whether has heating command in Queue
bool heatGetSendWaiting(uint8_t index)
{
  return heat_send_waiting[index];
}

void updateNextHeatCheckTime(void)
{
  nextHeatCheckTime = OS_GetTimeMs() + heat_update_time;
}

void loopCheckHeater(void)
{
  do
  {  // Send M105 query temperature continuously
    if(heat_update_waiting == true) {updateNextHeatCheckTime();break;}
    if(OS_GetTimeMs() < nextHeatCheckTime)     break;
    if(requestCommandInfoIsRunning())          break; //to avoid colision in Gcode response processing
    if(storeCmd("M105\n") == false)            break;
    updateNextHeatCheckTime();
    heat_update_waiting = true;
  }while(0);

  // Query the heater that needs to wait for the temperature to rise, whether it reaches the set temperature
  for(uint8_t i=0; i< MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting == WAIT_NONE)                              continue;
    else if (heater.T[i].waiting == WAIT_HEATING) {
      if (heater.T[i].current+2 <= heater.T[i].target)                 continue;
    }
    else if (heater.T[i].waiting == WAIT_COOLING_HEATING) {
      if (inRange(heater.T[i].current, heater.T[i].target, 2) != true) continue;
    }

    heater.T[i].waiting = WAIT_NONE;
    if (heatHasWaiting())                                              continue;

    if(infoMenu.menu[infoMenu.cur] == menuHeat)                        break;
    heat_update_time = TEMPERATURE_QUERY_SLOW_DURATION;
  }

  for(uint8_t i = 0; i < MAX_HEATER_COUNT; i++) // If the target temperature changes, send a Gcode to set the motherboard
  {
    if(lastTarget[i] != heater.T[i].target)
    {
      lastTarget[i] = heater.T[i].target;
      if(heat_send_waiting[i] != true)
      {
        heat_send_waiting[i] = true;
        storeCmd("%s ",heatCmd[i]);
      }
    }
  }
}
