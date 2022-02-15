#include "LED_Colors.h"
#include "includes.h"

#ifdef KNOB_LED_COLOR_PIN

const uint32_t knob_led_colors[KNOB_LED_COLOR_COUNT] = {
  GRB_OFF,
  GRB_WHITE,
  GRB_RED,
  GRB_ORANGE,
  GRB_YELLOW,
  GRB_GREEN,
  GRB_BLUE,
  GRB_INDIGO,
  GRB_VIOLET,
};

const LABEL knob_led_color_names[KNOB_LED_COLOR_COUNT] = {
  // label
  LABEL_OFF,
  LABEL_WHITE,
  LABEL_RED,
  LABEL_ORANGE,
  LABEL_YELLOW,
  LABEL_GREEN,
  LABEL_BLUE,
  LABEL_INDIGO,
  LABEL_VIOLET,
};

#endif  // KNOB_LED_COLOR_PIN

const LED_COLOR ledRed =   {0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF};
const LED_COLOR ledGreen = {0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF};
const LED_COLOR ledBlue =  {0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF};
const LED_COLOR ledWhite = {0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF};
const LED_COLOR ledOff =   {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

LED_COLOR ledColor = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};

void LED_SendColor(const LED_COLOR * led)
{
  if (infoMachineSettings.firmwareType != FW_REPRAPFW)
    storeCmd("M150 R%u U%u B%u W%u P%u I%u\n", (*led)[0], (*led)[1], (*led)[2], (*led)[3], (*led)[4], (*led)[5]);
  else
    storeCmd("M150 X2 R%u U%u B%u P%u\n", (*led)[0], (*led)[1], (*led)[2], (*led)[4]);
}

uint16_t LED_GetRGBColor(LED_COLOR * led)
{
  uint8_t r = (*led)[0] >> 3;
  uint8_t g = (*led)[1] >> 2;
  uint8_t b = (*led)[2] >> 3;
  return ((r & 0x001F) << 11) | ((g & 0x003F) << 5) | ((b & 0x001F));  // RGB color in RGB 565 16 bit format
}

void LED_SetRGBColor(LED_COLOR * led, uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t p, uint8_t i)
{
  (*led)[0] = r;
  (*led)[1] = g;
  (*led)[2] = b;
  (*led)[3] = w;
  (*led)[4] = p;
  (*led)[5] = i;
}

void LED_GetColor(LED_COLOR * led)
{
  for (int i = 0; i < LED_COLOR_COMPONENT_COUNT; i++)
    (*led)[i] = ledColor[i];
}

void LED_SetColor(const LED_COLOR * led, bool skipNeopixel)
{
  int size = skipNeopixel ? LED_COLOR_COMPONENT_COUNT - 3 : LED_COLOR_COMPONENT_COUNT;

  for (int i = 0; i < size; i++)
    ledColor[i] = (*led)[i];
}

void LED_SetEventColor(const LED_COLOR * led, bool skipPrinterLed)
{
  if (!skipPrinterLed)
    LED_SendColor(led);

  #ifdef KNOB_LED_COLOR_PIN
    uint32_t knobLedColor = 0;  // GRB format

    knobLedColor |= (uint32_t)((*led)[1]) << 16;  // G
    knobLedColor |= (uint32_t)((*led)[0]) << 8;   // R
    knobLedColor |= (uint32_t)((*led)[2]);        // B

    KNOB_LED_SET_COLOR(knobLedColor, infoSettings.neopixel_pixels);
  #endif
}

void LED_SetPostEventColor(void)
{
  if (infoSettings.led_always_on == 1)
    LED_SetEventColor(&ledColor, false);  // set (neopixel) LED light to current color
  else
    LED_SetEventColor(&ledOff, false);    // set (neopixel) LED light to OFF

  LCD_SET_KNOB_LED_IDLE(true);            // restore infoSettings.knob_led_idle and knob LED color to their default values
}
