#include "SpeedControl.h"
#include "includes.h"

static uint16_t percent[SPEED_NUM]     = {100,   100};  //Speed  Flow
static uint16_t lastPercent[SPEED_NUM] = {100,   100};  //Speed  Flow
static uint16_t curPercent[SPEED_NUM]  = {100,   100};  //Speed  Flow

static bool sendSpeed_waiting[SPEED_NUM];
static bool speedQueryWait = false;

static uint32_t nextSpeedTime = 0;
#define nextSpeedWait  500  // 1 second is 1000 

void speedSetSendWaiting(u8 tool, bool isWaiting)
{
  sendSpeed_waiting[tool] = isWaiting;
}

void speedQuerySetWait(bool wait)
{
  speedQueryWait = wait;
}

void speedSetRcvPercent(uint8_t tool, uint16_t per)
{
  curPercent[tool] = percent[tool] = per;
}

void speedSetPercent(uint8_t tool, uint16_t per)
{
  percent[tool]=NOBEYOND(SPEED_MIN, per, SPEED_MAX);
}

uint16_t speedGetPercent(uint8_t tool)
{
  return percent[tool];
}

bool SpeedChanged(u8 i)
{
  if (lastPercent[i] != percent[i])
  {
    lastPercent[i] = percent[i];
    sendSpeed_waiting[i] = false;
    return true;
  }
  else
  {
    sendSpeed_waiting[i] = true;
    return false;
  }
}

void loopSpeed(void)
{
  if (OS_GetTimeMs() > nextSpeedTime)
  {
    for(u8 i = 0; i < SPEED_NUM;i++)
      if((curPercent[i] != percent[i]) )
      {
        if(sendSpeed_waiting[i] == false)
        {
          sendSpeed_waiting[i] = true;
          const char *speedCmd[SPEED_NUM] = {"M220","M221"};
          sendSpeed_waiting[i] = storeCmd("%s S%d\n",speedCmd[i], percent[i]);
        }
      if (sendSpeed_waiting[i] == true) curPercent[i] = percent[i];
      }
    nextSpeedTime = OS_GetTimeMs() + nextSpeedWait; // avoid rapid fire, clogging the queue
  }
}

void speedQuery(void)
{
  if (infoHost.connected == true && infoHost.wait == false)
  {
    if (!speedQueryWait)
    {
      speedQueryWait = storeCmd("M220\nM221\n");
    }
  }
}
