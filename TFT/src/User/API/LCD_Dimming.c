#include "LCD_Dimming.h"
#include "includes.h"
#include "GPIO_Init.h"

#ifdef LCD_LED_PWM_CHANNEL

// in percentage 0-100
#define BRIGHTNESS_0     0
#define BRIGHTNESS_5     5
#define BRIGHTNESS_10   10
#define BRIGHTNESS_20   20
#define BRIGHTNESS_30   30
#define BRIGHTNESS_40   40
#define BRIGHTNESS_50   50
#define BRIGHTNESS_60   60
#define BRIGHTNESS_70   70
#define BRIGHTNESS_80   80
#define BRIGHTNESS_90   90
#define BRIGHTNESS_100 100

// in seconds
#define IDLE_TIME_OFF   0  // off
#define IDLE_TIME_5     5
#define IDLE_TIME_10   10
#define IDLE_TIME_30   30
#define IDLE_TIME_60   60
#define IDLE_TIME_120 120
#define IDLE_TIME_300 300

const uint8_t lcd_brightness[LCD_BRIGHTNESS_COUNT] = {
  BRIGHTNESS_0,
  BRIGHTNESS_5,
  BRIGHTNESS_10,
  BRIGHTNESS_20,
  BRIGHTNESS_30,
  BRIGHTNESS_40,
  BRIGHTNESS_50,
  BRIGHTNESS_60,
  BRIGHTNESS_70,
  BRIGHTNESS_80,
  BRIGHTNESS_90,
  BRIGHTNESS_100
};

const uint16_t lcd_idle_times[LCD_IDLE_TIME_COUNT] = {
  IDLE_TIME_OFF,
  IDLE_TIME_5,
  IDLE_TIME_10,
  IDLE_TIME_30,
  IDLE_TIME_60,
  IDLE_TIME_120,
  IDLE_TIME_300,
  IDLE_TIME_CUSTOM  // custom value predefined in Configuration.h
};

const LABEL lcd_idle_time_names[LCD_IDLE_TIME_COUNT] = {
  LABEL_OFF,
  LABEL_5_SECONDS,
  LABEL_10_SECONDS,
  LABEL_30_SECONDS,
  LABEL_60_SECONDS,
  LABEL_120_SECONDS,
  LABEL_300_SECONDS,
  LABEL_CUSTOM
};

typedef struct
{
  uint32_t next_idle_time;
  bool dimmed;
  bool locked;
} LCD_AUTO_DIM;

LCD_AUTO_DIM lcd_dim = {0, false, false};

bool LCD_IsBlocked(void)
{
  if (!lcd_dim.locked)
    return false;

  lcd_dim.locked = false;

  return true;
}

void LCD_Wake(void)
{
  if (infoSettings.lcd_idle_time != IDLE_TIME_OFF)  // if LCD dim function is activated
  {
    if (lcd_dim.dimmed)  // if LCD is dimmed
    {
      lcd_dim.locked = false;
      lcd_dim.dimmed = false;
      LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_brightness]);

      #ifdef KNOB_LED_COLOR_PIN
        if (infoSettings.knob_led_idle)
        {
          Knob_LED_SetColor(knob_led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
        }
      #endif
    }

    // always set next idle time
    lcd_dim.next_idle_time = OS_GetTimeMs() + SEC_TO_MS(lcd_idle_times[infoSettings.lcd_idle_time]);
  }
}

void LCD_CheckDimming(void)
{
  if (infoSettings.lcd_idle_time == IDLE_TIME_OFF)
    return;

  if (!TS_IsPressed()
    #if LCD_ENCODER_SUPPORT
      && !LCD_Enc_CheckState()
    #endif
    )  // if touch screen not pressed and rotary encoder neither pressed nor rotated
  {
    if (lcd_dim.dimmed || OS_GetTimeMs() < lcd_dim.next_idle_time)
      return;

    lcd_dim.locked = false;
    lcd_dim.dimmed = true;
    LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_idle_brightness]);

    #ifdef KNOB_LED_COLOR_PIN
      if (infoSettings.knob_led_idle)
      {
        Knob_LED_SetColor(knob_led_colors[KNOB_LED_OFF], infoSettings.neopixel_pixels);
      }
    #endif

    return;
  }

  // touch screen pressed and/or rotary encoder pressed or rotated

  // always set next idle time
  lcd_dim.next_idle_time = OS_GetTimeMs() + SEC_TO_MS(lcd_idle_times[infoSettings.lcd_idle_time]);

  if (!lcd_dim.dimmed)
    return;

  // if touch is blocked on idle and pressing on the LCD (not on the rotary encoder),
  // the first touch will be skipped preventing to trigger any undesired action
  if (infoSettings.lcd_lock_on_idle && TS_IsPressed())
    lcd_dim.locked = true;

  lcd_dim.dimmed = false;
  LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_brightness]);

  #ifdef KNOB_LED_COLOR_PIN
    if (infoSettings.knob_led_idle)
    {
      Knob_LED_SetColor(knob_led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
    }
  #endif
}

#ifdef KNOB_LED_COLOR_PIN

bool knob_led_idle = false;

void LCD_SetKnobLedIdle(bool enabled)
{
  if (!enabled)
  { // set infoSettings.knob_led_idle temporary to OFF
    if (infoSettings.knob_led_idle && !knob_led_idle)
    {
      knob_led_idle = true;
      infoSettings.knob_led_idle = 0;  // turn knob_led_idle off
    }
  }
  else
  { // make sure that infoSettings.knob_led_idle is back in business
    if (knob_led_idle)
    {
      knob_led_idle = false;
      infoSettings.knob_led_idle = 1;  // turn knob_led_idle on
    }

    // always restore default knob LED color
    Knob_LED_SetColor(knob_led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
  }
}

#endif  // KNOB_LED_COLOR_PIN

#endif  // LCD_LED_PWM_CHANNEL
