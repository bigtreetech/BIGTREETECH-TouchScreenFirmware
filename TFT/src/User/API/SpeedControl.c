#include "SpeedControl.h"
#include "includes.h"

const char *const speedCmd[SPEED_NUM] = {"M220","M221"};

static uint16_t setPercent[SPEED_NUM]     = {100, 100};  //Speed  Flow
static uint16_t lastSetPercent[SPEED_NUM] = {100, 100};  //Speed  Flow
static uint16_t curPercent[SPEED_NUM]  = {100, 100};  //Speed  Flow

static bool queryWait = false;

static uint32_t nextSpeedTime = 0;

#define NEXT_SPEED_WAIT 500  // 1 second is 1000

void speedSetCurPercent(uint8_t tool, uint16_t per)
{
  curPercent[tool] = per;
}

uint16_t speedGetCurPercent(uint8_t tool)
{
  return curPercent[tool];
}

void speedSetPercent(uint8_t tool, uint16_t per)
{
  setPercent[tool]=NOBEYOND(SPEED_MIN, per, SPEED_MAX);
}

uint16_t speedGetSetPercent(uint8_t tool)
{
  return setPercent[tool];
}

void loopSpeed(void)
{
  for (u8 i = 0; i < SPEED_NUM; i++)
  {
    if (lastSetPercent[i] != setPercent[i]  && (OS_GetTimeMs() > nextSpeedTime))
    {
      if (send_waiting[i] == false)
      {
        send_waiting[i] = true;
        send_waiting[i] = storeCmd("%s S%d D%d\n", speedCmd[i], percent[i], heatGetCurrentTool());
      }
      if (send_waiting[i] == true)
        curPercent[i] = percent[i];
      nextSpeedTime = OS_GetTimeMs() + NEXT_SPEED_WAIT; // avoid rapid fire, clogging the queue
    }
  }
}

void speedQuerySetWait(bool wait)
{
  queryWait = wait;
}

void speedQuery(void)
{
  if (infoHost.connected && !infoHost.wait && !queryWait)
  {
    storeCmd("M220\n");
    storeCmd("M221 D%d\n", heatGetCurrentTool());
    queryWait = true;
  }
}
