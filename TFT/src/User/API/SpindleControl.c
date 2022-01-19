//TG 2/4/2021 This module is new, created to support Spindle control, handles speed unit conversions
// and sending of M3/M4/M5 spindle speed commands via setSpindleSpeed[] and curSpindleSpeed[] variables.
// Function loopSpindle() is called by loopBackEnd() in menu.c to update the spindle speed to the machine.

#include "SpindleControl.h"
#include "Spindle.h"

uint8_t spindleType[MAX_SPINDLE_COUNT]; //TG Can this be deleted??

//TG remember....static also means only visible in this file!
static uint16_t setSpindleSpeed[MAX_SPINDLE_COUNT] = {0}; 
uint16_t lastSetSpindleSpeed[MAX_SPINDLE_COUNT] = {0};
static uint16_t curSpindleSpeed[MAX_SPINDLE_COUNT] = {0};

static bool spindleQueryWait = false;
static bool spindleQueryEnable = false;
static uint32_t nextSpindleTime = 0;
#define NEXT_SPINDLE_WAIT 500  // in msec, this is rate at which spindle speed commands are sent to Marlin

//uint8_t spindleGetTypID(uint8_t startIndex, uint8_t type)     //TG Can be deleted, not needed as spindles aren't typed
//{
//  for (uint8_t i = startIndex; i < MAX_SPINDLE_COUNT; i++)
//  {
//    if (spindleType[i] == type)
//      return i;
//  }
//  return SPINDLE_TYPE_UNKNOWN;
//}

//TG 8/31/21 not needed since only one spindle is allowed and the list is already defined in Spindle.c
//void spindleBuildList(void)
//{
//  char* spindleIDTemp[MAX_SPINDLE_COUNT] = SPINDLE_DISPLAY_ID;
//  char* spindleCmdTemp[MAX_SPINDLE_COUNT] = SPINDLE_CMD;
//  uint8_t spindleTypeTemp[MAX_SPINDLE_COUNT] = SPINDLE_TYPE;
//
//  for (uint8_t i = 0, j = 0; i < MAX_SPINDLE_COUNT; i++, j++)
//  {
//    if (infoSettings.spindle_count == i)
//    {
//      if (infoSettings.spindle_ctrl_count > 0)
//        i = (MAX_SPINDLE_COUNT - MAX_SPINDLE_CTRL_COUNT);
//      else
//        i = MAX_SPINDLE_COUNT;
//    }
//    if (i < MAX_SPINDLE_COUNT)
//    {
//      memcpy(spindleID + j, spindleIDTemp + i, sizeof(spindleIDTemp[i]));
//      memcpy(spindleCmd + j, spindleCmdTemp + i, sizeof(spindleCmdTemp[i]));
//      memcpy(spindleType + j, spindleTypeTemp + i, sizeof(spindleTypeTemp[i]));
//    }
//  }
//}

void spindleControlInit(void)
{
  //spindleBuildList();   //TG 8/31/21 not needed since only one spindle is allowed and the list is already defined in Spindle.c
    spindleQueryEnable = infoSettings.spindle_ctrl_count;
  
  //spindleQueryEnable = (infoSettings.spindle_ctrl_count > 0 &&
  //                  (spindleGetTypID(infoSettings.spindle_count -1, SPINDLE_TYPE_CTRL_I) ||
  //                   spindleGetTypID(infoSettings.spindle_count -1, SPINDLE_TYPE_CTRL_S)));
}

//TG 8/31/21 not needed since only one spindle is allowed
//bool spindleIsType(uint8_t i, uint8_t type)
//{
//  return (spindleType[i] == type);
//}

/* Notes on speed units selection

Spindle speed(power) can be set/displayed in units of RPM, %, or PWM255. The master setting is made in Marlin Configuration_adv.h.
The LCD units should ideally match the Marlin setting, but can be switched. However, inaccuracies can result between certain
pairs of LCD/Marlin unit settings due to the use of integers.
The best resolution and zero error is RPM/RPM. Second is PWM/PWM with less resolution, and Third is %/% with the least resolution.
All other combinations introduce error in the LCD value displayed (compared to the input step chosen) and in the output accuracy.
See below for details:

LCD    Marlin   Conversion                   Output value to Marlin                     LCD displayed value
-----  ------   ------------------------------------------------------------------------------------------------------------------
RPM     RPM     no conversion                output = input                             LCD display shows RPM as entered
%       RPM     RPM = %*MAXRPM               fractional, +/- 1 RPM error                LCD display shows % as entered
PWM     RPM     RPM = PWM/255*MAXRPM         fractional, +/- 1 RPM error                LCD display error up to +/- 1 PWM count 
RPM     %       % = RPM/MAXRPM               fractional, +/- 1 % error                  LCD display error up to +/- 1% of MAXRPM
%       %       no conversion                output = input                             LCD display shows % as entered
PWM     %       % = PWM/MAXPWM               fractional, +/- 1 % error                  LCD display error up to +/- 1 PWM count
RPM     PWM     PWM = RPM/MAXRPM*PWMMAX      fractional, +/- 1 PWM count error          LCD display error up to +/- 1% of MAXPWM
%       PWM     PWM = %*MAXPWM               fractional, +/- 1 PWM count error          LCD display shows % as entered   
PWM     PWM     no conversion                output = input                             LCD display shows PWM count as entered

*/

