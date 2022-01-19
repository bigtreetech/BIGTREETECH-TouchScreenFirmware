//TG MODIFIED BY T.GIOIOSA

#include "includes.h"
#include "SpindleSpeed.h"

/*
******************************************************************************************************************
This module is intended to manage requests for reporting of current spindle speed and auto-reporting of speed.
Currently in development.

T.Gioiosa 2/20/21
******************************************************************************************************************
*/

//TG remember....static also means only visible in this file!
// spindle speed reporting
static SPINDLE  spindle_motor = {{}, TOOL0};                    //TG 8/31/21 - **This is never used**
static int16_t lastTarget[MAX_SPINDLE_COUNT] = {0};             //TG 8/31/21 - **This is never used**
static uint8_t spindle_speed_update_seconds = 0;                //TG 8/31/21 - **This is never used**
static bool    spindle_speed_update_waiting = false;            //TG 8/31/21 - **This is never used**
static bool    spindle_speed_send_waiting[MAX_SPINDLE_COUNT];   //TG 8/31/21 - **This is never used**

uint32_t nextSpindleSpeedCheckTime = 0;   //TG 8/31/21 - **This is never used**
//TG 8/31/21 - **This is never used**
#define SPINDLE_SPEED_AUTOREPORT_TIMEOUT (nextSpindleSpeedCheckTime + 3000) // update interval + 3 second grace period

// Spindle speed reporting functions  //TG 2/10/21 added

// Set temperature/spindle update time interval
void spindleSpeedSetUpdateSeconds(uint8_t seconds)  //TG 8/31/21 - **This is never called**
{
  if (spindle_speed_update_seconds == seconds) return;

  spindle_speed_update_seconds = seconds;
  if (infoMachineSettings.autoReportTemp && !spindle_speed_update_waiting)
  {
    spindle_speed_update_waiting = storeCmd("M155 ");
  }
}

// Get query temperature/spindle seconds
uint8_t spindleSpeedGetUpdateSeconds(void)  //TG 8/31/21 - **This is never called**
{
  return spindle_speed_update_seconds;
}

// Set query temperature/spindle seconds
void spindleSpeedSyncUpdateSeconds(uint8_t seconds)  //TG 8/31/21 - **This is never called**
{
  spindle_speed_update_seconds = seconds;
}

// Set whether we need to query the current temperature/spindle
void spindleSpeedSetUpdateWaiting(bool isWaiting)  //TG 8/31/21 - **This is never called**
{
  spindle_speed_update_waiting = isWaiting;
}

// Set whether the heating command has been sent
void spindleSpeedSetSendWaiting(uint8_t index, bool isWaiting)  //TG 8/31/21 - **This is never called**
{
  spindle_speed_send_waiting[index] = isWaiting;
}

// Get whether has heating command in Queue
bool spindleSpeedGetSendWaiting(uint8_t index)  //TG 8/31/21 - **This is never called**
{
  return spindle_speed_send_waiting[index];
}

void updateNextSpindleSpeedCheckTime(void)  //TG 8/31/21 - **This is never called**
{
  nextSpindleSpeedCheckTime = OS_GetTimeMs() + spindle_speed_update_seconds * 1000;
}

// Is heating waiting to heat up
bool spindleSpeedGetIsWaiting(uint8_t index)  //TG 8/31/21 - **This is never called**
{
  return (spindle_motor.T[index].waiting != SP_WAIT_NONE);
}

// Check all heater if there is a heater waiting to be waited
bool spindleSpeedHasWaiting(void)  //TG 8/31/21 - **This is never called**
{
  for (uint8_t i = 0; i < MAX_TOOL_COUNT; i++)
  {
    if (spindle_motor.T[i].waiting != SP_WAIT_NONE)
      return true;
  }
  return false;
}

// Set heater waiting status
void spindleSpeedSetIsWaiting(uint8_t tool, SPINDLE_SPEED_WAIT isWaiting)  //TG 8/31/21 - **This is never called**
{
  spindle_motor.T[tool].waiting = isWaiting;

  if (isWaiting != (SPINDLE_SPEED_WAIT)WAIT_NONE) // wait heating now, query more frequently
  {
    spindleSpeedSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
  }
  else if (spindleSpeedHasWaiting() == false)
  {
    spindleSpeedSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }
}

