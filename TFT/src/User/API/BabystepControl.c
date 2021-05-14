#include "BabystepControl.h"
#include "includes.h"

static float babystep_value = BABYSTEP_DEFAULT_VALUE;

// Reset only babystep value to default value
float babystepReset(void)
{
  babystep_value = BABYSTEP_DEFAULT_VALUE;

  return babystep_value;
}

// Get current babystep value
float babystepGetValue(void)
{
  return babystep_value;
}

// Reset babystep value to default value
float babystepResetValue(void)
{
  if (babystep_value == BABYSTEP_DEFAULT_VALUE)  // if already default value, nothing to do
    return babystep_value;

  int step_count;
  float last_unit;
  float processed_baby_step = 0.0f;
  int8_t neg = 1;

  if (babystep_value < 0.0f)
    neg = -1;

  step_count = (babystep_value * neg) / BABYSTEP_MAX_STEP;
  for (; step_count > 0; step_count--)
  {
    if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      mustStoreCmd("G43.2 Z%.2f\n", -(BABYSTEP_MAX_STEP * neg));
    else
      mustStoreCmd("M290 Z%.2f\n", -(BABYSTEP_MAX_STEP * neg));

    processed_baby_step += BABYSTEP_MAX_STEP;
  }

  last_unit = (babystep_value * neg) - processed_baby_step;
  if (last_unit > 0.0f)
  {
    if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      mustStoreCmd("G43.2 Z%.2f\n", -(last_unit * neg));
    else
      mustStoreCmd("M290 Z%.2f\n", -(last_unit * neg));

    processed_baby_step += last_unit;
  }

  babystep_value -= (processed_baby_step * neg);

  return babystep_value;
}

// Update babystep value
float babystepUpdateValue(float unit, int8_t direction)
{
  float diff;

  if (direction < 0)
  {
    if (babystep_value <= BABYSTEP_MIN_VALUE)
      return babystep_value;

    diff = babystep_value - BABYSTEP_MIN_VALUE;
  }
  else
  {
    if (babystep_value >= BABYSTEP_MAX_VALUE)
      return babystep_value;

    diff = BABYSTEP_MAX_VALUE - babystep_value;
  }

  unit = ((diff > unit) ? unit : diff) * direction;
  babystep_value += unit;

  if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
    mustStoreCmd("G43.2 Z%.2f\n", unit);
  else
    mustStoreCmd("M290 Z%.2f\n", unit);

  return babystep_value;
}
