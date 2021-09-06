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