// convert LCD display speed units to Marlin units 
// convert input speed from LCD units to Marlin units. Works best when both use the same unit. LCD % to Marlin RPM
// also works well. If Marlin is in %, then conversion loses accuracy due to speeds being integers and % being non-float.
uint16_t convertSpeed_LCD_2_Marlin(uint8_t s_index, uint16_t input){
  MTYPE lcd_units = infoSettings.cutter_disp_unit;
  switch(infoSettings.cutter_power_unit)
  {
    case MRPM:  //targeting Marlin RPM mode
      {
        if (lcd_units==MPWM) {input = input * infoSettings.spindle_rpm_max[s_index] / MAX_SPINDLE_PWM_SPEED  + 0.5f;}
        else if (lcd_units==MPCT) {input = input * infoSettings.spindle_rpm_max[s_index] / 100.0f + 0.5f;}
        break;
      }
    case MPCT:  //targeting Marlin PCT mode
      {
        if (lcd_units==MRPM) {input = input * 100.0f / infoSettings.spindle_rpm_max[s_index] + 0.5f;}
        else if (lcd_units==MPWM) {input = input * 100.0f / MAX_SPINDLE_PWM_SPEED  + 0.5f;}
        break;
      }
    case MPWM:  //targeting Marlin PWM mode
      {
       if (lcd_units==MRPM) {input = input * MAX_SPINDLE_PWM_SPEED / infoSettings.spindle_rpm_max[s_index] + 0.5f;}
       else if (lcd_units==MPCT) {input = input * MAX_SPINDLE_PWM_SPEED / 100.0f  + 0.5f;}
       break;
      }
    default:
     break;
  }
  return input;
}

// convert speeds (which are always stored in Marlin units) to LCD units for display
// convert input speed from Marlin units to LCD units. Works best when both use the same unit. LCD % to Marlin RPM
// also works well. If Marlin is in %, then conversion loses accuracy due to speeds being integers and % being non-float.
uint16_t convertSpeed_Marlin_2_LCD(uint8_t s_index, uint16_t input){
  MTYPE marlin_units = infoSettings.cutter_power_unit;
  int16_t dispPower = input;     // direct copy is default, no conversion will be chosen
  switch(infoSettings.cutter_disp_unit)
  {
    case MRPM:  //convert all to  disp as RPM
      {
        if (marlin_units==MPWM) {dispPower = input * (float)infoSettings.spindle_rpm_max[s_index] / (float)MAX_SPINDLE_PWM_SPEED  + 0.5f;}
        else if (marlin_units==MPCT) {dispPower = input * (float)infoSettings.spindle_rpm_max[s_index] / 100.0f + 0.5f;}
        break;
      }
    case MPCT:  //convert all to  disp as PCT
      {
        if (marlin_units==MRPM) {dispPower = input * 100.0f / (float)infoSettings.spindle_rpm_max[s_index] + 0.5f;}
        else if (marlin_units==MPWM) {dispPower = input * 100.0f / (float)MAX_SPINDLE_PWM_SPEED  + 0.5f;}
        break;
      }
    case MPWM:  //convert all to  disp as PWM
      {
       if (marlin_units==MRPM) {dispPower = input * (float)MAX_SPINDLE_PWM_SPEED / (float)infoSettings.spindle_rpm_max[s_index] + 0.5f;}
       else if (marlin_units==MPCT) {dispPower = input * (float)MAX_SPINDLE_PWM_SPEED / 100.0f  + 0.5f;}
       break;
      }
    default:
     break;
  }
  return dispPower;
}


// convert speeds (which are always stored in Marlin units) to LCD inits for display
uint16_t getSpeedUnits_Marlin_2_LCD(MTYPE marlin_units, MTYPE lcd_units, uint8_t s_index){
  int16_t curPower = setSpindleSpeed[s_index];
  int16_t dispPower = curPower;   // direct copy is default, no conversion will be chosen
  switch(lcd_units)
  {
    case MRPM:  //convert all to  disp as RPM
      {
        if (marlin_units==MPWM) {dispPower = curPower * (float)infoSettings.spindle_rpm_max[s_index] / (float)MAX_SPINDLE_PWM_SPEED  + 0.5f;}
        else if (marlin_units==MPCT) {dispPower = curPower * (float)infoSettings.spindle_rpm_max[s_index] / 100.0f + 0.5f;}
        break;
      }

    case MPCT:  //convert all to  disp as PCT
      {
        if (marlin_units==MRPM) {dispPower = curPower * 100.0f / (float)infoSettings.spindle_rpm_max[s_index] + 0.5f;}
        else if (marlin_units==MPWM) {dispPower = curPower * 100.0f / (float)MAX_SPINDLE_PWM_SPEED  + 0.5f;}
        break;
      }

    case MPWM:  //convert all to  disp as PWM
      {
       if (marlin_units==MRPM) {dispPower = curPower * (float)MAX_SPINDLE_PWM_SPEED / (float)infoSettings.spindle_rpm_max[s_index] + 0.5f;}
       else if (marlin_units==MPCT) {dispPower = curPower * (float)MAX_SPINDLE_PWM_SPEED / 100.0f  + 0.5f;}
       break;
      }

    default:
     break;
  }
  return dispPower;
}

