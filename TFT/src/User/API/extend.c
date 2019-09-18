#include "extend.h"
#include "GPIO_Init.h"
#include "variants.h"
#include "includes.h"

#ifdef PS_ON_PIN
void PS_ON_Init(void)
{
  GPIO_InitSet(PS_ON_PIN, MGPIO_MODE_OUT_PP, 0);
  GPIO_SetLevel(PS_ON_PIN, PS_ON_ACTIVE_HIGH);
}
void PS_ON_On(void)
{
  GPIO_SetLevel(PS_ON_PIN, PS_ON_ACTIVE_HIGH);
}
void PS_ON_Off(void)
{
  GPIO_SetLevel(PS_ON_PIN, !PS_ON_ACTIVE_HIGH);
}
#endif

#ifdef FIL_RUNOUT_PIN
void FIL_Runout_Init(void)
{ 
  GPIO_InitSet(FIL_RUNOUT_PIN, FIL_RUNOUT_INVERTING ? MGPIO_MODE_IPD : MGPIO_MODE_IPU, 0);
}

void loopFILRunoutDetect(void)
{
  static u32 nowTime = 0;
  
  if(!infoSettings.runout)  return; //filament runout turn off
  if(!isPrinting() || isPause())  return; //not print or print is pause
  
  if(GPIO_GetLevel(FIL_RUNOUT_PIN) == FIL_RUNOUT_INVERTING)
  {
    if(OS_GetTime() <= nowTime + FIL_NOISE_THRESHOLD) return; //avoid noise
   
    if(setPrintPause(true))
      popupReminder(textSelect(LABEL_WARNING), textSelect(LABEL_FILAMENT_RUNOUT));
  }
  else
  {
    nowTime = OS_GetTime();
  }
}
#endif
