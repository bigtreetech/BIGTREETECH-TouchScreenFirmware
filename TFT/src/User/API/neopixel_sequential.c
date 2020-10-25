#include "neopixel_sequential.h"
#include "includes.h"

uint8_t prevPixelColor = 0;
uint8_t waitForNext = 0;

void setNeopixelSequentialColor(void)
{
  if(waitForNext > 0)
  {
    waitForNext--;
    return;
  }

  if(!isPrinting())
  {
    if(prevPixelColor == 0)
      return;
    if(printingFinished && prevPixelColor != 0)
    {
      //Set neopixel and ledknob to green
      storeCmd("M150 R0 G255 B0");
      #ifdef LED_COLOR_PIN
        WS2812_Send_DAT(led_color[GREEN]);
      #endif
    }
    else if(prevPixelColor != 0 && infoMenu.menu[infoMenu.cur] != menuPrinting)
    {
      //Restore colors to default value
      prevPixelColor = 0;

      //Turn of neopixels and set knob led back to default
      storeCmd("M150 R0 G0 B0");
      #ifdef LED_COLOR_PIN
        WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
      #endif
    }

    return;
  }
  else
  {
    uint16_t hotendTargetTemp = heatGetTargetTemp(0);
    uint16_t bedTargetTemp = heatGetTargetTemp(BED);

    if(hotendTargetTemp == 0 && bedTargetTemp == 0)
      return; //No temperature set "yet". Do nothing.

    long newLedValue = 0;
    if(hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        heatGetCurrentTemp(BED) >=  bedTargetTemp - 5)
    {
      //Only use total temperature when hotend and bed heat up at the same time
      uint16_t totalTemperature = hotendTargetTemp + bedTargetTemp;
      newLedValue = map(heatGetCurrentTemp(0) + heatGetCurrentTemp(BED), 0, totalTemperature, 0, 255);
    }
    else
    {
      if(hotendTargetTemp == 0)
        newLedValue = map(heatGetCurrentTemp(BED), 0, bedTargetTemp, 0, 125);
      else
        newLedValue = map(heatGetCurrentTemp(0), 0, bedTargetTemp, 125, 255);
    }
    if(newLedValue > prevPixelColor)
    {
      // Set the neopixel color
      storeCmd("M150 R%i G0 B%i", newLedValue, 255 - newLedValue);

      #ifdef LED_COLOR_PIN
        uint8_t colorRed = newLedValue;
        uint8_t colorBlue = 255 - newLedValue;
        uint32_t newPixelColor;
        sprintf(newPixelColor, "0x0000%X%X", colorRed, colorBlue);
        //Color the Knob led when available
        WS2812_Send_DAT(newPixelColor);
      #endif

      prevPixelColor = newLedValue;
    }

    if(hotendTargetTemp > 0 && bedTargetTemp > 0 &&
        heatGetCurrentTemp(0) >=  hotendTargetTemp -5 &&
        heatGetCurrentTemp(BED) >= bedTargetTemp - 5)
    {
      //Bed and hotend are on temperature. Set neopixel to white
      storeCmd("M150 R255 G255 B255");

      #ifdef LED_COLOR_PIN
        //Restore knob led to default
        WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
      #endif
    }
  }
  //Set the waitForNext to 100 to prevent that this function is
  waitForNext = 100;
}
