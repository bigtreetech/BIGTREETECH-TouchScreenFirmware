#include "Temperature.h"
#include "includes.h"

const char * const heaterID[MAX_HEATER_COUNT]      = HEAT_SIGN_ID;
const char * const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char * const heatShortID[MAX_HEATER_COUNT]   = HEAT_SHORT_ID;
const char * const heatCmd[MAX_HEATER_COUNT]       = HEAT_CMD;
const char * const heatWaitCmd[MAX_HEATER_COUNT]   = HEAT_WAIT_CMD;
const char * const extruderDisplayID[]             = EXTRUDER_ID;
const char * const toolChange[]                    = TOOL_CHANGE;

static HEATER  heater = {{}, NOZZLE0};
static uint8_t heat_update_seconds = TEMPERATURE_QUERY_SLOW_SECONDS;
static bool    heat_update_waiting = false;
static uint8_t heat_send_waiting = 0;
static uint8_t heat_feedback_waiting = 0;

uint32_t nextHeatCheckTime = 0;

#define AUTOREPORT_TIMEOUT (nextHeatCheckTime + 3000)  // update interval + 3 second grace period

// verify that the heater index is valid, and fix the index of multiple in and 1 out tool nozzles
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

// set target temperature
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

// get target temperature
uint16_t heatGetTargetTemp(uint8_t index)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return 0;

  return heater.T[index].target;
}

// set current temperature
void heatSetCurrentTemp(uint8_t index, int16_t temp)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return;

  heater.T[index].current = NOBEYOND(-99, temp, 999);

  if (infoMachineSettings.autoReportTemp)
    updateNextHeatCheckTime();  // set next timeout for temperature auto-report
}

// get current temperature
int16_t heatGetCurrentTemp(uint8_t index)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return 0;

  return heater.T[index].current;
}

// disable all heaters/hotends
void heatCoolDown(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heatSetTargetTemp(i, 0, FROM_GUI);
  }
}

// is heating waiting to heat up
bool heatGetIsWaiting(uint8_t index)
{
  return (heater.T[index].waiting == true);
}

// check all heater if there is a heater waiting to be waited
bool heatHasWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting == true)
      return true;
  }

  return false;
}

// set heater waiting status
void heatSetIsWaiting(uint8_t index, bool isWaiting)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return;

  heater.T[index].waiting = isWaiting;

  if (isWaiting == true)  // wait heating now, query more frequently
    heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
  else if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

void heatClearIsWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heater.T[i].waiting = false;
  }

  heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

// set current tool (extruder)
// used when tool change command is from TFT
bool heatSetTool(const uint8_t toolIndex)
{
  if (storeCmd("%s\n", toolChange[toolIndex]))
  {
    heater.toolIndex = toolIndex;
    return true;
  }

  return false;
}

void heatSetToolIndex(const uint8_t toolIndex)
{
  heater.toolIndex = toolIndex;
}

// get current Tool (extruder)
uint8_t heatGetToolIndex(void)
{
  return heater.toolIndex;
}

// get current hotend index in arry T[]
uint8_t heatGetCurrentHotend(void)
{
  return (infoSettings.hotend_count == 1) ? NOZZLE0 : heater.toolIndex;
}

// check whether the index is a valid heater index.
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

// set temperature update time interval
void heatSetUpdateSeconds(uint8_t seconds)
{
  if (heat_update_seconds == seconds)
    return;

  heat_update_seconds = seconds;

  if (infoMachineSettings.autoReportTemp && !heat_update_waiting)
    heat_update_waiting = storeCmd("M155 S%u\n", heatGetUpdateSeconds());
}

// get query temperature seconds
uint8_t heatGetUpdateSeconds(void)
{
  return heat_update_seconds;
}

// set query temperature seconds
void heatSyncUpdateSeconds(uint8_t seconds)
{
  heat_update_seconds = seconds;
}

// set whether we need to query the current temperature
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

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting && inRange(heater.T[i].current, heater.T[i].target, TEMPERATURE_RANGE))
      heater.T[i].waiting = false;

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

    if (GET_BIT(heat_send_waiting, i) && !GET_BIT(heat_feedback_waiting, i))
    {
      if (storeCmd("%s S%u\n", heatCmd[i], heatGetTargetTemp(i)))
      {
        SET_BIT_OFF(heat_send_waiting, i);
        SET_BIT_ON(heat_feedback_waiting, i);
      }
    }
  }

  if (MENU_IS_NOT(menuHeat) && !heatHasWaiting())
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
