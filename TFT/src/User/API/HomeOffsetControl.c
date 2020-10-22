#include "HomeOffsetControl.h"
#include "includes.h"

static float z_home_offset_value = HOME_OFFSET_DEFAULT_VALUE;


/* Set Z offset value */
float probeOffsetSetValue(float value)
{
  mustStoreCmd("M206 Z%.2f\n", value);

  z_home_offset_value = value;

  return z_home_offset_value;
}

/* Get current Z offset value */
float probeOffsetGetValue(void)
{
  z_home_offset_value = getParameter(P_HOME_OFFSET, Z_STEPPER);

  return z_home_offset_value;
}