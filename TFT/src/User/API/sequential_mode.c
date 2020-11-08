#include "sequential_mode.h"
#include "includes.h"

uint8_t prevPixelColor = 0;
uint8_t waitForNext = 0;
bool heatingDone = false;
#ifdef LED_COLOR_PIN
  #ifdef LCD_LED_PWM_CHANNEL
    bool idle_ledmode_previous = false;
  #endif
#endif

void setSequentialModeColor(void) {
  if (waitForNext > 0)
  {
    waitForNext--;
    return;
  }

  if (!isPrinting()) {
    if (prevPixelColor == 0)
    {
      return;
    }
    #ifdef LED_COLOR_PIN
      #ifdef LCD_LED_PWM_CHANNEL
        //Make sure that the knob_led_idle is back in business
        if(idle_ledmode_previous)
        {
          idle_ledmode_previous = false;
          infoSettings.knob_led_idle = 1;
        }
      #endif
    #endif
    //Reset flag heating done
    heatingDone = false;

    if (infoMenu.menu[infoMenu.cur] != menuPrinting) {
      //Restore colors to default value
      prevPixelColor = 0;

      //Turn off neopixels and set knob led back to default
      //storeCmd("M150 R0 U0 B0");
      #ifdef LED_COLOR_PIN
        #ifdef LCD_LED_PWM_CHANNEL
          if(infoSettings.knob_led_idle && lcd_dim.dimmed)
          {
            WS2812_Send_DAT(LED_OFF);
          }
          else
          {
            WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
          }
        #endif
      #endif
      return;
    }

    //Set neopixel and ledknob to green
    //storeCmd("M150 R0 U255 B0");

    #ifdef LED_COLOR_PIN
      WS2812_Send_DAT(LED_GREEN);
    #endif

  }
  else
  {

    if(heatingDone)
    {
      #ifdef LED_COLOR_PIN
        #ifdef LCD_LED_PWM_CHANNEL
          //Set the knob_led_idle on again
          if(idle_ledmode_previous)
          {
            idle_ledmode_previous = false;
            infoSettings.knob_led_idle = 1;
          }
        #endif
      #endif
      return; //Go back when preheating is finished
    }

    //Store current target temperature values to reduce cycle time
    uint16_t hotendTargetTemp = heatGetTargetTemp(0);
    uint16_t bedTargetTemp = heatGetTargetTemp(BED);

    if (hotendTargetTemp == 0 && bedTargetTemp == 0)
    {
      return;  //No temperature set "yet". Do nothing.
    }

    #ifdef LED_COLOR_PIN
      #ifdef LCD_LED_PWM_CHANNEL
        // set the knob_led_idle temperorly to OFF
        if(infoSettings.knob_led_idle && !idle_ledmode_previous)
        {
          idle_ledmode_previous = true;
          infoSettings.knob_led_idle = 0; //Temperory turn off led idle
        }
      #endif
    #endif

    //Store current temperature values to reduce cycle time
    uint16_t hotendCurrentTemp = heatGetCurrentTemp(0);
    uint16_t bedCurrentTemp = heatGetCurrentTemp(BED);

    uint8_t newLedValue = 0;
    if (hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        bedCurrentTemp >= bedTargetTemp - 5)
    {
      //Only use total temperature when hotend and bed heat up at the same time
      uint16_t totalTemperature = hotendTargetTemp + bedTargetTemp;
      newLedValue = map(hotendCurrentTemp + bedCurrentTemp, 0, totalTemperature, 0, 255);
    }
    else
    {
      if (hotendTargetTemp == 0)
      {
        newLedValue = map(bedCurrentTemp, 0, bedTargetTemp, 0, 125);
      }
      else
      {
        newLedValue = map(hotendCurrentTemp, 0, hotendTargetTemp, 125, 255);
      }
    }

    if (newLedValue > prevPixelColor)
    {
      // Set the neopixel color
      //storeCmd("M150 R%i U0 B%i", newLedValue, 255 - newLedValue);

      #ifdef LED_COLOR_PIN
        uint8_t colorRed = newLedValue;
        uint8_t colorBlue = 255 - newLedValue;
        char hexRed, hexBlue;
        sprintf(&hexRed, "%x", colorRed);
        sprintf(&hexBlue, "%x", colorBlue);
        uint32_t newPixelColor = 0;
        newPixelColor |= (uint32_t)(hexRed) << 8;
        newPixelColor |= (uint32_t)(hexBlue);
        //WEG
        char tempstr[60];
        sprintf(tempstr, "NEW %i | R:%x B:%x", newPixelColor, hexRed, hexBlue);
        GUI_DispString(100, 0, (u8 *)tempstr);
        //END WEG
        //Color the Knob led when available
        WS2812_Send_DAT(newPixelColor);
      #endif

      prevPixelColor = newLedValue;
    }

    if (hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        hotendCurrentTemp >= hotendTargetTemp - 5 &&
        bedCurrentTemp >= bedTargetTemp - 5)
    {
      //Bed and hotend are on temperature. Set neopixel to white
      //storeCmd("M150 R255 U255 B255");

      //Restore the enocder to the previous state
      #ifdef LED_COLOR_PIN
        #ifdef LCD_LED_PWM_CHANNEL
          if(infoSettings.knob_led_idle && lcd_dim.dimmed)
          {
            WS2812_Send_DAT(LED_OFF);
          }
          else
          {
            WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
          }
        #endif
      #endif

      //Set the flag heating done to true
      heatingDone = true;
    }
  }
  //Set the waitForNext to a certain number to prevent that this function is
  //This means that after X cycles it will be back in this function
  waitForNext = 100;
}
