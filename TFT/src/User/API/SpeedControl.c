#include "SpeedControl.h"
#include "includes.h"

#define NEXT_SPEED_WAIT 500  // 1 second is 1000

const char *const speedCmd[SPEED_NUM] = {"M220", "M221"};

static uint16_t setPercent[SPEED_NUM] = {100, 100};
static uint16_t curPercent[SPEED_NUM] = {100, 100};
static uint8_t  needSetPercent = 0;

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
  SET_BIT_VALUE(needSetPercent, tool, value != curPercent[tool]);
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
    if (infoSettings.ext_count == 0 && i > 0)
    {
      // Don't poll M221 if there are no extruders
      continue;
    }

    if (GET_BIT(needSetPercent, i) && (OS_GetTimeMs() > nextSpeedTime))
    {
      if (storeCmd("%s S%d D%d\n", speedCmd[i], setPercent[i], heatGetCurrentTool()))
      {
        SET_BIT_OFF(needSetPercent, i);
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
    if (infoSettings.ext_count > 0)
    {
      speedQueryWait = (storeCmd("M220\n") | storeCmd("M221\n"));
    }
    else
    {
      speedQueryWait = storeCmd("M220\n");
    }
  }
}
