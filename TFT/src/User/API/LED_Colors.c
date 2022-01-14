#include "LED_Colors.h"
#include "includes.h"

#ifdef LED_COLOR_PIN

const uint32_t led_colors[LED_COLOR_COUNT] = {
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

const LABEL led_color_names[LED_COLOR_COUNT] = {
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

#endif  // LED_COLOR_PIN

void LED_SetColor(uint8_t r, uint8_t g, uint8_t b, bool skipPrinterLed)
{
  if (!skipPrinterLed)
  {
    if (infoMachineSettings.firmwareType != FW_REPRAPFW)
      storeCmd("M150 R%i U%i B%i P255\n", r, g, b);
    else
      storeCmd("M150 X2 R%i U%i B%i P255\n", r, g, b);
  }

  #ifdef LED_COLOR_PIN
    uint32_t knobLedColor = 0;  // GRB format

    knobLedColor |= (uint32_t)(g) << 16;
    knobLedColor |= (uint32_t)(r) << 8;
    knobLedColor |= (uint32_t)(b);

    KNOB_LED_SET_COLOR(knobLedColor, infoSettings.neopixel_pixels);
  #endif
}
