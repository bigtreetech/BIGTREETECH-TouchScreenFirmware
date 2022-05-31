#include "Temperature.h"
#include "includes.h"

const char *const heaterID[MAX_HEATER_COUNT]      = HEAT_SIGN_ID;
const char *const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char *const heatShortID[MAX_HEATER_COUNT]   = HEAT_SHORT_ID;
const char *const heatCmd[MAX_HEATER_COUNT]       = HEAT_CMD;
const char *const heatWaitCmd[MAX_HEATER_COUNT]   = HEAT_WAIT_CMD;

static HEATER  heater = {{}, NOZZLE0};
static uint8_t heat_update_seconds = TEMPERATURE_QUERY_SLOW_SECONDS;
static bool    heat_update_waiting = false;
static uint8_t heat_send_waiting = 0;
static uint8_t heat_feedback_waiting = 0;

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
    return NOZZLE0;

  return INVALID_HEATER;  // Invalid heater
}

// Set target temperature
void heatSetTargetTemp(uint8_t index, int16_t temp, TEMP_SOURCE tempSource)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return;

  switch (tempSource)
  {
    case FROM_HOST:
      if (GET_BIT(heat_feedback_waiting, index))
        SET_BIT_OFF(heat_feedback_waiting, index);
      else if (!GET_BIT(heat_send_waiting, index))
        heater.T[index].target = temp;
      break;

    case FROM_GUI:
      heater.T[index].target = NOBEYOND(0, temp, infoSettings.max_temp[index]);
      SET_BIT_ON(heat_send_waiting, index);

      if (inRange(heater.T[index].current, heater.T[index].target, TEMPERATURE_RANGE))
        heater.T[index].status = SETTLED;
      else
        heater.T[index].status = heater.T[index].target > heater.T[index].current ? HEATING : COOLING;
      break;

    case FROM_CMD:
      if (GET_BIT(heat_feedback_waiting, index) == false)
      {
        heater.T[index].target = temp;
        SET_BIT_ON(heat_feedback_waiting, index);
      }
      break;
  }
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

// Disable all heaters/hotends
void heatCoolDown(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heatSetTargetTemp(i, 0, FROM_GUI);
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
    heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
  else if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
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
  if (heat_update_seconds == seconds)
    return;

  heat_update_seconds = seconds;

  if (infoMachineSettings.autoReportTemp && !heat_update_waiting)
    heat_update_waiting = storeCmd("M155 S%u\n", heatGetUpdateSeconds());
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

void updateNextHeatCheckTime(void)
{
  nextHeatCheckTime = OS_GetTimeMs() + SEC_TO_MS(heat_update_seconds);
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

      if (requestCommandInfoIsRunning())  // To avoid colision in gcode response processing
        break;

      if ((infoMachineSettings.firmwareType != FW_REPRAPFW) && !storeCmd("M105\n"))
        break;

      updateNextHeatCheckTime();
      heat_update_waiting = true;
    } while (0);
  }
  else  // check temperature auto-report timout and resend M155 command
  {
    if (OS_GetTimeMs() > AUTOREPORT_TIMEOUT && !heat_update_waiting)
    {
      heat_update_waiting = storeCmd("M155 S%u\n", heatGetUpdateSeconds());
      if (heat_update_waiting)
        updateNextHeatCheckTime();  // set next timeout for temperature auto-report
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

    if (MENU_IS(menuHeat))
      break;

    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }

  // Query heaters if they reached the target temperature (only if not printing)
  for (uint8_t i = 0; (i < MAX_HEATER_COUNT) && (!isPrinting()); i++)
  {
    if (heater.T[i].status != SETTLED && inRange(heater.T[i].current, heater.T[i].target, TEMPERATURE_RANGE))
    {
      switch (heater.T[i].status)
      {
        case HEATING:
          BUZZER_PLAY(SOUND_HEATED);
          break;

        case COOLING:
          BUZZER_PLAY(SOUND_COOLED);
          break;

        default:
          break;
      }

      heater.T[i].status = SETTLED;
    }
  }

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)  // If the target temperature changes, send a gcode to set the motherboard
  {
    if (GET_BIT(heat_send_waiting, i) && !GET_BIT(heat_feedback_waiting, i))
    {
      if (storeCmd("%s S%u\n", heatCmd[i], heatGetTargetTemp(i)))
      {
        SET_BIT_OFF(heat_send_waiting, i);
        SET_BIT_ON(heat_feedback_waiting, i);
      }
    }
  }
}
