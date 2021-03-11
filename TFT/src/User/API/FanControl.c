#include "FanControl.h"

char* fanID[MAX_FAN_COUNT];
char* fanCmd[MAX_FAN_COUNT];
uint8_t fanType[MAX_FAN_COUNT];

static FAN     fan[MAX_FAN_COUNT];
static uint8_t lastSetFanSpeed[MAX_FAN_COUNT];
static bool    fan_send_waiting[MAX_HEATER_COUNT];

static uint8_t act_fan_count = MAX_FAN_COUNT;

static bool fanQueryWait = false;

uint8_t fanGetTypID(uint8_t startIndex, uint8_t type)
{
  for (uint8_t i = startIndex; i < act_fan_count; i++)
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
      fanID[j] = fanIDTemp[i];
      fanCmd[j] = fanCmdTemp[i];
      fanType[j] = fanTypeTemp[i];
    }
  }

  act_fan_count = infoSettings.fan_count + infoSettings.fan_ctrl_count;
  memset(fan, 0, sizeof(fan));
  memset(lastSetFanSpeed, 0, sizeof(lastSetFanSpeed));
  memset(fan_send_waiting, 0, sizeof(fan_send_waiting));
}

void fanControlInit(void)
{
  fanBuildList();
}

bool fanIsType(uint8_t i, uint8_t type)
{
  return (i < act_fan_count) ? (fanType[i] == type) : false;
}

void fanSetSpeed(uint8_t i, uint8_t speed)
{
  if (i < act_fan_count)
  {
    fan[i].setFanSpeed = speed;
  }
}

uint8_t fanGetSetSpeed(uint8_t i)
{
  return (i < act_fan_count) ? fan[i].setFanSpeed : 0;
}

void fanSetPercent(uint8_t i, uint8_t percent)
{
  if (i < act_fan_count)
  {
    percent = NOBEYOND(0, percent, 100);
    fan[i].setFanSpeed = (percent * infoSettings.fan_max[i]) / 100;
  }
}

uint8_t fanGetSetPercent(uint8_t i)
{
  return (i < act_fan_count) ? ((fan[i].setFanSpeed * 100.0f) / infoSettings.fan_max[i] + 0.5f) : 0;
}

void fanSetCurSpeed(uint8_t i, uint8_t speed)
{
  if (i < act_fan_count)
  {
    fan[i].curFanSpeed = speed;
  }
}

uint8_t fanGetCurSpeed(uint8_t i)
{
  return (i < act_fan_count) ? fan[i].curFanSpeed : 0;
}

void fanSetCurPercent(uint8_t i, uint8_t percent)
{
  if (i < act_fan_count)
  {
    percent = NOBEYOND(0, percent, 100);
    fan[i].curFanSpeed = (percent * infoSettings.fan_max[i]) / 100;
  }
}

uint8_t fanGetCurPercent(uint8_t i)
{
  return (i < act_fan_count) ? ((fan[i].curFanSpeed * 100.0f) / infoSettings.fan_max[i] + 0.5f) : 0;
}

//Sync fan speed
void fanSyncSpeed(uint8_t i, uint8_t speed)
{
  if (i < act_fan_count)
  {
    lastSetFanSpeed[i] = fan[i].setFanSpeed = fan[i].curFanSpeed = speed;
  }
}

// Set whether the fan command has been sent
void fanSetSendWaiting(uint8_t index, bool isWaiting)
{
  fan_send_waiting[index] = isWaiting;
}

void loopFan(void)
{
  for (uint8_t i = 0; i < act_fan_count; i++)
  {
    if (lastSetFanSpeed[i] != fan[i].setFanSpeed)
    {
      lastSetFanSpeed[i] = fan[i].setFanSpeed;
      if (fan_send_waiting[i] != true)
      {
        fan_send_waiting[i] = storeCmd("%s ", fanCmd[i]);
      }
    }
  }
}

void fanQuerySetWait(bool wait)
{
  fanQueryWait = wait;
}

void fanSpeedQuery(void)
{
  if (infoSettings.fan_ctrl_count && !infoHost.wait && !fanQueryWait && infoHost.connected)
  {
    fanQueryWait = storeCmd("M710\n");
  }
}
