#include "CaseLightControl.h"
#include "includes.h"

static bool caseLightState = true;
static uint8_t caseLightBrightness = 0;

bool caseLightGetState(void)
{
  return caseLightState;
}

void caseLightSetState(bool state)
{
  caseLightState = state;
}

uint8_t caseLightGetBrightness(void)
{
  return caseLightBrightness;
}

void caseLightSetBrightness(uint8_t brightness)
{
  caseLightBrightness = NOBEYOND(0, brightness, 255);
}

uint8_t caseLightGetBrightnessPercent(void)
{
  return (caseLightBrightness * 100.0f) / 255 + 0.5f;
}

void caseLightSetBrightnessPercent(int8_t unit)
{
  unit = NOBEYOND(-100, unit, 100);
  caseLightBrightness = NOBEYOND(0, (caseLightGetBrightnessPercent() + unit) * 255 / 100.0f, 255);
}