void spindleSpeedClearIsWaiting(void)  //TG 8/31/21 - **This is never called**
{
  for (uint8_t i = 0; i < MAX_SPINDLE_COUNT; i++)
  {
    spindle_motor.T[i].waiting = WAIT_NONE;
  }
  spindleSpeedSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

// Set current Tool (Extruder)
void spindleSpeedSetCurrentTool(uint8_t tool)  //TG 8/31/21 - **This is never called**
{
  if (tool >= infoSettings.ext_count)
    return;
  spindle_motor.toolIndex = tool;
}

// Get current Tool (Extruder)
uint8_t spindleSpeedGetCurrentTool(void)  //TG 8/31/21 - **This is never called**
{
  return spindle_motor.toolIndex;
}

// Get current hotend index in arry T[]
uint8_t spindleSpeedGetCurrentHotend(void)  //TG 8/31/21 - **This is never called**
{
  return (infoSettings.hotend_count == 1) ? TOOL0 : spindle_motor.toolIndex;   //TG 1/9/20 to allow zero extruders  was MIN_EXT_COUNT
}

// Check whether the index is a valid spindle_motor index.
bool spindle_motor_IsValid(uint8_t index) //TG 8/31/21 - **This is never called**
{
  if (index >= infoSettings.hotend_count && index < MAX_SPINDLE_COUNT)
    return false;
  if (!infoSettings.bed_en && index == BED)
    return false;
  if (!infoSettings.chamber_en && index == CHAMBER)
    return false;
  return true;
}


void loopCheckSpindleSpeed(void)    //TG 8/31/21 - **This is never called**
{
  // Send M105 to query the temperatures, if motherboard does not supports M155 (AUTO_REPORT_TEMPERATURES) feature
  // to automatically report the temperatures.
  //TG This gets sent the very first time the backEnd loop runs after startup so first code sent to printer is M105
  if (!infoMachineSettings.autoReportTemp)
  {
    do
    {
      // Send M105 query temperature continuously
      if (spindle_speed_update_waiting == true)
      {
        updateNextSpindleSpeedCheckTime();
        break;
      }
      if (OS_GetTimeMs() < nextSpindleSpeedCheckTime)
        break;
      if (requestCommandInfoIsRunning())  // To avoid colision in Gcode response processing
        break;
      if (storeCmd("M105\n") == false)
        break;
      updateNextSpindleSpeedCheckTime();
      spindle_speed_update_waiting = true;
    } while (0);
  }
  else // check temperature auto-report timout and resend M155 command
  {
    if (OS_GetTimeMs() > SPINDLE_SPEED_AUTOREPORT_TIMEOUT && !spindle_speed_update_waiting)
    {
      spindle_speed_update_waiting = storeCmd("M155 ");  //TG 2/10/21 TODO  what gcode goes here??
      if(spindle_speed_update_waiting) updateNextSpindleSpeedCheckTime();  // set next timeout for temperature auto-report
    }
  }

  // Query the spindle that needs to wait for the speed to rise, whether it reaches the set speed
  for (uint8_t i = 0; i < MAX_SPINDLE_COUNT; i++)
  {
    if (spindle_motor.T[i].waiting == SP_WAIT_NONE)
    {
      continue;
    }
    else if (spindle_motor.T[i].waiting == SP_WAIT_HEATING)
    {
      if (spindle_motor.T[i].current + 2 <= spindle_motor.T[i].target)
        continue;
    }
    else if (spindle_motor.T[i].waiting == SP_WAIT_COOLING_HEATING)
    {
      if (inRange(spindle_motor.T[i].current, spindle_motor.T[i].target, 2) != true)
        continue;
    }

    spindle_motor.T[i].waiting = SP_WAIT_NONE;
    if (spindleSpeedHasWaiting())               // returns true id any heater is waiting
      continue;

    if (infoMenu.menu[infoMenu.cur] == menuSpindle)
      break;
    spindleSpeedSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }

  // If the target speed changes, send a Gcode to set the motherboard
  for (uint8_t i = 0; i < MAX_SPINDLE_COUNT; i++) // If the target temperature changes, send a Gcode to set the motherboard
  {
    if (lastTarget[i] != spindle_motor.T[i].target)
    {
      lastTarget[i] = spindle_motor.T[i].target;
      if (spindle_speed_send_waiting[i] != true)
      {
        spindle_speed_send_waiting[i] = storeCmd("%s ",heatCmd[i]);
      }
    }
  }
}
