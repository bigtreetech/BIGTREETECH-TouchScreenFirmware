//TG MODIFIED BY T.GIOIOSA

#include "SpeedControl.h"
#include "includes.h"

const char *const speedCmd[SPEED_NUM] = {"M220","M221"};

static uint16_t setPercent[SPEED_NUM]     = {100, 100};  //Speed  Flow
static uint16_t lastSetPercent[SPEED_NUM] = {100, 100};  //Speed  Flow
static uint16_t curPercent[SPEED_NUM]  = {100, 100};  //Speed  Flow

static bool speedQueryWait = false;
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
  for (uint8_t i = 0; i < SPEED_NUM; i++)
  {
    if (lastSetPercent[i] != setPercent[i]  && (OS_GetTimeMs() > nextSpeedTime))
    {
      if (storeCmd("%s S%d D%d\n",speedCmd[i], setPercent[i], heatGetCurrentTool()))
        lastSetPercent[i] = setPercent[i];
      nextSpeedTime = OS_GetTimeMs() + NEXT_SPEED_WAIT;  // avoid rapid fire, clogging the queue
    }
  }
}

void speedQuerySetWait(bool wait)
{
  speedQueryWait = wait;
}

void speedQuery(void)   // ask printer for speed and flow of current hotend  //TG 2/21/21 mod to ignore M221
{
  if (infoHost.connected && !infoHost.wait && !speedQueryWait)
  {
    if(infoSettings.hotend_count > 0) 
      speedQueryWait = storeCmd("M220\nM221 D%d\n",heatGetCurrentTool());
    else  
      speedQueryWait = storeCmd("M220\n");

  }
}
