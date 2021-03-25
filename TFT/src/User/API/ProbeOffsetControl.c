#include "ProbeOffsetControl.h"
#include "includes.h"

static float z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
static bool probe_offset_enabled = false;

// Enable probe offset
void probeOffsetEnable(bool skipZOffset)
{
  probe_offset_enabled = true;

  probeHeightEnable();  // temporary disable software endstops

  // Z offset gcode sequence start
  mustStoreCmd("G28\n");                          // home printer
  probeHeightStop(infoSettings.z_raise_probing);  // raise nozzle

  if (infoSettings.xy_offset_probing)  // if HW allows nozzle to reach XY probing point
  {
    probeHeightRelative();                                  // set relative position mode
    mustStoreCmd("G1 X%.2f Y%.2f\n",
                 getParameter(P_PROBE_OFFSET, X_STEPPER),
                 getParameter(P_PROBE_OFFSET, Y_STEPPER));  // move nozzle to XY probing point and set feedrate
  }

  if (skipZOffset)
  {
    probeHeightStart(-probeOffsetGetValue(), false);  // lower nozzle to probing Z0 point
    probeOffsetSetValue(0.0f);                        // reset Z offset in order probing Z0 matches absolute Z0 point
  }
  else
  {
    probeHeightStart(0.0f, false);  // lower nozzle to absolute Z0 point
  }

  probeHeightRelative();  // set relative position mode
}

// Disable probe offset
void probeOffsetDisable(void)
{
  probe_offset_enabled = false;

  // Z offset gcode sequence stop
  mustStoreCmd("G28\n");                          // home printer
  probeHeightStop(infoSettings.z_raise_probing);  // raise nozzle
  probeHeightAbsolute();                          // set absolute position mode

  probeHeightDisable();  // restore original software endstops state
}

// Get probe offset status
bool probeOffsetGetStatus(void)
{
  return probe_offset_enabled;
}

// Set Z offset value
float probeOffsetSetValue(float value)
{
  mustStoreCmd("M851 Z%.2f\n", value);
  mustStoreCmd("M851\n");  // needed by probeOffsetGetValue() to retrieve the new value
  z_offset_value = value;

  return z_offset_value;
}

// Get current Z offset value
float probeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);

  return z_offset_value;
}

// Reset Z offset value to default value
float probeOffsetResetValue(void)
{
  if (z_offset_value == PROBE_Z_OFFSET_DEFAULT_VALUE)  // if already default value, nothing to do
    return z_offset_value;

  float unit = z_offset_value - PROBE_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
  mustStoreCmd("M851 Z%.2f\n", z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", -unit);             // move nozzle

  return z_offset_value;
}

// Decrease Z offset value
float probeOffsetDecreaseValue(float unit)
{
  if (z_offset_value > PROBE_Z_OFFSET_MIN_VALUE)
  {
    float diff = z_offset_value - PROBE_Z_OFFSET_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;
    z_offset_value -= unit;
    mustStoreCmd("M851 Z%.2f\n", z_offset_value);  // set Z offset value
    mustStoreCmd("G1 Z%.2f\n", -unit);             // move nozzle
  }

  return z_offset_value;
}

// Increase Z offset value
float probeOffsetIncreaseValue(float unit)
{
  if (z_offset_value < PROBE_Z_OFFSET_MAX_VALUE)
  {
    float diff = PROBE_Z_OFFSET_MAX_VALUE - z_offset_value;

    unit = (diff > unit) ? unit : diff;
    z_offset_value += unit;
    mustStoreCmd("M851 Z%.2f\n", z_offset_value);  // set Z offset value
    mustStoreCmd("G1 Z%.2f\n", unit);              // move nozzle
  }

  return z_offset_value;
}

// Update Z offset value by encoder
float probeOffsetUpdateValueByEncoder(float unit, int8_t direction)
{
  float overall_unit = (direction > 0) ? (direction * unit) : (-direction * unit);  // always positive unit

  if (direction < 0)  // if negative encoder value, decrease the value. Otherwise increase the value
    probeOffsetDecreaseValue(overall_unit);
  else
    probeOffsetIncreaseValue(overall_unit);

  return z_offset_value;
}
