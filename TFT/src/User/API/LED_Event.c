#include "LED_Event.h"
#include "includes.h"

#define LED_REFRESH_TIME 2000  // 1 seconds is 1000

//#define ROOM_TEMPERATURE
#ifdef ROOM_TEMPERATURE
  #define MAX_COLD_TEMP     50
  #define DEFAULT_COLD_TEMP 25

  static uint16_t coldTemperature = 0;

  #define COLD_TEMPERATURE coldTemperature
#else
  #define COLD_TEMPERATURE 0
#endif

inline static bool nextUpdate(void)
{
  static uint32_t lastUpdateTime = 0;

  if (OS_GetTimeMs() - lastUpdateTime < LED_REFRESH_TIME)
    return false;

  lastUpdateTime = OS_GetTimeMs();

  return true;
}

#ifdef ROOM_TEMPERATURE

void getColdTemperature(void)
{ // let's estimate the room temperature
  if (coldTemperature == 0)
  {
    uint16_t hotendCurrentTemp = heatGetCurrentTemp(NOZZLE0);
    uint16_t bedCurrentTemp = heatGetCurrentTemp(BED);
    uint8_t divider = 0;

    if (hotendCurrentTemp < MAX_COLD_TEMP)
    {
      coldTemperature += hotendCurrentTemp;
      divider++;
    }

    if (bedCurrentTemp < MAX_COLD_TEMP)
    {
      coldTemperature += bedCurrentTemp;
      divider++;
    }

    if (coldTemperature == 0)
      coldTemperature = DEFAULT_COLD_TEMP;
    else
      coldTemperature = coldTemperature / divider;
  }
}

#endif  // ROOM_TEMPERATURE

void LED_CheckEvent(void)
{
  static uint8_t prevLedValue = 0;
  static bool heatingDone = false;
  static bool printingDone = false;

  #ifdef ROOM_TEMPERATURE
    getColdTemperature();
  #endif

  if (!isPrinting())
  {
    if (prevLedValue == 0)  // if no color was set yet, nothing to do
      return;

    if (!(MENU_IS(menuPrinting)))
    {
      prevLedValue = 0;      // reset color to default value
      heatingDone = false;   // reset flag to "false"
      printingDone = false;  // reset flag to "false"

      // set (neopixel) LED light to current color or to OFF according to infoSettings.led_always_on and
      // restore infoSettings.knob_led_idle and knob LED color to their default values
      LED_SetPostEventColor();

      return;
    }

    if (printingDone)  // if printing is finished, nothing to do
      return;

    printingDone = true;  // set flag to "true"

    LED_SetEventColor(&ledGreen, false);  // set (neopixel) LED light to GREEN
    LCD_SET_KNOB_LED_IDLE(false);         // set infoSettings.knob_led_idle temporary to OFF
  }
  else
  {
    if (!isPrintingFromTFT() || heatingDone)  // if not printng from TFT media or if heating is finished, nothing to do
      return;

    if (!nextUpdate())  // if not time for a new update, nothing to do
      return;

    // store current target temperature values
    uint16_t hotendTargetTemp = heatGetTargetTemp(NOZZLE0);
    uint16_t bedTargetTemp = heatGetTargetTemp(BED);

    if (hotendTargetTemp == 0 && bedTargetTemp == 0)  // if no temperature is set yet, nothing to do
      return;

    // if heating is controlled by printer, we cannot control printer's (neopixel) LED but only TFT's knob LED, if any
    bool skipPrinterLed = (GET_BIT(infoSettings.general_settings, INDEX_EMULATED_M109_M190) == 0) ? true : false;

    // store current temperature values
    uint16_t hotendCurrentTemp = heatGetCurrentTemp(NOZZLE0);
    uint16_t bedCurrentTemp = heatGetCurrentTemp(BED);

    // check if both bed and hotend reached their target temperatures
    if (hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        hotendCurrentTemp >= hotendTargetTemp - TEMPERATURE_RANGE &&
        bedCurrentTemp >= bedTargetTemp - TEMPERATURE_RANGE)
    {
      prevLedValue = 255;  // set with a value just in case heating is immediately reached (e.g. nozzle and bed were already heated)
      heatingDone = true;  // set flag to "true"

      LED_SetEventColor(&ledColor, skipPrinterLed);  // set (neopixel) LED light to current color
      LCD_SET_KNOB_LED_IDLE(true);                   // restore infoSettings.knob_led_idle and knob LED color to their default values

      return;
    }

    long newLedValue = 0;

    // only use total temperature when hotend and bed heat up at the same time
    if (hotendTargetTemp > 0 && bedTargetTemp > 0 && bedCurrentTemp >= bedTargetTemp - TEMPERATURE_RANGE)
      newLedValue = map(hotendCurrentTemp + bedCurrentTemp, COLD_TEMPERATURE, hotendTargetTemp + bedTargetTemp, 0, 255);
    else if (hotendTargetTemp > 0)
      newLedValue = map(hotendCurrentTemp, COLD_TEMPERATURE, hotendTargetTemp, 128, 255);
    else if (bedTargetTemp > 0)
      newLedValue = map(bedCurrentTemp, COLD_TEMPERATURE, bedTargetTemp, 0, 127);

    newLedValue = NOBEYOND(0, newLedValue, 255);  // be sure new value is in range 0-255

    if (!((uint8_t)(newLedValue) > prevLedValue))  // if new value is not bigger than previous one, nothing to do
      return;

    prevLedValue = (uint8_t)(newLedValue);  // save new value as previous one

    LED_COLOR led;

    LED_SetRGBColor(&led, newLedValue, 0, 255 - newLedValue, 0, 255, 255);  // set (neopixel) LED light custom color
    LED_SetEventColor(&led, skipPrinterLed);                                // set (neopixel) LED light to custom color
    LCD_SET_KNOB_LED_IDLE(false);                                           // set infoSettings.knob_led_idle temporary to OFF
  }
}
