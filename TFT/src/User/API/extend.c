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
  GPIO_SetLevel(PS_ON_PIN, infoSettings.ps_active_high);
}

// Power Supply Control turn on, M80
void PS_ON_On(void)
{
  GPIO_SetLevel(PS_ON_PIN, infoSettings.ps_active_high);
}

// Power Supply Control turn off, M81
void PS_ON_Off(void)
{
  GPIO_SetLevel(PS_ON_PIN, !infoSettings.ps_active_high);
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
  GPIO_InitSet(FIL_RUNOUT_PIN, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #ifdef FIL_RUNOUT_PIN_1
    GPIO_InitSet(FIL_RUNOUT_PIN_1, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_2
    GPIO_InitSet(FIL_RUNOUT_PIN_2, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_3
    GPIO_InitSet(FIL_RUNOUT_PIN_3, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_4
    GPIO_InitSet(FIL_RUNOUT_PIN_4, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_5
    GPIO_InitSet(FIL_RUNOUT_PIN_5, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #endif
}

bool FIL_RunoutPinFilteredLevel(void)
{
  static bool rst = false;
  static u32 nextTime = 0;
  static u32 trueTimes = 0;
  static u32 falseTimes = 0;

  if (OS_GetTimeMs() > nextTime)
  {
    rst = trueTimes > falseTimes ? true : false;
    nextTime = OS_GetTimeMs() + infoSettings.runout_noise_ms ;
    trueTimes = 0;
    falseTimes = 0;
  }
  else
  {
    bool filRunout = 0;
    uint8_t toolNum = heatGetCurrentTool();
    switch(toolNum)
    {
      case 0:
        filRunout = GPIO_GetLevel(FIL_RUNOUT_PIN);
        break;
      #ifdef FIL_RUNOUT_PIN_1
        case 1:
          filRunout = GPIO_GetLevel(FIL_RUNOUT_PIN_1);
          break;
      #endif
      #ifdef FIL_RUNOUT_PIN_2
        case 2:
          filRunout = GPIO_GetLevel(FIL_RUNOUT_PIN_2);
          break;
      #endif
      #ifdef FIL_RUNOUT_PIN_3
        case 3:
          filRunout = GPIO_GetLevel(FIL_RUNOUT_PIN_3);
          break;
      #endif
      #ifdef FIL_RUNOUT_PIN_4
        case 4:
          filRunout = GPIO_GetLevel(FIL_RUNOUT_PIN_4);
          break;
      #endif
      #ifdef FIL_RUNOUT_PIN_5
        case 5:
          filRunout = GPIO_GetLevel(FIL_RUNOUT_PIN_5);
          break;
      #endif
    }
    if (filRunout)
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


static u32 update_time = 2000;
// Use an encoder disc to toggles the runout
// Suitable for BigTreeTech Smart filament detecter
bool FIL_SmartRunoutDetect(void)
{
  static float lastExtrudePosition = 0.0f;
  static uint8_t lastRunoutPinLevel = 0;
  static uint8_t isAlive = false;
  static u32  nextTime=0;

  bool pinLevel = FIL_RunoutPinFilteredLevel();
  float actualExtrude = coordinateGetAxisActual(E_AXIS);

  do
  {  /* Send M114 E query extrude position continuously	*/
    if(update_waiting == true)        {nextTime=OS_GetTimeMs()+update_time;break;}
    if(OS_GetTimeMs()<nextTime)       break;
    if(requestCommandInfoIsRunning()) break; //to avoid colision in Gcode response processing
    if(storeCmd("M114 E\n")==false)   break;

    nextTime=OS_GetTimeMs()+update_time;
    update_waiting=true;
  }while(0);

  if (isAlive == false)
  {
    if (lastRunoutPinLevel != pinLevel)
    {
      isAlive = true;
    }
  }

  if (ABS(actualExtrude - lastExtrudePosition) >= infoSettings.runout_distance)
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
      return (FIL_RunoutPinFilteredLevel() == infoSettings.runout_invert);

    case FILAMENT_SMART_RUNOUT_ON:
      return FIL_SmartRunoutDetect();

    default:
      return false;
  }
}

void loopBackEndFILRunoutDetect(void)
{
  if (infoSettings.runout == FILAMENT_RUNOUT_OFF)  return; // Filament runout turn off
  if (!FIL_IsRunout()) return; // Filament not runout yet, need constant scanning to filter interference
  if (!isPrinting() || isPause())  return; // No printing or printing paused

  setPrintRunout(true);
}

void loopFrontEndFILRunoutDetect(void)
{
  if (!getPrintRunout()) return;

  if (setPrintPause(true,false))
  {
    setPrintRunout(false);
    popupReminder(DIALOG_TYPE_ERROR, textSelect(LABEL_WARNING), textSelect(LABEL_FILAMENT_RUNOUT));
  }
}
#endif
