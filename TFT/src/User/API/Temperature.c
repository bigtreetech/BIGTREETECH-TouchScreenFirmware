#include "Temperature.h"
#include "includes.h"

#define HEATING_TIMEOUT    300000  // 5 minutes (1 sec is 1000)
#define AUTOREPORT_TIMEOUT 3000    // 3 second grace period

const char * const heaterID[MAX_HEATER_COUNT]      = HEAT_SIGN_ID;
const char * const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char * const heatShortID[MAX_HEATER_COUNT]   = HEAT_SHORT_ID;
const char * const heatCmd[MAX_HEATER_COUNT]       = HEAT_CMD;
const char * const heatWaitCmd[MAX_HEATER_COUNT]   = HEAT_WAIT_CMD;
const char * const extruderDisplayID[]             = EXTRUDER_ID;
const char * const toolChange[]                    = TOOL_CHANGE;

static HEATER heater = {{}, NOZZLE0};
static bool heat_waiting = false;            // "false" if no heater waiting for target temperature. "true" otherwise
static uint32_t heat_timestamp = 0;          // keep track of timestamp of last heating command sent to mainboard
static uint8_t heat_feedback_waiting = 0;    // "true" if command requested from GUI has been enqueued or a command has been sent to mainboard
static uint8_t heat_enqueueing_waiting = 0;  // "true" if command requested from GUI and waiting for enqueueing in command queue

static uint8_t heat_update_seconds = TEMPERATURE_QUERY_SLOW_SECONDS;
static uint32_t heat_next_update_time = 0;
static bool heat_sending_waiting = false;

// verify that the heater index is valid, and fix the index of multiple in and 1 out tool nozzles
static uint8_t heaterIndexFix(uint8_t index)
{
  if (index == BED && infoSettings.bed_en)  // bed
    return index;

  if (index == CHAMBER && infoSettings.chamber_en)  // chamber
    return index;

  if (index < infoSettings.hotend_count)  // vaild tool nozzle
    return index;

  if (index < infoSettings.ext_count && infoSettings.hotend_count == 1)  // "multi-extruder" that shares a single nozzle
    return NOZZLE0;

  return INVALID_HEATER;  // invalid heater
}

void heatSetTargetTemp(uint8_t index, const int16_t temp, const TEMP_SOURCE tempSource)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return;

  switch (tempSource)
  {
    // temperature status (actual/requested) from host (Marlin, RepRap, etc.)
    case FROM_HOST:
      // set target temperature if not waiting for feedback and no pending command request from GUI
      // (it avoids to set old target temperature in case of multiple commands issued from GUI)
      //
      if (GET_BIT(heat_feedback_waiting, index))          // if waiting for feedback, clear flag
        SET_BIT_OFF(heat_feedback_waiting, index);
      else if (!GET_BIT(heat_enqueueing_waiting, index))  // if no pending command request from GUI, set target temperature
        heater.T[index].target = temp;
      break;

    // temperature requested from TFT's GUI
    case FROM_GUI:
      heater.T[index].target = NOBEYOND(0, temp, infoSettings.max_temp[index]);

      if (inRange(heater.T[index].current, heater.T[index].target, TEMPERATURE_RANGE))
        heater.T[index].status = SETTLED;
      else
        heater.T[index].status = heater.T[index].target > heater.T[index].current ? HEATING : COOLING;

      SET_BIT_ON(heat_enqueueing_waiting, index);
      break;

    // temperature requested in command queue (from gcode file, TFT's GUI or external source connected to TFT) and ready to be sent to mainboard
    case FROM_CMD:
      heat_timestamp = OS_GetTimeMs();  // update timestamp

      if (!GET_BIT(heat_feedback_waiting, index))
      {
        heater.T[index].target = temp;

        SET_BIT_ON(heat_feedback_waiting, index);
      }
      break;
  }
}

int16_t heatGetTargetTemp(uint8_t index)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return 0;

  return heater.T[index].target;
}

void heatSetCurrentTemp(uint8_t index, const int16_t temp)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return;

  heater.T[index].current = NOBEYOND(-99, temp, 999);
}

int16_t heatGetCurrentTemp(uint8_t index)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return 0;

  return heater.T[index].current;
}

