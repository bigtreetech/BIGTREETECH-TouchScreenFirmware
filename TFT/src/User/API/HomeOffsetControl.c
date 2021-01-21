#include "HomeOffsetControl.h"
#include "includes.h"

static float z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
static bool home_offset_enabled = false;

// Enable home offset
void homeOffsetEnable(bool skipZOffset)
{
  home_offset_enabled = true;

  probeHeightEnable();  // temporary disable software endstops

  // Z offset gcode sequence start
  mustStoreCmd("G28\n");  // home printer
  mustStoreCmd("G1 F%d\n", infoSettings.level_feedrate[FEEDRATE_XY]);  // set feedrate
  probeHeightStart(0.0f);  // lower nozzle to absolute Z0 point
  probeHeightRelative();   // set relative position mode
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
  mustStoreCmd("M206 Z%.2f\n", value);
  mustStoreCmd("M206\n");  // needed by homeOffsetResetValue() to retrieve the new value
  z_offset_value = value;
  return z_offset_value;
}

// Get current Z offset value
float homeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_HOME_OFFSET, Z_STEPPER);
  return z_offset_value;
}

// Reset Z offset value to default value
float homeOffsetResetValue(void)
{
  if (z_offset_value == HOME_Z_OFFSET_DEFAULT_VALUE)  // if already default value, nothing to do
    return z_offset_value;

  float unit = z_offset_value - HOME_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
  mustStoreCmd("M206 Z%.2f\n", z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", unit);              // move nozzle
  return z_offset_value;
}

// Decrease Z offset value
float homeOffsetDecreaseValue(float unit)
{
  if (z_offset_value > HOME_Z_OFFSET_MIN_VALUE)
  {
    float diff = z_offset_value - HOME_Z_OFFSET_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;
    z_offset_value += unit;
    mustStoreCmd("M206 Z%.2f\n", z_offset_value);  // set Z offset value
    mustStoreCmd("G1 Z%.2f\n", -unit);             // move nozzle
  }

  return z_offset_value;
}

// Increase Z offset value
float homeOffsetIncreaseValue(float unit)
{
  if (z_offset_value < HOME_Z_OFFSET_MAX_VALUE)
  {
    float diff = HOME_Z_OFFSET_MAX_VALUE - z_offset_value;

    unit = (diff > unit) ? unit : diff;
    z_offset_value -= unit;
    mustStoreCmd("M206 Z%.2f\n", z_offset_value);  // set Z offset value
    mustStoreCmd("G1 Z%.2f\n", unit);              // move nozzle
  }

  return z_offset_value;
}

// Update Z offset value by encoder
float homeOffsetUpdateValueByEncoder(float unit, int8_t direction)
{
  float overall_unit = (direction > 0) ? (direction * unit) : (-direction * unit);  // always positive unit

  if (direction < 0)  // if negative encoder value, decrease the value. Otherwise increase the value
    homeOffsetDecreaseValue(overall_unit);
  else
    homeOffsetIncreaseValue(overall_unit);

  return z_offset_value;
}
