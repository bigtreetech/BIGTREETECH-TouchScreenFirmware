#include "CaseLightControl.h"
#include "includes.h"

static uint8_t caseLightBrightness = 0;
static uint8_t lastCaseLightBrightness = 0;
static bool lastCaseLightState = true;
static bool caseLightState = true;

static bool lightQueryWait = false;
static bool caseLight_send_waiting = false;

// Sends a query (M355) to the printer asking for the current brightness
void caseLightValueQuery(void)
{
  if (infoMachineSettings.caseLightsBrightness == ENABLED && infoHost.connected &&
      !infoHost.wait && !lightQueryWait)
  {
    lightQueryWait = storeCmd("M355\n");
  }
}

void caseLightSetState(bool state)
{
  caseLightState = state;
}

void caseLightToggleState(void)
{
  caseLightState = !caseLightState;
}

void caseLightSetBrightness(uint8_t brightness)
{
  caseLightBrightness = NOBEYOND(0, brightness, 255);
}

void caseLightChangeBrightness(int8_t brightness_delta)
{
  caseLightBrightness = NOBEYOND(0, caseLightBrightness + brightness_delta, 255);
}

void caseLightChangeBrightnessPrecent(int8_t brightness_delta_percent)
{
  brightness_delta_percent = NOBEYOND(-100, brightness_delta_percent, 100);
  caseLightBrightness = NOBEYOND(0, (caseLightGetBrightnessPercent() + brightness_delta_percent) * 255 / 100.0f, 255);
}

void caseLightQuerySetWait(bool wait)
{
  lightQueryWait = wait;
}

uint8_t caseLightGetBrightness(void)
{
  return caseLightBrightness;
}

uint8_t caseLightGetBrightnessPercent(void)
{
  return (caseLightBrightness * 100.0f) / 255 + 0.5f;
}

bool caseLightGetState(void)
{
  return caseLightState;
}

void caseLightSendWaiting(bool isWaiting)
{
  caseLight_send_waiting = isWaiting;
}

void loopCaseLight(void)
{
  if (lastCaseLightBrightness != caseLightBrightness || lastCaseLightState != caseLightState)
  {
    lastCaseLightBrightness = caseLightBrightness;
    lastCaseLightState = caseLightState;
    if (caseLight_send_waiting == false)
    {
      caseLight_send_waiting = storeCmd("M355 S%d P%d\n", caseLightState ? 1 : 0, caseLightBrightness);
    }
  }
}
