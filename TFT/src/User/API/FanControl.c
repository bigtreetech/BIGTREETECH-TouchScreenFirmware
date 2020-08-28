#include "FanControl.h"
#include "includes.h"

const char* fanID[MAX_FAN_COUNT] = FAN_DISPLAY_ID;
const char* fanCmd[MAX_FAN_COUNT] = FAN_CMD;
const uint8_t fanType[MAX_FAN_COUNT] = FAN_TYPE;

static uint8_t fanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t lastFanSpeed[MAX_FAN_COUNT] = {0};

static bool fan_send_waiting[MAX_FAN_COUNT] = {false};
static bool fanQueryWait = false;

uint8_t fanGetTypID(uint8_t startIndex, uint8_t type) {
  for(uint8_t i = startIndex; i < MAX_FAN_COUNT; i++) {
    if(fanType[i] == type) return i;
  }
  return FAN_TYPE_UNKNOWN;
}

bool fanIsType(uint8_t i, uint8_t type) {
  return (fanType[i] == type);
}

void fanSetSpeed(uint8_t i, uint8_t speed)
{
  fanSpeed[i] = speed;
}

uint8_t fanGetSpeed(uint8_t i)
{
  return fanSpeed[i];
}

void fanSetSpeedPercent(uint8_t i, uint8_t percent)
{
  percent = NOBEYOND(0, percent, 100);
  fanSpeed[i] = (percent * infoSettings.fan_max[i]) / 100;
}

uint8_t fanGetSpeedPercent(uint8_t i)
{
  return (fanSpeed[i] * 100.0f) / infoSettings.fan_max[i] + 0.5f;
}

void fanSetSendWaiting(uint8_t i, bool isWaiting)
{
  fan_send_waiting[i] = isWaiting;
}

void fanSpeedQuerySetWait(bool wait)
{
  fanQueryWait = wait;
}

void loopFan(void)
{
  for (uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (lastFanSpeed[i] != fanSpeed[i])
    {
      lastFanSpeed[i] = fanSpeed[i];
      if(fan_send_waiting[i] == false)
      {
        fan_send_waiting[i] = true;
        if(fanIsType(i,FAN_TYPE_F) || fanIsType(i,FAN_TYPE_CTRL_S)) {
          storeCmd("%s S%d\n", fanCmd[i],fanSpeed[i]);
        } 
        else if (fanIsType(i,FAN_TYPE_CTRL_I)) {
          storeCmd("%s I%d\n", fanCmd[i],fanSpeed[i]);
        }
      }
    }
  }
}

void fanSpeedQuery(void)
{
  if ( infoHost.connected == true && infoHost.wait == false )
  { 
    if (!fanQueryWait) 
    {
      storeCmd("M710\n");
      fanQueryWait = true;
    }
  }
}
