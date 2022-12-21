#include "BabystepControl.h"
#include "includes.h"

static float babystep_value = BABYSTEP_DEFAULT_VALUE;

#define BABYSTEP_CMD     "M290 Z%.2f\n"
#define BABYSTEP_CMD_SMW "G43.2 Z%.2f\n"

// Reset only babystep value to default value
float babystepReset(void)
{
  babystep_value = BABYSTEP_DEFAULT_VALUE;

  return babystep_value;
}

// Set current babystep value
void babystepSetValue(const float value)
{
  babystep_value = value;
}

// Get current babystep value
float babystepGetValue(void)
{
  return babystep_value;
}

// Reset babystep value to default value
float babystepResetValue(void)
{
  if (babystep_value != BABYSTEP_DEFAULT_VALUE)  // if already default value, nothing to do
  {
    char * babyStepCmd = (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) ? BABYSTEP_CMD_SMW : BABYSTEP_CMD;
    uint32_t step_count;
    float last_unit;
    int8_t direction = 1;

    if (babystep_value > BABYSTEP_DEFAULT_VALUE)
      direction = -1;

    step_count = ((BABYSTEP_DEFAULT_VALUE - babystep_value) * direction) / BABYSTEP_MAX_STEP;

    for (; step_count > 0; step_count--)
    {
      mustStoreCmd(babyStepCmd, BABYSTEP_MAX_STEP * direction);
    }

    last_unit = (BABYSTEP_DEFAULT_VALUE - babystep_value) - (BABYSTEP_MAX_STEP * step_count * direction);
    if (last_unit != 0.0f)
    {
      mustStoreCmd(babyStepCmd, last_unit);
    }

    babystep_value = BABYSTEP_DEFAULT_VALUE;
  }

  return babystep_value;
}

// Update babystep value
float babystepUpdateValue(float unit, const int8_t direction)
{
  char * babyStepCmd = (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) ? BABYSTEP_CMD_SMW : BABYSTEP_CMD;
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

  mustStoreCmd(babyStepCmd, unit);

  return babystep_value;
}
