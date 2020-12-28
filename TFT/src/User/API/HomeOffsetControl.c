#include "HomeOffsetControl.h"
#include "includes.h"

static float z_offset = HOME_Z_OFFSET_DEFAULT_VALUE;

/* Set Z offset value */
float homeOffsetSetValue(float value)
{
  mustStoreCmd("M206 Z%.2f\n", value);

  z_offset = value;

  return z_offset;
}

/* Get current Z offset value */
float homeOffsetGetValue(void)
{
  z_offset = getParameter(P_HOME_OFFSET, Z_STEPPER);

  return z_offset;
}
