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

  step_count = (babystep_value * neg) / BABYSTEP_MAX_UNIT;
  for (; step_count > 0; step_count--)
  {
    mustStoreCmd("M290 Z%.2f\n", -(BABYSTEP_MAX_UNIT * neg));
    processed_baby_step += BABYSTEP_MAX_UNIT;
  }

  last_unit = (babystep_value * neg) - processed_baby_step;
  if (last_unit > 0.0f)
  {
    mustStoreCmd("M290 Z%.2f\n", -(last_unit * neg));
    processed_baby_step += last_unit;
  }

  babystep_value -= (processed_baby_step * neg);

  return babystep_value;
}

// Decrease babystep value
float babystepDecreaseValue(float unit)
{
  if (babystep_value > BABYSTEP_MIN_VALUE)
  {
    float diff = babystep_value - BABYSTEP_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;
    mustStoreCmd("M290 Z-%.2f\n", unit);
    babystep_value -= unit;
  }

  return babystep_value;
}

// Increase babystep value
float babystepIncreaseValue(float unit)
{
  if (babystep_value < BABYSTEP_MAX_VALUE)
  {
    float diff = BABYSTEP_MAX_VALUE - babystep_value;

    unit = (diff > unit) ? unit : diff;
    mustStoreCmd("M290 Z%.2f\n", unit);
    babystep_value += unit;
  }

  return babystep_value;
}

// Update babystep value by encoder
float babystepUpdateValueByEncoder(float unit, int8_t direction)
{
  float overall_unit = (direction > 0) ? (direction * unit) : (-direction * unit);  // always positive unit

  if (direction < 0)  // if negative encoder value, decrease the value. Otherwise increase the value
    babystepDecreaseValue(overall_unit);
  else
    babystepIncreaseValue(overall_unit);

  return babystep_value;
}
