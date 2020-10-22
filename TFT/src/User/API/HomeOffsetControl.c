#include "HomeOffsetControl.h"
#include "includes.h"

static float home_offset_z_value = HOME_OFFSET_Z_DEFAULT_VALUE;


/* Set Z offset value */
float homeOffsetSetValue(float value)
{
  mustStoreCmd("M206 Z%.2f\n", value);

  home_offset_z_value = value;

  return home_offset_z_value;
}

/* Get current Z offset value */
float homeOffsetGetValue(void)
{
  home_offset_z_value = getParameter(P_HOME_OFFSET, Z_STEPPER);

  return home_offset_z_value;
}