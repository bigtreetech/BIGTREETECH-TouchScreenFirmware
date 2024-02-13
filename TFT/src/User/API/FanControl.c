#include "FanControl.h"
#include "includes.h"

#define FAN_REFRESH_TIME 500  // 1 second is 1000

const char * fanID[MAX_FAN_COUNT] = FAN_DISPLAY_ID;
const char * fanCmd[MAX_FAN_COUNT] = FAN_CMD;

static uint8_t setFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t curFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t needSetFanSpeed = 0;

static bool ctrlFanSendingWaiting = false;

void fanResetSpeed(void)
{
  needSetFanSpeed = 0;
  memset(setFanSpeed, 0, sizeof(setFanSpeed));
  memset(curFanSpeed, 0, sizeof(curFanSpeed));
}

bool fanIsValid(const uint8_t index)
{
  if (index >= infoSettings.fan_count && index < MAX_COOLING_FAN_COUNT)  // invalid cooling fan index
    return false;
  else if (!infoSettings.ctrl_fan_en && index >= MAX_COOLING_FAN_COUNT)  // controller cooling fan is disabled
    return false;
  else if (index >= (MAX_COOLING_FAN_COUNT + MAX_CTRL_FAN_COUNT))  // invalid controller cooling fan index (not active/idle)
    return false;
  else
    return true;
}

void fanSetSpeed(const uint8_t i, const uint8_t speed)
{
  SET_BIT_VALUE(needSetFanSpeed, i, fanGetCurSpeed(i) != speed);
  setFanSpeed[i] = speed;
}

uint8_t fanGetSetSpeed(const uint8_t i)
{
  return setFanSpeed[i];
}

void fanSetPercent(const uint8_t i, const uint8_t percent)
{
  fanSetSpeed(i, (NOBEYOND(0, percent, 100) * infoSettings.fan_max[i]) / 100);
}

uint8_t fanGetSetPercent(const uint8_t i)
{
  return (setFanSpeed[i] * 100.0f) / infoSettings.fan_max[i] + 0.5f;
}

void fanSetCurSpeed(const uint8_t i, const uint8_t speed)
{
  curFanSpeed[i] = speed;
}

uint8_t fanGetCurSpeed(const uint8_t i)
{
  return curFanSpeed[i];
}

void fanSetCurPercent(const uint8_t i, const uint8_t percent)
{
  curFanSpeed[i] = (NOBEYOND(0, percent, 100) * infoSettings.fan_max[i]) / 100;
}

uint8_t fanGetCurPercent(const uint8_t i)
{
  return (curFanSpeed[i] * 100.0f) / infoSettings.fan_max[i] + 0.5f;
}

void loopCheckFan(void)
{
  static uint32_t nextUpdateTime = 0;

  if (OS_GetTimeMs() < nextUpdateTime)  // avoid rapid fire, clogging the queue
    return;

  nextUpdateTime = OS_GetTimeMs() + FAN_REFRESH_TIME;  // extend next check time

  for (uint8_t i = 0; i < MAX_FAN_COUNT; i++)
  {
    if (GET_BIT(needSetFanSpeed, i))
    {
      if (storeCmd(fanCmd[i], setFanSpeed[i]))
        SET_BIT_OFF(needSetFanSpeed, i);
    }
  }
}

void ctrlFanQueryClearSendingWaiting(void)
{
  ctrlFanSendingWaiting = false;
}

void ctrlFanQuery(void)
{ // following conditions ordered by importance
  if (infoSettings.ctrl_fan_en && !ctrlFanSendingWaiting && infoHost.tx_slots != 0 && infoHost.connected)
    ctrlFanSendingWaiting = storeCmd("M710\n");
}
