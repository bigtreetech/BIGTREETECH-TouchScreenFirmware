#include "FanControl.h"

char*   fanID[MAX_FAN_COUNT];
char*   fanCmd[MAX_FAN_COUNT];
uint8_t fanType[MAX_FAN_COUNT];

static uint8_t setFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t lastSetFanSpeed[MAX_FAN_COUNT] = {0};
static uint8_t curFanSpeed[MAX_FAN_COUNT] = {0};

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
        i = MAX_FAN_COUNT;
    }
    if (i < MAX_FAN_COUNT)
    {
      memcpy(fanID + j, fanIDTemp + i, sizeof(fanIDTemp[i]));
      memcpy(fanCmd + j, fanCmdTemp + i, sizeof(fanCmdTemp[i]));
      memcpy(fanType + j, fanTypeTemp + i, sizeof(fanTypeTemp[i]));
    }
  }
}

void fanControlInit(void)
{
  fanBuildList();

  fanQueryEnable = (infoSettings.fan_ctrl_count > 0 &&
                    (fanGetTypID(infoSettings.fan_count -1, FAN_TYPE_CTRL_I) ||
                     fanGetTypID(infoSettings.fan_count -1, FAN_TYPE_CTRL_S)));
}

bool fanIsType(uint8_t i, uint8_t type)
{
  return (fanType[i] == type);
}

void fanSetSpeed(uint8_t i, uint8_t speed)
{
  setFanSpeed[i] = speed;
}

uint8_t fanGetSetSpeed(uint8_t i)
{
  return setFanSpeed[i];
}

void fanSetPercent(uint8_t i, uint8_t percent)
{
  percent = NOBEYOND(0, percent, 100);
  setFanSpeed[i] = (percent * infoSettings.fan_max[i]) / 100;
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
  for (uint8_t i = 0; i < (infoSettings.fan_count + infoSettings.fan_ctrl_count); i++)
  {
    if ((lastSetFanSpeed[i] != setFanSpeed[i]) && (OS_GetTimeMs() > nextFanTime))
    {
      if(fanIsType(i,FAN_TYPE_F) || fanIsType(i,FAN_TYPE_CTRL_S))
      {
        if (storeCmd("%s S%d\n", fanCmd[i], setFanSpeed[i]))
          lastSetFanSpeed[i] = setFanSpeed[i];
      }
      else if (fanIsType(i,FAN_TYPE_CTRL_I))
      {
        if (storeCmd("%s I%d\n", fanCmd[i], setFanSpeed[i]))
          lastSetFanSpeed[i] = setFanSpeed[i];
      }
      nextFanTime = OS_GetTimeMs() + NEXT_FAN_WAIT; // avoid rapid fire, clogging the queue
    }
  }
}

void fanQuerySetWait(bool wait)
{
  fanQueryWait = wait;
}

void fanSpeedQuery(void)
{
  if (infoHost.connected && !infoHost.wait && !fanQueryWait && fanQueryEnable)
  {
    fanQueryWait = storeCmd("M710\n");
  }
}
