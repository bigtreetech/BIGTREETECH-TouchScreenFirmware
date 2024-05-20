#include "HomeOffsetControl.h"
#include "includes.h"

static float z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
static bool home_offset_enabled = false;

// enable home offset
void homeOffsetEnable(float shim)
{
  home_offset_enabled = true;

  probeHeightEnable();  // temporary disable software endstops and save ABL state

  // Z offset gcode sequence start
  probeHeightHomeAndNoABL();      // home and disable ABL
  probeHeightStart(shim, false);  // lower nozzle to absolute Z0 point + shim
  probeHeightRelative();          // set relative position mode
}

// disable home offset
void homeOffsetDisable(void)
{
  home_offset_enabled = false;

  // Z offset gcode sequence stop
  probeHeightHomeAndNoABL();  // home and disable ABL
  probeHeightAbsolute();      // set absolute position mode

  probeHeightDisable();  // restore original software endstops state and ABL state
}

// get home offset status
bool homeOffsetGetStatus(void)
{
  return home_offset_enabled;
}

// set Z offset value
float homeOffsetSetValue(float value)
{
  sendParameterCmd(P_HOME_OFFSET, AXIS_INDEX_Z, value);
  mustStoreCmd("M206\n");  // needed by homeOffsetResetValue() to retrieve the new value
  z_offset_value = value;

  return z_offset_value;
}

// get current Z offset value
float homeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_HOME_OFFSET, AXIS_INDEX_Z);

  return z_offset_value;
}

// reset Z offset value to default value
float homeOffsetResetValue(void)
{
  if (z_offset_value != HOME_Z_OFFSET_DEFAULT_VALUE)  // if not default value
  {
    sendParameterCmd(P_HOME_OFFSET, AXIS_INDEX_Z, HOME_Z_OFFSET_DEFAULT_VALUE);  // set Z home offset value
    mustStoreCmd("G1 Z%.2f\n", z_offset_value - HOME_Z_OFFSET_DEFAULT_VALUE);    // move nozzle

    z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
  }

  return z_offset_value;
}

// update Z offset value
float homeOffsetUpdateValue(float unit)
{
  unit = z_offset_value - NOBEYOND(HOME_Z_OFFSET_MIN_VALUE, z_offset_value - unit, HOME_Z_OFFSET_MAX_VALUE);

  if (unit != 0)
  {
    z_offset_value -= unit;

    sendParameterCmd(P_HOME_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z home offset value
    mustStoreCmd("G1 Z%.2f\n", unit);                               // move nozzle
  }

  return z_offset_value;
}
