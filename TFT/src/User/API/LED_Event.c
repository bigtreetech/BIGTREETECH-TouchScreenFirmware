#include "LED_Event.h"
#include "includes.h"

#define UPDATE_TIME  2000  // 1 seconds is 1000
#define MAXCOLD_TEMP 50
#define DEFAULT_COLDTEMP 25

uint32_t lastUpdateTime = 0;
uint8_t prevPixelColor = 0;
bool heatingDone = false;
bool finishedPrint = false;
#ifdef LED_COLOR_PIN
  #ifdef LCD_LED_PWM_CHANNEL
    bool idle_ledmode_previous = false;
  #endif
#endif
uint16_t coldTemperature = 0;

inline static bool nextUpdate(void)
{
  uint32_t curTime = OS_GetTimeMs();
  if (curTime > (lastUpdateTime + UPDATE_TIME))
  {
    lastUpdateTime = curTime;
    return true;
  }
  else
  {
    return false;
  }
}

void LED_SetEventColor(void)
{
  if (!nextUpdate())
    return;

  // Let's estimate the room temperature
  if (coldTemperature == 0)
  {
    uint16_t hotendCurrentTemp = heatGetCurrentTemp(NOZZLE0);
    uint16_t bedCurrentTemp    = heatGetCurrentTemp(BED);
    uint8_t divider = 0;
    if (hotendCurrentTemp < MAXCOLD_TEMP)
    {
      coldTemperature = hotendCurrentTemp;
      divider++;
    }
    if (bedCurrentTemp < MAXCOLD_TEMP)
    {
      coldTemperature = bedCurrentTemp;
      divider++;
    }
    if (coldTemperature == 0)
    {
      coldTemperature = DEFAULT_COLDTEMP;
    }
    else
    {
      coldTemperature = coldTemperature / divider;
    }
  }

  if (!isPrinting())
  {
    // Not printing/
    // Check if the previous color has been set.
    // If not, return to reduce cycletime
    if (prevPixelColor == 0) return;

    // There was a previous color.
    // Set every led to the correct color.

    // Reset flag heating done
    heatingDone = false;

    if (infoMenu.menu[infoMenu.cur] != menuPrinting)
    {
      #ifdef LED_COLOR_PIN
        #ifdef LCD_LED_PWM_CHANNEL
          // Make sure that the knob_led_idle is back in business
          if (idle_ledmode_previous)
          {
            idle_ledmode_previous = false;
            infoSettings.knob_led_idle = 1;
          }
        #endif
      #endif

      // Restore colors to default value
      prevPixelColor = 0;

      // Reset print fineshed bool
      finishedPrint = false;

      // Turn off neopixels and set knob led back to default
      storeCmd("M150 R0 U0 B0 P0\n");

      #ifdef LED_COLOR_PIN
        #ifdef LCD_LED_PWM_CHANNEL
          // set the screen to the max brightness
          // The encoder knob will get it's default color.
//          lcd_dim.dimmed = true; // Force dimmed mode
          LCD_WAKE();
        #endif
      #endif
      return;
    }

  
    if (finishedPrint)
    {
      // Check if print is already marked as ready.
      // No need to change the LED's  again
      return;
    }

    #ifdef LED_COLOR_PIN
      #ifdef LCD_LED_PWM_CHANNEL
      // set the knob_led_idle temperorly to OFF
      if (infoSettings.knob_led_idle && !idle_ledmode_previous)
      {
        idle_ledmode_previous = true;
        infoSettings.knob_led_idle = 0; //Temperory turn off led idle
      }
      #endif
    #endif

    // Set neopixel and ledknob to green
    storeCmd("M150 R0 U255 B0 P255\n");

    KNOB_LED_SET_COLOR(LED_GREEN, infoSettings.neopixel_pixels);  // set knob LED light to GREEN

    finishedPrint = true;
    return;
  }
  else
  {

    if (heatingDone)
    {
      return; // Go back when preheating is finished
    }

    // Store current target temperature values to reduce cycle time
    uint16_t hotendTargetTemp = heatGetTargetTemp(0);
    uint16_t bedTargetTemp = heatGetTargetTemp(BED);

    if (hotendTargetTemp == 0 && bedTargetTemp == 0)
    {
      return; //No temperature set "yet". Do nothing.
    }

    #ifdef LED_COLOR_PIN
      #ifdef LCD_LED_PWM_CHANNEL
        // set the knob_led_idle temperorly to OFF
        if (infoSettings.knob_led_idle && !idle_ledmode_previous)
        {
          idle_ledmode_previous = true;
          infoSettings.knob_led_idle = 0; //Temperory turn off led idle
        }
      #endif
    #endif

    // Store current temperature values to reduce cycle time
    uint16_t hotendCurrentTemp = heatGetCurrentTemp(0);
    uint16_t bedCurrentTemp = heatGetCurrentTemp(BED);

    // Check if the temperature already reached it's target temperature
    if (hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        hotendCurrentTemp >= hotendTargetTemp - 5 &&
        bedCurrentTemp >= bedTargetTemp - 5)
    {
      // Bed and hotend are on temperature. Set neopixel to white
      storeCmd("M150 R255 U255 B255 P255\n");

      // Restore the encoder to the previous state
      #ifdef LED_COLOR_PIN
        #ifdef LCD_LED_PWM_CHANNEL
          // Set the knob_led_idle on again
          if (idle_ledmode_previous)
          {
            idle_ledmode_previous = false;
            infoSettings.knob_led_idle = 1;
          }
        #endif
      #endif

      // set the screen to the max brightness
      // The encoder knob will get it's default color.
      LCD_WAKE();

      // Set the flag heating done to true
      heatingDone = true;
      return;
    }

    uint8_t newLedValue = 0;
    if (hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        bedCurrentTemp >= bedTargetTemp - 5)
    {
      // Only use total temperature when hotend and bed heat up at the same time
      uint16_t totalTemperature = hotendTargetTemp + bedTargetTemp;
      newLedValue = map(hotendCurrentTemp + bedCurrentTemp, coldTemperature, totalTemperature, 0, 255);
    }
    else
    {
      if (hotendTargetTemp == 0)
      {
        newLedValue = map(bedCurrentTemp, coldTemperature, bedTargetTemp, 0, 125);
      }
      else
      {
        newLedValue = map(hotendCurrentTemp, coldTemperature, hotendTargetTemp, 125, 255);
      }
    }

    if (!(newLedValue > prevPixelColor))
    {
      // Previous led value is the same as the current one.
      // Rest of the code is not needed to execute.
      return;
    }

    // Set the neopixel color
    storeCmd("M150 R%i U0 B%i P255\n", newLedValue, 255 - newLedValue);

    #ifdef LED_COLOR_PIN
      uint32_t newPixelColor = 0;
      newPixelColor |= (uint32_t)(newLedValue) << 8;
      newPixelColor |= (uint32_t)(255 - newLedValue);

      //WEG
      //char tempstr[60];
      //sprintf(tempstr, "NEW %i | R:%i B:%i | LV: %i", newPixelColor, colorRed, colorBlue, newLedValue);
      //GUI_DispString(100, 0, (u8 *)tempstr);
      //END WEG

      // Color the Knob led when available
      KNOB_LED_SET_COLOR(newPixelColor, infoSettings.neopixel_pixels);
    #endif

    prevPixelColor = newLedValue;
    return;
  }
}
