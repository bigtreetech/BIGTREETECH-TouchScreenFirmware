#include "SmartFeatures.h"
#include "includes.h"
#include "GPIO_Init.h"

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

#endif  // PS_ON_PIN

// Filament runout detection
#ifdef FIL_RUNOUT_PIN

enum
{
  FILAMENT_SENSOR_NORMAL,
  FILAMENT_SENSOR_SMART,
};

static bool posE_updateWaiting = false;
static bool sfs_alive = false;  // Use an encoder disc to toggles the runout. Suitable for BigTreeTech Smart Filament Sensor

void FIL_Runout_Init(void)
{
  #if defined(MKS_TFT)
    GPIO_InitSet(FIL_RUNOUT_PIN, MGPIO_MODE_IPN, 0);  // MKS TFTs already have an external pull-up resistor on PB0 and PB1 pins
  #else
    GPIO_InitSet(FIL_RUNOUT_PIN, infoSettings.runout_invert ? MGPIO_MODE_IPU : MGPIO_MODE_IPD, 0);
  #endif
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

// Set whether we need to query the current position
void FIL_PosE_SetUpdateWaiting(bool waiting)
{
  posE_updateWaiting = waiting;
}

void FIL_SFS_SetAlive(bool alive)
{
  sfs_alive = alive;
}

bool FIL_NormalRunoutDetect(void)
{
  static bool runout = false;
  static uint32_t trueTimes = 0;
  static uint32_t falseTimes = 0;
  static uint32_t nextRunoutTime = 0;

  if (OS_GetTimeMs() > nextRunoutTime)
  {
    runout = trueTimes > falseTimes ? true : false;
    trueTimes = 0;
    falseTimes = 0;
    nextRunoutTime = OS_GetTimeMs() + infoSettings.runout_noise_ms;
  }
  else
  {
    bool pinState = false;
    uint8_t toolNum = heatGetCurrentTool();

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
    }

    if (pinState)
    {
      trueTimes++;
    }
    else
    {
      falseTimes++;
    }
  }

  // Detect HIGH/LOW level, Suitable for general mechanical / photoelectric switches
  return (runout == infoSettings.runout_invert);
}

bool FIL_SmartRunoutDetect(void)
{
  static float lastPosE = 0.0f;
  static bool lastRunout = false;
  static uint32_t nextUpdateTime = 0;

  float posE = coordinateGetExtruderActual();
  bool runout = FIL_NormalRunoutDetect();

  do
  { // Send M114 E to query extrude position continuously
    if (posE_updateWaiting == true)
    {
      nextUpdateTime = OS_GetTimeMs() + POS_E_UPDATE_TIME;
      break;
    }

    if (OS_GetTimeMs() < nextUpdateTime)
      break;

    if (requestCommandInfoIsRunning())  // To avoid colision in Gcode response processing
      break;

    if (storeCmd("M114 E\n") == false)
      break;

    posE_updateWaiting = true;
    nextUpdateTime = OS_GetTimeMs() + POS_E_UPDATE_TIME;
  } while (0);

  if (sfs_alive == false)
  {
    if (lastRunout != runout)
    {
      sfs_alive = true;
    }
  }

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

bool FIL_IsRunout(void)
{
  if (infoSettings.runout & 1)
  {
    // Get sensor type
    uint8_t sensorType = (infoSettings.runout >> 1) & 1;

    switch (sensorType)
    {
      case FILAMENT_SENSOR_NORMAL:
        return FIL_NormalRunoutDetect();

      case FILAMENT_SENSOR_SMART:
        return FIL_SmartRunoutDetect();

      default:
        return false;
    }
  }

  return false;
}

void FIL_BE_CheckRunout(void)
{
  if (!(infoSettings.runout & 1))  // Filament runout turn off
    return;

  setPrintRunout(FIL_IsRunout());  // Need constant scanning to filter interference
}

void FIL_FE_CheckRunout(void)
{
  static uint32_t nextReminderTime = 0;

  if (!getPrintRunout() && !getRunoutAlarm())
    return;

  if (printPause(true, PAUSE_NORMAL) && !getRunoutAlarm())  // If not printing, printPause() function will always fail
  {                                                         // so no useless error message is displayed
    setRunoutAlarmTrue();
    setDialogText(LABEL_WARNING, LABEL_FILAMENT_RUNOUT, LABEL_CONFIRM, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_ALERT, setRunoutAlarmFalse, NULL, NULL);
  }

  if ((OS_GetTimeMs() > nextReminderTime) && (getRunoutAlarm() == true))
  {
    BUZZER_PLAY(sound_error);
    nextReminderTime = OS_GetTimeMs() + ALARM_REMINDER_TIME;
  }
}

#endif  // FIL_RUNOUT_PIN

// Smart home
#ifdef SMART_HOME

void loopCheckBack(void)
{
  static bool longPress = false;

  #ifdef HAS_EMULATOR
    static bool backHeld = false;
  #endif

  if (!isPress())
  {
    #ifdef HAS_EMULATOR
      backHeld = false;
    #endif

    longPress = false;

    #ifndef HAS_EMULATOR
      Touch_Enc_ReadPen(0);  // reset TSC press timer
    #endif

    return;
  }

  if (isPrinting())  // no jump to main menu while printing
    return;

  if (getMenuType() != MENU_TYPE_ICON)
    return;

  if ((infoMenu.cur == 0) || (infoMenu.menu[infoMenu.cur] == menuMode))
    return;

  #ifdef HAS_EMULATOR
    if (backHeld == true)  // prevent mode selection or screenshot if Back button is held
    {
      backHeld = Touch_Enc_ReadPen(0);
      return;
    }
  #endif

  if (longPress == false)  // check if longpress already handled
  {
    if (Touch_Enc_ReadPen(LONG_TOUCH))  // check if TSC is pressed and held
    {
      KEY_VALUES tempKey = KEY_IDLE;
      longPress = true;
      touchSound = false;

      if (infoMenu.menu[infoMenu.cur] == menuPrinting)
      {
        tempKey = Key_value(COUNT(rect_of_keySS), rect_of_keySS);
      }
      else
      {
        tempKey = Key_value(COUNT(rect_of_key), rect_of_key);
      }

      touchSound = true;

      if (tempKey != KEY_IDLE)
      {
        if (getCurMenuItems()->items[tempKey].label.index != LABEL_BACK)  // check if Back button is held
        {
          return;
        }
        else
        {
          BUZZER_PLAY(sound_ok);

          #ifdef HAS_EMULATOR
            backHeld = true;
          #endif

          infoMenu.menu[1] = infoMenu.menu[infoMenu.cur];  // prepare menu tree for jump to 0
          infoMenu.cur = 1;

          if (infoMenu.menu[1] == menuPrinting)
            clearInfoFile();
        }
      }
    }
  }
}

#endif  // SMART_HOME
