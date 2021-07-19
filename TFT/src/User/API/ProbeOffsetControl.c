#include "ProbeOffsetControl.h"
#include "includes.h"

static float z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
static bool probe_offset_enabled = false;

// Enable probe offset
void probeOffsetEnable(bool skipZOffset, float shim)
{
  probe_offset_enabled = true;

  probeHeightEnable();  // temporary disable software endstops

  // Z offset gcode sequence start
  mustStoreCmd("G28\n");                          // home printer
  probeHeightStop(infoSettings.z_raise_probing);  // raise nozzle

  if (infoSettings.xy_offset_probing)  // if HW allows nozzle to reach XY probing point
  {
    probeHeightRelative();                                     // set relative position mode
    mustStoreCmd("G1 X%.2f Y%.2f\n",
                 getParameter(P_PROBE_OFFSET, AXIS_INDEX_X),
                 getParameter(P_PROBE_OFFSET, AXIS_INDEX_Y));  // move nozzle to XY probing point and set feedrate
  }

  if (skipZOffset)
  {
    probeHeightStart(-probeOffsetGetValue() + shim, false);  // lower nozzle to probing Z0 point + shim
    probeOffsetSetValue(0.0f);                               // reset Z offset in order probing Z0 matches absolute Z0 point
  }
  else
  {
    probeHeightStart(shim, false);  // lower nozzle to absolute Z0 point + shim
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
  sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, value);
  mustStoreCmd("M851\n");  // needed by probeOffsetGetValue() to retrieve the new value
  z_offset_value = value;

  return z_offset_value;
}

// Get current Z offset value
float probeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_PROBE_OFFSET, AXIS_INDEX_Z);

  return z_offset_value;
}

// Reset Z offset value to default value
float probeOffsetResetValue(void)
{
  if (z_offset_value == PROBE_Z_OFFSET_DEFAULT_VALUE)  // if already default value, nothing to do
    return z_offset_value;

  float unit = z_offset_value - PROBE_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
  sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", -unit);                               // move nozzle

  return z_offset_value;
}

// Update Z offset value
float probeOffsetUpdateValue(float unit, int8_t direction)
{
  float diff;

  if (direction < 0)
  {
    if (z_offset_value <= PROBE_Z_OFFSET_MIN_VALUE)
      return z_offset_value;

    diff = z_offset_value - PROBE_Z_OFFSET_MIN_VALUE;
  }
  else
  {
    if (z_offset_value >= PROBE_Z_OFFSET_MAX_VALUE)
      return z_offset_value;

    diff = PROBE_Z_OFFSET_MAX_VALUE - z_offset_value;
  }

  unit = ((diff > unit) ? unit : diff) * direction;
  z_offset_value += unit;
  sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", unit);                                // move nozzle

  return z_offset_value;
}
