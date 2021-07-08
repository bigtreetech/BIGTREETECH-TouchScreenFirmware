#include "HomeOffsetControl.h"
#include "includes.h"

static float z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
static bool home_offset_enabled = false;

// Enable home offset
void homeOffsetEnable(bool skipZOffset, float shim)
{
  home_offset_enabled = true;

  probeHeightEnable();  // temporary disable software endstops

  // Z offset gcode sequence start
  mustStoreCmd("G28\n");          // home printer
  probeHeightStart(shim, false);  // lower nozzle to absolute Z0 point + shim
  probeHeightRelative();          // set relative position mode
}

// Disable home offset
void homeOffsetDisable(void)
{
  home_offset_enabled = false;

  // Z offset gcode sequence stop
  mustStoreCmd("G28\n");  // home printer
  probeHeightAbsolute();  // set absolute position mode

  probeHeightDisable();  // restore original software endstops state
}

// Get home offset status
bool homeOffsetGetStatus(void)
{
  return home_offset_enabled;
}

// Set Z offset value
float homeOffsetSetValue(float value)
{
  sendParameterCmd(P_HOME_OFFSET, AXIS_INDEX_Z, value);
  mustStoreCmd("M206\n");  // needed by homeOffsetResetValue() to retrieve the new value
  z_offset_value = value;

  return z_offset_value;
}

// Get current Z offset value
float homeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_HOME_OFFSET, AXIS_INDEX_Z);

  return z_offset_value;
}

// Reset Z offset value to default value
float homeOffsetResetValue(void)
{
  if (z_offset_value == HOME_Z_OFFSET_DEFAULT_VALUE)  // if already default value, nothing to do
    return z_offset_value;

  float unit = z_offset_value - HOME_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
  sendParameterCmd(P_HOME_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", unit);              // move nozzle

  return z_offset_value;
}

// Update Z offset value
float homeOffsetUpdateValue(float unit, int8_t direction)
{
  float diff;

  if (direction < 0)
  {
    if (z_offset_value <= HOME_Z_OFFSET_MIN_VALUE)
      return z_offset_value;

    diff = z_offset_value - HOME_Z_OFFSET_MIN_VALUE;
  }
  else
  {
    if (z_offset_value >= HOME_Z_OFFSET_MAX_VALUE)
      return z_offset_value;

    diff = HOME_Z_OFFSET_MAX_VALUE - z_offset_value;
  }

  unit = ((diff > unit) ? unit : diff) * direction;
  z_offset_value -= unit;
  sendParameterCmd(P_HOME_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", unit);              // move nozzle

  return z_offset_value;
}
