#include "FanControl.h"

char*   fanID[MAX_FAN_COUNT];
char*   fanCmd[MAX_FAN_COUNT];
uint8_t fanType[MAX_FAN_COUNT];

static uint8_t desiredFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t newFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t curFanSpeed[MAX_FAN_COUNT] = {0};

static bool fan_send_waiting[MAX_FAN_COUNT] = {false};
static bool fanQueryWait = false;
static bool fanQueryEnable = false;

static uint32_t nextFanTime = 0;

#define NEXT_FAN_WAIT 500  // 1 second is 1000

uint8_t fanGetTypID(uint8_t startIndex, uint8_t type)
{
  for (uint8_t i = startIndex; i < MAX_FAN_COUNT; i++)
  {
    if (fanType[i] == type)
      return i;
  }
  return FAN_TYPE_UNKNOWN;
}

void fanBuildList(void)
{
  char* fanIDTemp[MAX_FAN_COUNT] = FAN_DISPLAY_ID;
  char* fanCmdTemp[MAX_FAN_COUNT] = FAN_CMD;
  uint8_t fanTypeTemp[MAX_FAN_COUNT] = FAN_TYPE;

  for (uint8_t i = 0, j = 0; i < MAX_FAN_COUNT; i++, j++)
  {
    if (infoSettings.fan_count == i)
    {
      if (infoSettings.fan_ctrl_count > 0)
        i = (MAX_FAN_COUNT - MAX_FAN_CTRL_COUNT);
      else
        i= MAX_FAN_COUNT;
    }
    if (i < MAX_FAN_COUNT)
    {
      memcpy(fanID+j, fanIDTemp+i, sizeof(fanIDTemp[i]));
      memcpy(fanCmd+j, fanCmdTemp+i, sizeof(fanCmdTemp[i]));
      memcpy(fanType+j, fanTypeTemp+i, sizeof(fanTypeTemp[i]));
    }
  }
}

void fanControlInit(void)
{
  fanBuildList();

  fanQueryEnable =
    (infoSettings.fan_ctrl_count > 0 &&
     (fanGetTypID(infoSettings.fan_count -1, FAN_TYPE_CTRL_I) ||
      fanGetTypID(infoSettings.fan_count -1, FAN_TYPE_CTRL_S))
    );
}

bool fanIsType(uint8_t i, uint8_t type)
{
  return (fanType[i] == type);
}

void fanSetRcvSpeed(uint8_t i, uint8_t speed)
{
  curFanSpeed[i] = newFanSpeed[i] = desiredFanSpeed[i] = speed;  // avoid resend received values
}

void fanSetDesiredSpeed(uint8_t i, uint8_t speed)
{
  desiredFanSpeed[i] = speed;
}

uint8_t fanGetDesiredSpeed(uint8_t i)
{
  return desiredFanSpeed[i];
}

void fanSetDesiredPercent(uint8_t i, uint8_t percent)
{
  percent = NOBEYOND(0, percent, 100);
  desiredFanSpeed[i] = (percent * infoSettings.fan_max[i]) / 100;
}

uint8_t fanGetDesiredPercent(uint8_t i)
{
  return (desiredFanSpeed[i] * 100.0f) / infoSettings.fan_max[i] + 0.5f;
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

void fanSetNewSpeed(uint8_t i, uint8_t speed)
{
  curFanSpeed[i] = speed;
}

uint8_t fanGetNewSpeed(uint8_t i)
{
  return curFanSpeed[i];
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
  for (uint8_t i = 0; i < (infoSettings.fan_count + infoSettings.fan_ctrl_count); i++)
  {
    if ((newFanSpeed[i] != desiredFanSpeed[i]) && (OS_GetTimeMs() > nextFanTime))
    {
      if(fan_send_waiting[i] == false)
      {
        if(fanIsType(i,FAN_TYPE_F) || fanIsType(i,FAN_TYPE_CTRL_S))
          fan_send_waiting[i] = storeCmd("%s S%d\n", fanCmd[i], desiredFanSpeed[i]);
        else if (fanIsType(i,FAN_TYPE_CTRL_I))
          fan_send_waiting[i] = storeCmd("%s I%d\n", fanCmd[i], desiredFanSpeed[i]);
      }
      if (fan_send_waiting[i] == true)
        newFanSpeed[i] = desiredFanSpeed[i];
      nextFanTime = OS_GetTimeMs() + NEXT_FAN_WAIT; // avoid rapid fire, clogging the queue
    }
  }
}

void fanSpeedQuery(void)
{
  if (infoHost.connected && !infoHost.wait && !fanQueryWait && fanQueryEnable)
  {
    storeCmd("M710\n");
    fanQueryWait = true;
  }
}
