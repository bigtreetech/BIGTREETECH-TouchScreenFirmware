#include "SpeedControl.h"
#include "includes.h"

#define SPEED_REFRESH_TIME 500  // 1 second is 1000

static const char * const speedCmd[SPEED_NUM] = {"M220", "M221"};

static uint16_t targetPercent[SPEED_NUM]  = {100, 100};
static uint16_t currentPercent[SPEED_NUM] = {100, 100};
static uint8_t targetPercentNeeded        = 0;

static bool speedSendingWaiting = false;

void speedSetTargetPercent(const uint8_t tool, const uint16_t per)
{
  uint16_t value = NOBEYOND(SPEED_MIN, per, SPEED_MAX);

  SET_BIT_VALUE(targetPercentNeeded, tool, value != currentPercent[tool]);
  targetPercent[tool] = value;
}

uint16_t speedGetTargetPercent(const uint8_t tool)
{
  return targetPercent[tool];
}

void speedSetCurrentPercent(const uint8_t tool, const uint16_t per)
{
  currentPercent[tool] = per;
}

uint16_t speedGetCurrentPercent(const uint8_t tool)
{
  return currentPercent[tool];
}

void loopCheckSpeed(void)
{
  static uint32_t nextUpdateTime = 0;

  if (OS_GetTimeMs() < nextUpdateTime)  // avoid rapid fire, clogging the queue
    return;

  nextUpdateTime = OS_GetTimeMs() + SPEED_REFRESH_TIME;  // extend next check time

  for (uint8_t i = 0; i < SPEED_NUM; i++)
  {
    if (infoSettings.ext_count == 0 && i > 0)  // don't poll M221 if there are no extruders
      continue;

    if (GET_BIT(targetPercentNeeded, i))
    {
      if (storeCmd("%s S%d D%d\n", speedCmd[i], targetPercent[i], heatGetToolIndex()))
        SET_BIT_OFF(targetPercentNeeded, i);
    }
  }
}

void speedQueryClearSendingWaiting(void)
{
  speedSendingWaiting = false;
}

void speedQuery(void)
{ // following conditions ordered by importance
  if (!speedSendingWaiting && infoHost.tx_slots != 0 && infoHost.connected && infoMachineSettings.firmwareType != FW_REPRAPFW)
  {
    speedSendingWaiting = storeCmd("M220\n");

    if (infoSettings.ext_count > 0)
      speedSendingWaiting |= storeCmd("M221\n");  // speedSendingWaiting set to "true" if at least one command will be sent
  }
}
