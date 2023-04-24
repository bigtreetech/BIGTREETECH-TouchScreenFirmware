#include "FanControl.h"
#include "includes.h"

#define NEXT_FAN_WAIT 500  // 1 second is 1000

const char* fanID[MAX_FAN_COUNT] = FAN_DISPLAY_ID;
const char* fanCmd[MAX_FAN_COUNT] = FAN_CMD;

static uint8_t setFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t curFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t needSetFanSpeed = 0;

static bool ctrlFanQueryWait = false;
static uint32_t nextCtrlFanTime = 0;

void fanResetSpeed(void)
{
  needSetFanSpeed = 0;
  memset(setFanSpeed, 0, sizeof(setFanSpeed));
  memset(curFanSpeed, 0, sizeof(curFanSpeed));
}

// Check whether the index is a valid fan index.
bool fanIsValid(const uint8_t index)
{
  if (index >= infoSettings.fan_count && index < MAX_COOLING_FAN_COUNT)  // invalid cooling fan index
    return false;
  else if (!infoSettings.ctrl_fan_en && index >= MAX_COOLING_FAN_COUNT)  // controller cooling fan is disabled
    return false;
  else if (index >= (MAX_COOLING_FAN_COUNT + MAX_CRTL_FAN_COUNT))  // invalid controller cooling fan index (not active/idle)
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

void loopFan(void)
{
  for (uint8_t i = 0; i < MAX_FAN_COUNT; i++)
  {
    if (GET_BIT(needSetFanSpeed, i) && (OS_GetTimeMs() > nextCtrlFanTime))
    {
      if (storeCmd(fanCmd[i], setFanSpeed[i]))
      {
        SET_BIT_OFF(needSetFanSpeed, i);
      }

      nextCtrlFanTime = OS_GetTimeMs() + NEXT_FAN_WAIT;  // avoid rapid fire, clogging the queue
    }
  }
}

void ctrlFanQuerySetWait(const bool wait)
{
  ctrlFanQueryWait = wait;
}

// query for controller fan only
void ctrlFanQuery(void)
{
  if (infoHost.connected && !infoHost.wait && !ctrlFanQueryWait && infoSettings.ctrl_fan_en)
  {
    ctrlFanQueryWait = storeCmd("M710\n");
  }
}