void heatCoolDown(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heatSetTargetTemp(i, 0, FROM_GUI);
  }
}

void heatSetWaiting(uint8_t index, const bool isWaiting)
{
  index = heaterIndexFix(index);

  if (index == INVALID_HEATER)
    return;

  heater.T[index].waiting = isWaiting;

  if (isWaiting == true)  // wait heating now, query more frequently
  {
    heat_waiting = true;  // always set to "true" when waiting for heating

    heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
  }
  else if (heatIsWaiting() == false)
  {
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }
}

bool heatIsWaiting(void)
{
  if (!heat_waiting)  // if no heater waiting for target temperature
    return false;

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting == true)
      return true;
  }

  heat_waiting = false;  // set to "false" when no more heaters waiting for target temperature

  return false;
}

bool heatIsWaitingTimedout(void)
{
  if (!heatIsWaiting() || OS_GetTimeMs() - heat_timestamp < HEATING_TIMEOUT)  // if no heater waiting for target temperature or no timeout
    return false;

  heat_timestamp = OS_GetTimeMs();  // update timestamp

  return true;
}

void heatClearWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heater.T[i].waiting = false;
  }

  heat_waiting = false;  // set to "false" when no more heaters waiting for target temperature

  heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

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

uint8_t heatGetToolIndex(void)
{
  return heater.toolIndex;
}

uint8_t heatGetCurrentHotend(void)
{
  return (infoSettings.hotend_count == 1) ? NOZZLE0 : heater.toolIndex;
}

bool heaterDisplayIsValid(const uint8_t index)
{
  if (index >= infoSettings.hotend_count && index < MAX_HOTEND_COUNT) return false;
  if (!infoSettings.bed_en && index == BED) return false;
  if (!infoSettings.chamber_en && index == CHAMBER) return false;

  return true;
}

void heatSetUpdateSeconds(const uint8_t seconds)
{
  if (heat_update_seconds == seconds)
    return;

  heat_update_seconds = seconds;

  if (infoMachineSettings.autoReportTemp && !heat_sending_waiting)
    heat_sending_waiting = storeCmd("M155 S%u\n", heat_update_seconds);
}

uint8_t heatGetUpdateSeconds(void)
{
  return heat_update_seconds;
}

void heatSyncUpdateSeconds(const uint8_t seconds)
{
  heat_update_seconds = seconds;
}

void heatSetNextUpdateTime(void)
{
  heat_next_update_time = OS_GetTimeMs() + SEC_TO_MS(heat_update_seconds);

  if (infoMachineSettings.autoReportTemp)
    heat_next_update_time += AUTOREPORT_TIMEOUT;
}

void heatClearSendingWaiting(void)
{
  heat_sending_waiting = false;
}

void loopCheckHeater(void)
{
  do
  { // periodically send M105 to query the temperatures, if motherboard does not supports M155 (AUTO_REPORT_TEMPERATURES)
    // feature to automatically report the temperatures or (if M155 is supported) check temperature auto-report timeout
    // and resend M155 command in case of timeout expired

    if (OS_GetTimeMs() < heat_next_update_time)  // if next check time not yet elapsed, do nothing
      break;

    heatSetNextUpdateTime();  // extend next check time

    // if M105/M155 previously enqueued and not yet sent or pending command
    // (to avoid collision in gcode response processing), do nothing
    if (heat_sending_waiting || requestCommandInfoIsRunning())
      break;

    heat_sending_waiting = !infoMachineSettings.autoReportTemp ? storeCmd("M105\n") : storeCmd("M155 S%u\n", heat_update_seconds);
  } while (0);

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

    // send a pending command requested by GUI only if there is no pending feedback
    if (GET_BIT(heat_enqueueing_waiting, i) && !GET_BIT(heat_feedback_waiting, i))
    {
      if (storeCmd("%s S%u\n", heatCmd[i], heatGetTargetTemp(i)))
      {
        SET_BIT_OFF(heat_enqueueing_waiting, i);
        SET_BIT_ON(heat_feedback_waiting, i);
      }
    }
  }

  if (MENU_IS_NOT(menuHeat) && !heatIsWaiting())
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
