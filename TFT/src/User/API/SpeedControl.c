#include "SpeedControl.h"
#include "includes.h"

static u16 percentage[SPEED_NUM]     = {100,   100};     //Speed  Flow
static u16 lastPercentage[SPEED_NUM] = {100,   100}; //Speed  Flow
static u16 curPercentage[SPEED_NUM]  = {100,   100};  //Speed  Flow

static bool send_waiting[SPEED_NUM];

void speedSetSendWaiting(u8 tool, bool isWaiting)
{
  send_waiting[tool] = isWaiting;
}

void speedSetPercent(u8 tool, u16 per)
{
  percentage[tool]=limitValue(SPEED_MIN, per, SPEED_MAX);
}

u16 speedGetPercent(u8 tool)
{
  return percentage[tool];
}

bool SpeedChanged(u8 i)
{
  if (lastPercentage[i] != percentage[i])
  {
    lastPercentage[i] = percentage[i];
    return true;
  }
  else
  {
    return false;
  }
}

void loopSpeed(void)
{
  for(u8 i = 0; i < SPEED_NUM;i++)
    if(curPercentage[i] != percentage[i])
    {
      curPercentage[i] = percentage[i];
      //if(send_waiting[i] != true)
      //{
        //send_waiting[i] = true;
      char *speedCmd[SPEED_NUM] = {"M220","M221"};
      storeCmd("%s S%d\n",speedCmd[i], percentage[i]);
      //}
    }
}
