#include "SpeedControl.h"
#include "includes.h"

static u16 percent[SPEED_NUM]     = {100,   100};     //Speed  Flow
static u16 lastPercent[SPEED_NUM] = {100,   100}; //Speed  Flow
static u16 curPercent[SPEED_NUM]  = {100,   100};  //Speed  Flow

static bool send_waiting[SPEED_NUM];

void speedSetSendWaiting(u8 tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void speedSetPercent(u8 tool, u16 per)
{
  percent[tool]=limitValue(SPEED_MIN, per, SPEED_MAX);
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
