#include "BabystepControl.h"
#include "includes.h"

#define BABYSTEP_CMD     "M290 Z%.2f\n"
#define BABYSTEP_CMD_SMW "G43.2 Z%.2f\n"

static float babystep_value = BABYSTEP_DEFAULT_VALUE;

// set current babystep value
void babystepSetValue(float value)
{
  babystep_value = value;
}

// get current babystep value
float babystepGetValue(void)
{
  return babystep_value;
}

// reset babystep value to default value
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
      mustStoreCmd(babyStepCmd, last_unit);

    babystep_value = BABYSTEP_DEFAULT_VALUE;
  }

  return babystep_value;
}

// update babystep value
float babystepUpdateValue(float unit)
{
  unit = NOBEYOND(BABYSTEP_MIN_VALUE, babystep_value + unit, BABYSTEP_MAX_VALUE) - babystep_value;

  if (unit != 0)
  {
    babystep_value += unit;

    mustStoreCmd((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) ? BABYSTEP_CMD_SMW : BABYSTEP_CMD, unit);
  }

  return babystep_value;
}
