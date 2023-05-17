#include "ProbeOffsetControl.h"
#include "includes.h"

static float z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
static bool probe_offset_enabled = false;

// Enable probe offset
void probeOffsetEnable(float shim)
{
  probe_offset_enabled = true;

  probeHeightEnable();  // temporary disable software endstops and save ABL state

  // Z offset gcode sequence start
  probeHeightHome();  // home, disable ABL and raise nozzle

  float probedZ = 0.0f;

  if (infoSettings.probing_z_offset)  // if homing without a probe (e.g. with a min endstop)
  {
    levelingProbePoint(LEVEL_CENTER);  // probe center of bed

    TASK_LOOP_WHILE(levelingGetProbedPoint() == LEVEL_NO_POINT)  // if probed Z is set, exit from loop and read probed Z

    probedZ = levelingGetProbedZ();
    levelingResetProbedPoint();  // reset to check for new updates
  }

  probeHeightRelative();                                            // set relative position mode
  mustStoreCmd("G1 X%.2f Y%.2f\n",
               getParameter(P_PROBE_OFFSET, AXIS_INDEX_X),
               getParameter(P_PROBE_OFFSET, AXIS_INDEX_Y));         // move nozzle to XY probing point
  probeHeightStart(probedZ - probeOffsetGetValue() + shim, false);  // lower nozzle to probing Z0 point + shim
  probeOffsetSetValue(probedZ);                                     // set Z offset to match probing Z0 point
  probeHeightRelative();                                            // set relative position mode
}

// Disable probe offset
void probeOffsetDisable(void)
{
  probe_offset_enabled = false;

  // Z offset gcode sequence stop
  probeHeightHome();      // home, disable ABL and raise nozzle
  probeHeightAbsolute();  // set absolute position mode

  probeHeightDisable();  // restore original software endstops state and ABL state
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
  if (z_offset_value != PROBE_Z_OFFSET_DEFAULT_VALUE)  // if not default value
  {
    sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, PROBE_Z_OFFSET_DEFAULT_VALUE);  // set Z probe offset value
    mustStoreCmd("G1 Z%.2f\n", PROBE_Z_OFFSET_DEFAULT_VALUE - z_offset_value);     // move nozzle

    z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
  }

  return z_offset_value;
}

// Update Z offset value
float probeOffsetUpdateValue(float unit)
{
  unit = NOBEYOND(PROBE_Z_OFFSET_MIN_VALUE, z_offset_value + unit, PROBE_Z_OFFSET_MAX_VALUE) - z_offset_value;

  if (unit != 0)
  {
    z_offset_value += unit;

    sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z probe offset value
    mustStoreCmd("G1 Z%.2f\n", unit);                                // move nozzle
  }

  return z_offset_value;
}
