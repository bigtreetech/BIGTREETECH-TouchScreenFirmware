#include "AddonHardware.h"
#include "includes.h"
#include "GPIO_Init.h"

//
// power supply
//

#ifdef PS_ON_PIN

void PS_ON_Init(void)
{
  GPIO_InitSet(PS_ON_PIN, MGPIO_MODE_OUT_PP, 0);
  GPIO_SetLevel(PS_ON_PIN, infoSettings.ps_active_high);
}

void PS_ON_On(void)
{
  GPIO_SetLevel(PS_ON_PIN, infoSettings.ps_active_high);
}

void PS_ON_Off(void)
{
  GPIO_SetLevel(PS_ON_PIN, !infoSettings.ps_active_high);
}

#endif  // PS_ON_PIN

//
// filament runout detection
//

#ifdef FIL_RUNOUT_PIN

#define FIL_POS_E_REFRESH_TIME  2000
#define FIL_ALARM_REMINDER_TIME 10000

enum
{
  FILAMENT_SENSOR_NORMAL,
  FILAMENT_SENSOR_SMART,
};

static uint32_t posE_nextUpdateTime = FIL_ALARM_REMINDER_TIME;  // give TFT time to connect to mainboard first before polling for runout
static bool posE_sendingWaiting = false;
static bool sfs_alive = false;  // use an encoder disc to toggles the runout. Suitable for BigTreeTech Smart Filament Sensor

void FIL_Runout_Init(void)
{
  GPIO_MODE pull =
  #if defined(MKS_TFT)
    MGPIO_MODE_IPN;  // MKS TFTs already have an external pull-up resistor on PB0 and PB1 pins
  #else
    (GET_BIT(infoSettings.runout, RUNOUT_INVERTED) ^ GET_BIT(infoSettings.runout, RUNOUT_NO_NC)) ? MGPIO_MODE_IPD : MGPIO_MODE_IPU;
  #endif

  GPIO_InitSet(FIL_RUNOUT_PIN, pull, 0);

  #ifdef FIL_RUNOUT_PIN_1
    GPIO_InitSet(FIL_RUNOUT_PIN_1, pull, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_2
    GPIO_InitSet(FIL_RUNOUT_PIN_2, pull, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_3
    GPIO_InitSet(FIL_RUNOUT_PIN_3, pull, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_4
    GPIO_InitSet(FIL_RUNOUT_PIN_4, pull, 0);
  #endif
  #ifdef FIL_RUNOUT_PIN_5
    GPIO_InitSet(FIL_RUNOUT_PIN_5, pull, 0);
  #endif
}

void FIL_PosE_ClearSendingWaiting(void)
{
  posE_sendingWaiting = false;
}

void FIL_SFS_SetAlive(bool alive)
{
  sfs_alive = alive;
}

bool FIL_NormalRunoutDetect(void)
{
  static bool runout = false;
  static int32_t trigBalance = 0;
  static uint32_t nextUpdateTime = 0;

  if (OS_GetTimeMs() < nextUpdateTime)
  {
    bool pinState = false;
    uint8_t toolNum = heatGetToolIndex();

    switch (toolNum)
    {
      case 0:
        pinState = GPIO_GetLevel(FIL_RUNOUT_PIN);
        break;

      #ifdef FIL_RUNOUT_PIN_1
        case 1:
          pinState = GPIO_GetLevel(FIL_RUNOUT_PIN_1);
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN_2
        case 2:
          pinState = GPIO_GetLevel(FIL_RUNOUT_PIN_2);
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN_3
        case 3:
          pinState = GPIO_GetLevel(FIL_RUNOUT_PIN_3);
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN_4
        case 4:
          pinState = GPIO_GetLevel(FIL_RUNOUT_PIN_4);
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN_5
        case 5:
          pinState = GPIO_GetLevel(FIL_RUNOUT_PIN_5);
          break;
      #endif

      default:
        pinState = GPIO_GetLevel(FIL_RUNOUT_PIN);
        break;
    }

    trigBalance += (pinState == GET_BIT(infoSettings.runout, RUNOUT_INVERTED)) ? 1: -1;  // if triggered add 1 else substract 1

    return runout;
  }

  // if OS_GetTimeMs() >= nextUpdateTime

  runout = (trigBalance > 0);
  trigBalance = 0;
  nextUpdateTime = OS_GetTimeMs() + infoSettings.runout_noise;

  return runout;
}

static inline bool FIL_SmartRunoutDetect(void)
{
  static float lastPosE = 0.0f;
  static bool lastRunout = false;

  float posE = coordinateGetExtruderActual();
  bool runout = FIL_NormalRunoutDetect();

  do
  { // send M114 E to query extrude position continuously

    if (OS_GetTimeMs() < posE_nextUpdateTime)  // if next check time not yet elapsed, do nothing
      break;

    posE_nextUpdateTime = OS_GetTimeMs() + FIL_POS_E_REFRESH_TIME;  // extend next check time

    // if M114 previously enqueued and not yet sent or pending command
    // (to avoid collision in gcode response processing), do nothing
    if (posE_sendingWaiting || requestCommandInfoIsRunning())
      break;

    posE_sendingWaiting = storeCmd("M114 E\n");
  } while (0);

  if (!sfs_alive && lastRunout != runout)
    sfs_alive = true;

  if (ABS(posE - lastPosE) >= infoSettings.runout_distance)
  {
    lastPosE = posE;

    if (sfs_alive)
    {
      sfs_alive = false;
      lastRunout = runout;
    }
    else
    {
      return true;
    }
  }

  return false;
}

static inline bool FIL_IsRunout(void)
{
  switch (GET_BIT(infoSettings.runout, RUNOUT_SENSOR_TYPE))
  {
    case FILAMENT_SENSOR_NORMAL:
      return FIL_NormalRunoutDetect();

    case FILAMENT_SENSOR_SMART:
      return FIL_SmartRunoutDetect();

    default:
      return false;
  }
}

void FIL_BE_CheckRunout(void)
{
  if (!GET_BIT(infoSettings.runout, RUNOUT_ENABLED))  // filament runout turned off
    return;

  setPrintRunout(FIL_IsRunout());  // need constant scanning to filter interference
}

void FIL_FE_CheckRunout(void)
{
  static uint32_t nextReminderTime = 0;

  if (!getPrintRunout() && !getRunoutAlarm())
    return;

  if (pausePrint(true, PAUSE_NORMAL) && !getRunoutAlarm())  // if not printing, pausePrint() function will always fail
  {                                                         // so no useless error message is displayed
    setRunoutAlarmTrue();
    popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_FILAMENT_RUNOUT, LABEL_CONFIRM, LABEL_NULL, setRunoutAlarmFalse, NULL, NULL);
  }

  if (OS_GetTimeMs() >= nextReminderTime && getRunoutAlarm())
  {
    BUZZER_PLAY(SOUND_ERROR);
    nextReminderTime = OS_GetTimeMs() + FIL_ALARM_REMINDER_TIME;
  }
}

#endif  // FIL_RUNOUT_PIN
