#include "FanControl.h"
#include "includes.h"

#define NEXT_FAN_WAIT 500  // 1 second is 1000

const char* fanID[MAX_FAN_COUNT] = FAN_DISPLAY_ID;
const char* fanCmd[MAX_FAN_COUNT] = FAN_CMD;

static uint8_t setFanSpeed[MAX_FAN_COUNT] = {0};
static bool needSetFanSpeed[MAX_FAN_COUNT] = {false};
static uint8_t curFanSpeed[MAX_FAN_COUNT] = {0};

static bool ctrlFanQueryWait = false;
static uint32_t nextCtrlFanTime = 0;

// Check whether the index is a valid fan index.
bool fanIsValid(uint8_t index)
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

void fanSetSpeed(uint8_t i, uint8_t speed)
{
  needSetFanSpeed[i] = fanGetCurSpeed(i) != speed;
  setFanSpeed[i] = speed;
}

uint8_t fanGetSetSpeed(uint8_t i)
{
  return setFanSpeed[i];
}

void fanSetPercent(uint8_t i, uint8_t percent)
{
  percent = NOBEYOND(0, percent, 100);
  fanSetSpeed(i, (percent * infoSettings.fan_max[i]) / 100);
}

uint8_t fanGetSetPercent(uint8_t i)
{
  return (setFanSpeed[i] * 100.0f) / infoSettings.fan_max[i] + 0.5f;
}

void fanSetCurSpeed(uint8_t i, uint8_t speed)
{
  curFanSpeed[i] = speed;
}

uint8_t fanGetCurSpeed(uint8_t i)
{
  return curFanSpeed[i];
}

void fanSetCurPercent(uint8_t i, uint8_t percent)
{
  percent = NOBEYOND(0, percent, 100);
  curFanSpeed[i] = (percent * infoSettings.fan_max[i]) / 100;
}

uint8_t fanGetCurPercent(uint8_t i)
{
  return (curFanSpeed[i] * 100.0f) / infoSettings.fan_max[i] + 0.5f;
}

void loopFan(void)
{
  for (uint8_t i = 0; i < MAX_FAN_COUNT; i++)
  {
    if (needSetFanSpeed[i] && (OS_GetTimeMs() > nextCtrlFanTime))
    {
      if (storeCmd(fanCmd[i], setFanSpeed[i]))
      {
        needSetFanSpeed[i] = false;
      }

      nextCtrlFanTime = OS_GetTimeMs() + NEXT_FAN_WAIT;  // avoid rapid fire, clogging the queue
    }
  }
}

void ctrlFanQuerySetWait(bool wait)
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
