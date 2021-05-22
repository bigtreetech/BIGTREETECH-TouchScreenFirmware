#include "Temperature.h"
#include "includes.h"

const char *const heaterID[MAX_HEATER_COUNT]      = HEAT_SIGN_ID;
const char *const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char *const heatCmd[MAX_HEATER_COUNT]       = HEAT_CMD;
const char *const heatWaitCmd[MAX_HEATER_COUNT]   = HEAT_WAIT_CMD;

static HEATER  heater = {{}, NOZZLE0};
static int16_t lastTarget[MAX_HEATER_COUNT] = {0};
static uint8_t heat_update_seconds = TEMPERATURE_QUERY_SLOW_SECONDS;
static bool    heat_update_waiting = false;
static bool    heat_send_waiting[MAX_HEATER_COUNT];

uint32_t nextHeatCheckTime = 0;
#define AUTOREPORT_TIMEOUT (nextHeatCheckTime + 3000)  // update interval + 3 second grace period

// Verify that the heater index is valid, and fix the index of multiple in and 1 out tool nozzles
static uint8_t heaterIndexFix(uint8_t index)
{
  if (index == BED && infoSettings.bed_en)  // Bed
    return index;
  if (index == CHAMBER && infoSettings.chamber_en)  // Chamber
    return index;
  if (index < infoSettings.hotend_count)  // Vaild tool nozzle
    return index;
  if (index < infoSettings.ext_count && infoSettings.hotend_count == 1)  // "multi-extruder" that shares a single nozzle.
  {
    return NOZZLE0;
  }

  return INVALID_HEATER;  // Invalid heater
}

// Set target temperature
void heatSetTargetTemp(uint8_t index, int16_t temp)
{
  index = heaterIndexFix(index);
  if (index == INVALID_HEATER)
    return;

  heater.T[index].target = NOBEYOND(0, temp, infoSettings.max_temp[index]);
  if (heater.T[index].target + TEMPERATURE_RANGE > heater.T[index].current)
  {
    heater.T[index].status = HEATING;
  }
  if (heater.T[index].target < heater.T[index].current + TEMPERATURE_RANGE)
  {
    heater.T[index].status = COOLING;
  }
  if (inRange(heater.T[index].current, heater.T[index].target, TEMPERATURE_RANGE))
  {
    heater.T[index].status = SETTLED;
  }
}

// Sync target temperature
void heatSyncTargetTemp(uint8_t index, int16_t temp)
{
  index = heaterIndexFix(index);
  if (index == INVALID_HEATER)
    return;

  lastTarget[index] = heater.T[index].target = temp;
}

// Get target temperature
uint16_t heatGetTargetTemp(uint8_t index)
{
  index = heaterIndexFix(index);
  if (index == INVALID_HEATER)
    return 0;

  return heater.T[index].target;
}

// Set current temperature
void heatSetCurrentTemp(uint8_t index, int16_t temp)
{
  index = heaterIndexFix(index);
  if (index == INVALID_HEATER)
    return;

  heater.T[index].current = NOBEYOND(-99, temp, 999);

  if (infoMachineSettings.autoReportTemp)
    updateNextHeatCheckTime();  // set next timeout for temperature auto-report
}

// Get current temperature
int16_t heatGetCurrentTemp(uint8_t index)
{
  index = heaterIndexFix(index);
  if (index == INVALID_HEATER)
    return 0;

  return heater.T[index].current;
}

// Disable all heater/hotends
void heatCoolDown(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heatSetTargetTemp(i, 0);
  }
}

// Is heating waiting to heat up
bool heatGetIsWaiting(uint8_t index)
{
  return (heater.T[index].waiting != WAIT_NONE);
}

// Check all heater if there is a heater waiting to be waited
bool heatHasWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting != WAIT_NONE)
      return true;
  }
  return false;
}

// Set heater waiting status
void heatSetIsWaiting(uint8_t index, HEATER_WAIT isWaiting)
{
  index = heaterIndexFix(index);
  if (index == INVALID_HEATER)
    return;

  heater.T[index].waiting = isWaiting;

  if (isWaiting != WAIT_NONE)  // wait heating now, query more frequently
  {
    heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
  }
  else if (heatHasWaiting() == false)
  {
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }
}

void heatClearIsWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heater.T[i].waiting = WAIT_NONE;
  }
  heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

// Set current Tool (Extruder)
void heatSetCurrentTool(uint8_t tool)
{
  if (tool >= infoSettings.ext_count)
    return;
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
bool heaterDisplayIsValid(uint8_t index)
{
  if (index >= infoSettings.hotend_count && index < MAX_HOTEND_COUNT)
    return false;
  if (!infoSettings.bed_en && index == BED)
    return false;
  if (!infoSettings.chamber_en && index == CHAMBER)
    return false;
  return true;
}

// Set temperature update time interval
void heatSetUpdateSeconds(uint8_t seconds)
{
  if (heat_update_seconds == seconds) return;

  heat_update_seconds = seconds;
  if (infoMachineSettings.autoReportTemp && !heat_update_waiting)
  {
    heat_update_waiting = storeCmd("M155 ");
  }
}

// Get query temperature seconds
uint8_t heatGetUpdateSeconds(void)
{
  return heat_update_seconds;
}

// Set query temperature seconds
void heatSyncUpdateSeconds(uint8_t seconds)
{
  heat_update_seconds = seconds;
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
  nextHeatCheckTime = OS_GetTimeMs() + heat_update_seconds * 1000;
}

void loopCheckHeater(void)
{
  // Send M105 to query the temperatures, if motherboard does not supports M155 (AUTO_REPORT_TEMPERATURES) feature
  // to automatically report the temperatures.
  if (!infoMachineSettings.autoReportTemp)
  {
    do
    {
      // Send M105 query temperature continuously
      if (heat_update_waiting == true)
      {
        updateNextHeatCheckTime();
        break;
      }
      if (OS_GetTimeMs() < nextHeatCheckTime)
        break;
      if (requestCommandInfoIsRunning())  // To avoid colision in Gcode response processing
        break;
      if (storeCmd("M105\n") == false)
        break;
      updateNextHeatCheckTime();
      heat_update_waiting = true;
    } while (0);
  }
  else  // check temperature auto-report timout and resend M155 command
  {
    if (OS_GetTimeMs() > AUTOREPORT_TIMEOUT && !heat_update_waiting)
    {
      heat_update_waiting = storeCmd("M155 ");
      if (heat_update_waiting) updateNextHeatCheckTime();  // set next timeout for temperature auto-report
    }
  }

  // Query the heater that needs to wait for the temperature to rise, whether it reaches the set temperature
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting == WAIT_NONE)
    {
      continue;
    }
    else if (heater.T[i].waiting == WAIT_HEATING)
    {
      if (heater.T[i].current + TEMPERATURE_RANGE <= heater.T[i].target)
        continue;
    }
    else if (heater.T[i].waiting == WAIT_COOLING_HEATING)
    {
      if (inRange(heater.T[i].current, heater.T[i].target, TEMPERATURE_RANGE) == false)
        continue;
    }

    heater.T[i].waiting = WAIT_NONE;
    if (heatHasWaiting())
      continue;

    if (infoMenu.menu[infoMenu.cur] == menuHeat)
      break;
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }

  // Query heaters if they reached the target temperature (only if not prining)
  for (uint8_t i = 0; (i < MAX_HEATER_COUNT) && (!isPrinting()); i++)
  {
    if (heater.T[i].status != SETTLED && inRange(heater.T[i].current, heater.T[i].target, TEMPERATURE_ALERT_RANGE))
    {
      switch (heater.T[i].status)
      {
        case HEATING:
          BUZZER_PLAY(sound_heated);
          break;

        case COOLING:
          BUZZER_PLAY(sound_cooled);
          break;

        default:
          break;
      }
      heater.T[i].status = SETTLED;
    }
  }

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)  // If the target temperature changes, send a Gcode to set the motherboard
  {
    if (lastTarget[i] != heater.T[i].target)
    {
      lastTarget[i] = heater.T[i].target;
      if (heat_send_waiting[i] != true)
      {
        heat_send_waiting[i] = storeCmd("%s ", heatCmd[i]);
      }
    }
  }
}
