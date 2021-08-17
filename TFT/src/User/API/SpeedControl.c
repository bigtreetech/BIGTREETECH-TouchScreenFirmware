#include "SpeedControl.h"
#include "includes.h"

#define NEXT_SPEED_WAIT 500  // 1 second is 1000

const char *const speedCmd[SPEED_NUM] = {"M220", "M221"};

static uint16_t setPercent[SPEED_NUM] = {100, 100};
static bool needSetPercent[SPEED_NUM] = {false, false};
static uint16_t curPercent[SPEED_NUM] = {100, 100};

static bool speedQueryWait = false;
static uint32_t nextSpeedTime = 0;

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
  uint16_t value = NOBEYOND(SPEED_MIN, per, SPEED_MAX);
  needSetPercent[tool] = value != curPercent[tool];
  setPercent[tool] = value;
}

uint16_t speedGetSetPercent(uint8_t tool)
{
  return setPercent[tool];
}

void loopSpeed(void)
{
  for (uint8_t i = 0; i < SPEED_NUM; i++)
  {
    if (needSetPercent[i] && (OS_GetTimeMs() > nextSpeedTime))
    {
      if (storeCmd("%s S%d D%d\n", speedCmd[i], setPercent[i], heatGetCurrentTool()))
      {
        needSetPercent[i] = false;
      }

      nextSpeedTime = OS_GetTimeMs() + NEXT_SPEED_WAIT;  // avoid rapid fire, clogging the queue
    }
  }
}

void speedQuerySetWait(bool wait)
{
  speedQueryWait = wait;
}

void speedQuery(void)
{
  if (infoHost.connected && !infoHost.wait && !speedQueryWait && infoMachineSettings.firmwareType != FW_REPRAPFW)
  {
    speedQueryWait = storeCmd("M220\nM221\n");
  }
}