// gets LCD Step values from appropriate arrays based on LCD display units in force.
uint16_t getLCD_Units_StepValue(uint8_t spIndex,uint8_t stepsIndex)  {  //TG choose step value based on disp_unit setting
  switch(infoSettings.cutter_disp_unit) {
    // switch function requires the cases to be in numerical ascending order
    case MRPM:
      return RPMSteps[stepsIndex];
      break;
    case MPCT:
      return percentSteps[stepsIndex];
      break;
    case MPWM:
      return PWMSteps[stepsIndex];
      break;
    default:
    return 0;
      break;
  }
}


// check the current speed setting is within bounds based on what units mode we are sending to Marlin
uint16_t checkPowerLimits(uint8_t s_index, uint16_t newpower){
switch(infoSettings.cutter_power_unit)          // check limits against marlin current format and return valid value
  {
  case MRPM:  //targeting Marlin RPM mode
    {
      newpower = NOBEYOND(0,newpower,(uint16_t)infoSettings.spindle_rpm_max[s_index]);
      break;
    }
  case MPCT:  //targeting Marlin PCT mode
    {
      newpower =  NOBEYOND(0,newpower,100);
      break;
    }
  case MPWM:  //targeting Marlin PWM mode
    {
      newpower =  NOBEYOND(0,newpower,MAX_SPINDLE_PWM_SPEED);
      break;
    }
  default:
   break;
  }
  return newpower;
}

void spindleSetSpeed(uint8_t i, uint16_t speed)
{
  setSpindleSpeed[i] = speed;
}
uint16_t spindleGetSetSpeed(uint8_t i)
{
  return setSpindleSpeed[i];
}
void spindleSetCurSpeed(uint8_t i, uint16_t speed)
{
  curSpindleSpeed[i] = speed;
}
uint16_t spindleGetCurSpeed(uint8_t i)
{
  return curSpindleSpeed[i];
}

//TG this is called by loopBackEnd() in menu.c to update the spindle speed to the machine for TFT speed control
//external control from Marlin on receipt of M3/4/5 commands is handled in parseAck.c, just before parse_end: label
void loopSpindle(void)    
{
  // do for each spindle (currently there is only one spindle allowed)
  for (uint8_t i = 0; i < (infoSettings.spindle_count + infoSettings.spindle_ctrl_count); i++)
  {
    //TG Only if speed has changed and it's time to update spindle speed
    if ((lastSetSpindleSpeed[i] != setSpindleSpeed[i]) && (OS_GetTimeMs() > nextSpindleTime))
    // gets the "M" from spindleCmd[i], adds "04" or "03" per infoSettings.spin_dir, then "S" plus setSpindleSpeed[i]
    // giving "M04 Sxxx" or "M03 Sxxx"  The xxx can be rpm or percentage depending on infoSettings.cutter_disp_unit
    {
      if(spindleState == 1)  // if state is ON
      {  
          // issue command, CCW spin=0, CW spin=1 along with commanded speed
          if (storeCmd("%s%s S%d\n", spindleCmd[i], infoSettings.spin_dir==0 ? "04": "03", setSpindleSpeed[i]))
          {  
            lastSetSpindleSpeed[i] = setSpindleSpeed[i];
            if((vacuumState & 2) == 2)    // turn vacuum on if in auto mode (state bit 1 is set)
              vacuum_set(255);            // set vacuum on
          }
      }
      else if(setSpindleSpeed[i] == 0)    // state is OFF
      {
          // issue stop command
          if (storeCmd("%s\n", "M05"))
          {
            lastSetSpindleSpeed[i] = setSpindleSpeed[i];
            if((vacuumState & 2) == 2)    // turn vacuum off if in auto mode (state bit 1 is set)
              vacuum_set(0);              // set vacuum off
          }
      }
      nextSpindleTime = OS_GetTimeMs() + NEXT_SPINDLE_WAIT; // avoid rapid fire, clogging the queue
    } // if value changed and time to send
  } // for loop 
}// loopSpindle

void spindleQuerySetWait(bool wait)
{
  spindleQueryWait = wait;
}

//TG 8/31/21 not used
//void spindleSpeedQuery(void)
//{
//  if (infoHost.connected && !infoHost.wait && !spindleQueryWait && spindleQueryEnable)
//  {
//    spindleQueryWait = storeCmd("M710\n");
//  }
//}

