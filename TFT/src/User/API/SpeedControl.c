#include "SpeedControl.h"
#include "includes.h"

static uint16_t percent[SPEED_NUM]     = {100,   100};  //Speed  Flow
static uint16_t lastPercent[SPEED_NUM] = {100,   100};  //Speed  Flow
static uint16_t curPercent[SPEED_NUM]  = {100,   100};  //Speed  Flow

static bool send_waiting[SPEED_NUM];
static bool queryWait = false;

void speedSetSendWaiting(u8 tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void speedQuerySetWait(bool wait)
{
  queryWait = wait;
}

void speedSetPercent(u8 tool, u16 per)
{
  percent[tool]=NOBEYOND(SPEED_MIN, per, SPEED_MAX);
}

u16 speedGetPercent(u8 tool)
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
  for(u8 i = 0; i < SPEED_NUM;i++)
    if(curPercent[i] != percent[i])
    {
      curPercent[i] = percent[i];
      if(send_waiting[i] != true)
      {
        send_waiting[i] = true;
        const char *speedCmd[SPEED_NUM] = {"M220","M221"};
        storeCmd("%s S%d\n",speedCmd[i], percent[i]);
      }
    }
}

void speedQuery(void)
{
  if (infoHost.connected == true && infoHost.wait == false)
  {
    if (!queryWait)
    {
      storeCmd("M220\nM221\n");
      queryWait = true;
    }
  }
}
