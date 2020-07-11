#include "includes.h"
#include "Temperature.h"


const char* toolID[]              = HEAT_SIGN_ID;
const char* const heatDisplayID[] = HEAT_DISPLAY_ID;
const char* heatCmd[]             = HEAT_CMD;
const char* heatWaitCmd[]         = HEAT_WAIT_CMD;

static HEATER  heater               = {{}, NOZZLE0, NOZZLE0};
static HEATER  lastHeater           = {{}, NOZZLE0, NOZZLE0};
static u32     heat_update_time     = TEMPERATURE_QUERY_SLOW_DURATION;
static bool    heat_update_waiting  = false;
static bool    heat_send_waiting[HEATER_NUM];

u32 nextHeatCheckTime = 0;

//Set target temperature
void heatSetTargetTemp(TOOL tool, u16 temp)
{
  heater.T[tool].target = temp;
}
//Sync target temperature
void heatSyncTargetTemp(TOOL tool, u16 temp)
{
  lastHeater.T[tool].target = heater.T[tool].target = temp;
}

//Get target temperature
u16 heatGetTargetTemp(TOOL tool)
{
  return heater.T[tool].target;
}

// Set current temperature
void heatSetCurrentTemp(TOOL tool, s16 temp)
{
  heater.T[tool].current = limitValue(-99, temp, 999);
}

// Get current temperature
s16 heatGetCurrentTemp(TOOL tool)
{
  return heater.T[tool].current;
}

// Is heating waiting to heat up
bool heatGetIsWaiting(TOOL tool)
{
  return (heater.T[tool].waiting != WAIT_NONE);
}

// Check all heater if there is a heater waiting to be waited
bool heatHasWaiting(void)
{
  TOOL i;
  for(i = BED; i < HEATER_COUNT; i++)
  {
    if(heater.T[i].waiting != WAIT_NONE)
      return true;
  }
  return false;
}

// Set heater waiting status
void heatSetIsWaiting(TOOL tool, HEATER_WAIT isWaiting)
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
  for(TOOL i = BED; i < HEATER_COUNT; i++)
  {
    heater.T[i].waiting = WAIT_NONE;
  }
  heat_update_time = TEMPERATURE_QUERY_SLOW_DURATION;
}

// Set current heater tool, nozzle or hot bed
void heatSetCurrentTool(TOOL tool)
{
  if(tool >= HEATER_COUNT) return;
  heater.tool = tool;
}
// Get current tool, nozzle or hot bed
TOOL heatGetCurrentTool(void)
{
  return heater.tool;
}

// check if current tool index is changed
bool heatToolChanged(void)
{
  if (lastHeater.tool != heater.tool)
  {
    lastHeater.tool = heater.tool;
    return true;
  }
  else
  {
    return false;
  }
}

// Set current nozzle
void heatSetCurrentToolNozzle(TOOL tool)
{
  if(tool >= HEATER_COUNT || tool < NOZZLE0) return;
  heater.nozzle = tool;
  heater.tool = tool;
}

// Get current nozzle
TOOL heatGetCurrentToolNozzle(void)
{
  return heater.nozzle;
}

// check if current nozzle index is changed
bool heatNozzleChanged(void)
{
  if (lastHeater.nozzle != heater.nozzle)
  {
    lastHeater.nozzle = heater.nozzle;
    return true;
  }
  else
  {
    return false;
  }
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
void heatSetSendWaiting(TOOL tool, bool isWaiting)
{
  heat_send_waiting[tool] = isWaiting;
}

// Get whether has heating command in Queue
bool heatGetSendWaiting(TOOL tool)
{
  return heat_send_waiting[tool];
}

// check if Current Temperature of a Heater has changed
bool heatCurrentTempChanged(TOOL tool)
{
  if (lastHeater.T[tool].current != heater.T[tool].current)
  {
    lastHeater.T[tool].current = heater.T[tool].current;
    return true;
  }
  else
  {
    return false;
  }
}

// check if Target temperature of a Heater has changed
bool heatTargetTempChanged(TOOL tool)
{
  if (lastHeater.T[tool].target != heater.T[tool].target)
  {
    return true;
  }
  else
  {
    return false;
  }
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
    if(RequestCommandInfoIsRunning())          break; //to avoid colision in Gcode response processing
    if(storeCmd("M105\n") == false)            break;
    updateNextHeatCheckTime();
    heat_update_waiting = true;
  }while(0);

  // Query the heater that needs to wait for the temperature to rise, whether it reaches the set temperature
  for(u8 i=0; i< HEATER_COUNT; i++)
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

  for(TOOL i = BED; i < HEATER_COUNT; i++) // If the target temperature changes, send a Gcode to set the motherboard
  {
    if(lastHeater.T[i].target != heater.T[i].target)
    {
      lastHeater.T[i].target = heater.T[i].target;
      if(heat_send_waiting[i] != true)
      {
        heat_send_waiting[i] = true;
        storeCmd("%s ",heatCmd[i]);
      }
    }
  }
}
