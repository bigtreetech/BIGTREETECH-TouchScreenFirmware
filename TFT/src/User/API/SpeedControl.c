#include "SpeedControl.h"
#include "includes.h"

const char *const speedCmd[SPEED_NUM] = {"M220","M221"};

static uint16_t percent[SPEED_NUM]     = {100, 100};  //Speed  Flow
static uint16_t lastPercent[SPEED_NUM] = {100, 100};  //Speed  Flow
static uint16_t curPercent[SPEED_NUM]  = {100, 100};  //Speed  Flow

static bool send_waiting[SPEED_NUM];
static bool queryWait = false;

static uint32_t nextSpeedTime = 0;

#define NEXT_SPEED_WAIT 500  // 1 second is 1000

void speedSetSendWaiting(u8 tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void speedQuerySetWait(bool wait)
{
  queryWait = wait;
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
    send_waiting[i] = false;
    return true;
  }
  else
  {
    send_waiting[i] = true;
    return false;
  }
}

void loopSpeed(void)
{
  for (u8 i = 0; i < SPEED_NUM; i++)
  {
    if ((curPercent[i] != percent[i]) && (OS_GetTimeMs() > nextSpeedTime))
    {
      if (send_waiting[i] == false)
      {
        send_waiting[i] = true;
        send_waiting[i] = storeCmd("%s S%d\n", speedCmd[i], percent[i]);
      }
      if (send_waiting[i] == true)
        curPercent[i] = percent[i];
      nextSpeedTime = OS_GetTimeMs() + NEXT_SPEED_WAIT; // avoid rapid fire, clogging the queue
    }
  }
}

void speedQuery(void)
{
  if (infoHost.connected && !infoHost.wait && !queryWait)
  {
    storeCmd("M220\nM221\n");
    queryWait = true;
  }
}
