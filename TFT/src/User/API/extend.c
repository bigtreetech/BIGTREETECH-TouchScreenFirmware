#include "extend.h"
#include "GPIO_Init.h"
#include "variants.h"
#include "includes.h"

// Power Supply
#ifdef PS_ON_PIN
// Power Supply Control pins Initialization
void PS_ON_Init(void)
{
  GPIO_InitSet(PS_ON_PIN, MGPIO_MODE_OUT_PP, 0);
  GPIO_SetLevel(PS_ON_PIN, PS_ON_ACTIVE_HIGH);
}

// Power Supply Control turn on, M80
void PS_ON_On(void)
{
  GPIO_SetLevel(PS_ON_PIN, PS_ON_ACTIVE_HIGH);
}

// Power Supply Control turn off, M81
void PS_ON_Off(void)
{
  GPIO_SetLevel(PS_ON_PIN, !PS_ON_ACTIVE_HIGH);
}
#endif

// Filament runout detect
#ifdef FIL_RUNOUT_PIN

static bool update_waiting = false;
/* Set whether we need to query the current position */
void positionSetUpdateWaiting(bool isWaiting)
{
  update_waiting = isWaiting;
}

void FIL_Runout_Init(void)
{ 
  GPIO_InitSet(FIL_RUNOUT_PIN, FIL_RUNOUT_INVERTING ? MGPIO_MODE_IPD : MGPIO_MODE_IPU, 0);
}

bool FIL_RunoutPinFilteredLevel(void)
{
  static bool rst = false;  
  static u32 nowTime = 0;
  static u32 trueTimes = 0;
  static u32 falseTimes = 0;
  
  if (OS_GetTime() > nowTime + FIL_NOISE_THRESHOLD)
  {
    rst = trueTimes > falseTimes ? true : false;
    nowTime = OS_GetTime();
    trueTimes = 0;
    falseTimes = 0;
  }
  else
  {
    if (GPIO_GetLevel(FIL_RUNOUT_PIN))
    {
      trueTimes++;
    }
    else
    {
      falseTimes++;
    }
  }
  return rst;
}


static u32 update_time = 200;
// Use an encoder disc to toggles the runout
// Suitable for BigTreeTech Smart filament detecter 
bool FIL_SmartRunoutDetect(void)
{
  static float lastExtrudePosition = 0.0f;
  static uint8_t lastRunoutPinLevel = 0;
  static uint8_t isAlive = false;  
  static u32  nowTime=0;
  
  bool pinLevel = FIL_RunoutPinFilteredLevel();
  float actualExtrude = coordinateGetAxisActual(E_AXIS);

  do
  {  /* Send M114 E query extrude position continuously	*/
    if(update_waiting == true)                {nowTime=OS_GetTime();break;}
    if(OS_GetTime()<nowTime+update_time)       break;
    if(RequestCommandInfoIsRunning())          break; //to avoid colision in Gcode response processing
    if(storeCmd("M114 E\n")==false)            break;

    nowTime=OS_GetTime();
    update_waiting=true;
  }while(0);
  
  if (isAlive == false)
  {
    if (lastRunoutPinLevel != pinLevel)
    {
      isAlive = true;
    }  
  }
  
  if (ABS(actualExtrude - lastExtrudePosition) >= FILAMENT_RUNOUT_DISTANCE_MM)
  {
    lastExtrudePosition = actualExtrude;
    if (isAlive)
    {
      isAlive = false;
      lastRunoutPinLevel =  pinLevel;
    }
    else
    {
      return true;
    }
  }
  return false;
}

bool FIL_IsRunout(void)
{
  switch (infoSettings.runout) {
    case FILAMENT_RUNOUT_ON:
      // Detect HIGH/LOW level, Suitable for general mechanical / photoelectric switches
      return (FIL_RunoutPinFilteredLevel() == FIL_RUNOUT_INVERTING);
    
    case FILAMENT_SMART_RUNOUT_ON:
      return FIL_SmartRunoutDetect();
    
    default:
      return false;
  }
}

void loopFILRunoutDetect(void)
{  
  if (infoSettings.runout == FILAMENT_RUNOUT_OFF)  return; // Filament runout turn off
  if (!FIL_IsRunout()) return; // Filament not runout yet, need constant scanning to filter interference
  if (!isPrinting() || isPause())  return; // No printing or printing paused
  
  if (setPrintPause(true))
  {
    popupReminder(textSelect(LABEL_WARNING), textSelect(LABEL_FILAMENT_RUNOUT));
  }
}

#endif
