#include "ProbeOffsetControl.h"
#include "includes.h"

static float z_offset_value = PROBE_OFFSET_DEFAULT_VALUE;
static bool probe_offset_enabled = false;

/* Enable probe offset */
void probeOffsetEnable(void)
{
  probe_offset_enabled = true;

  // Z offset gcode sequence start
  mustStoreCmd("M211 S0\n");                               // disable Software Endstop to move nozzle minus Zero (Z0) if necessary
  mustStoreCmd("G28\n");                                   // home printer
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 X%.2f Y%.2f\n",
    getParameter(P_PROBE_OFFSET, X_STEPPER),
    getParameter(P_PROBE_OFFSET, Y_STEPPER));              // move nozzle to XY probing point
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("G1 Z0 F%d\n",                              // move nozzle to Z0 absolute point and set feedrate
    infoSettings.axis_speed[infoSettings.move_speed]);
  mustStoreCmd("G91\n");                                   // set relative position mode
}

/* Disable probe offset */
void probeOffsetDisable(void)
{
  probe_offset_enabled = false;

  // Z offset gcode sequence stop
  mustStoreCmd("G28\n");                                   // home printer
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 Z%.2f\n", infoSettings.pause_z_raise ); // raise Z
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("M211 S1\n");                               // enable Software Endstop
}

/* Get probe offset status */
bool probeOffsetGetStatus(void)
{
  return probe_offset_enabled;
}

/* Set Z offset value */
float probeOffsetSetValue(float value)
{
  mustStoreCmd("M851 Z%.2f\n", value);

  z_offset_value = value;

  return z_offset_value;
}

/* Get current Z offset value */
float probeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);

  return z_offset_value;
}

/* Reset Z offset value to default value */
float probeOffsetResetValue()
{
  if (z_offset_value == PROBE_OFFSET_DEFAULT_VALUE)        // if already default value, nothing to do
    return z_offset_value;

  float unit = z_offset_value - PROBE_OFFSET_DEFAULT_VALUE;

  mustStoreCmd("M851 Z%.2f\n", PROBE_OFFSET_DEFAULT_VALUE);// set the Z offset value
  mustStoreCmd("G1 Z%.2f\n", -unit);                       // move the nozzle

  z_offset_value = PROBE_OFFSET_DEFAULT_VALUE;

  return z_offset_value;
}

// Decrease Z offset value
float probeOffsetDecreaseValue(float unit)
{
  if (z_offset_value > PROBE_OFFSET_MIN_VALUE)
  {
    float diff = z_offset_value - PROBE_OFFSET_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;

    mustStoreCmd("M851 Z%.2f\n", z_offset_value - unit);   // set the Z offset value
    mustStoreCmd("G1 Z%.2f\n", -unit);                     // move the nozzle

    z_offset_value -= unit;
  }

  return z_offset_value;
}

// Increase Z offset value
float probeOffsetIncreaseValue(float unit)
{
  if (z_offset_value < PROBE_OFFSET_MAX_VALUE)
  {
    float diff = PROBE_OFFSET_MAX_VALUE - z_offset_value;

    unit = (diff > unit) ? unit : diff;

    mustStoreCmd("M851 Z%.2f\n", z_offset_value + unit);   // set the Z offset value
    mustStoreCmd("G1 Z%.2f\n", unit);                      // move the nozzle

    z_offset_value += unit;
  }

  return z_offset_value;
}

// Update Z offset value by encoder
float probeOffsetUpdateValueByEncoder(float unit)
{
#if LCD_ENCODER_SUPPORT
  if (encoderPosition)
  {
    float overall_unit = (encoderPosition < 0.0f) ? (-1 * unit * encoderPosition) : (unit * encoderPosition);          // always positive unit

    if (encoderPosition < 0.0)                             // if negative encoder value, decrease the value. Otherwise increase the value
      probeOffsetDecreaseValue(overall_unit);
    else
      probeOffsetIncreaseValue(overall_unit);
  }
#endif

  return z_offset_value;
}
