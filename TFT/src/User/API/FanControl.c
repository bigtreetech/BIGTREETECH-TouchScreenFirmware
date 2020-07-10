#include "FanControl.h"
#include "includes.h"


const char* fanID[] = FAN_DISPLAY_ID;
const char* fanCmd[] = FAN_CMD;
const char* fanSignID[] = FAN_SIGN_ID;

static u8   fanSpeed[MAX_FAN_COUNT] = {0};
static u8   lastfanSpeed[MAX_FAN_COUNT] = {0};
static u8   curfanSpeed[MAX_FAN_COUNT] = {0};
static u8   curIndex = 0;
static bool send_waiting[MAX_FAN_COUNT] = {false};


void fanSetSpeed(u8 i, u8 speed)
{
  fanSpeed[i] = speed;
}

u8 fanGetSpeed(u8 i)
{
  return fanSpeed[i];
}

void fanSetSpeedPercent(u8 i, int16_t percent)
{
  percent = limitValue(0, percent, 100);
  fanSpeed[i] = (percent * infoSettings.fan_max[i]) / 100;
}

u8 fanGetSpeedPercent(u8 i)
{
  return ((fanSpeed[i] * 100) / infoSettings.fan_max[i]);
}

void fanSetCurIndex(u8 i)
{
  if(i >= infoSettings.fan_count) return;
  curIndex = i;
}

u8 fanGetCurIndex(u8 i)
{
  return curIndex;
}

void fanSetSendWaiting(u8 i, bool isWaiting)
{
  send_waiting[i] = isWaiting;
}

bool fanSpeedChanged(u8 i)
{
  if (fanSpeed[i] != lastfanSpeed[i])
  {
    lastfanSpeed[i] = fanSpeed[i];
    return true;
  }
  else
  {
    return false;
  }
}

void loopFan(void)
{
  if (curfanSpeed[curIndex] != fanSpeed[curIndex])
  {
    curfanSpeed[curIndex] = fanSpeed[curIndex];
    // if(send_waiting[curIndex] != true)
    // {
    //send_waiting[curIndex] = true;
    storeCmd("%s S%d\n", fanCmd[curIndex], fanSpeed[curIndex]);
    //}
  }
}
