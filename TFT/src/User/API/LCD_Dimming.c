#include "LCD_Dimming.h"
#include "includes.h"
#include "GPIO_Init.h"

#ifdef LCD_LED_PWM_CHANNEL

  const uint32_t lcd_brightness[LCD_BRIGHTNESS_COUNT] = {
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

  const uint32_t lcd_idle_times[LCD_IDLE_TIME_COUNT] = {
    IDLE_TIME_OFF,
    IDLE_TIME_5,
    IDLE_TIME_10,
    IDLE_TIME_30,
    IDLE_TIME_60,
    IDLE_TIME_120,
    IDLE_TIME_300,
    IDLE_TIME_CUSTOM
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
    uint32_t idle_ms;
    bool dimmed;
    bool blocked;
  } LCD_AUTO_DIM;

  LCD_AUTO_DIM lcd_dim = {0, false, false};

  bool LCD_IsBlocked(void)
  {
    if (!lcd_dim.blocked)
      return false;

    lcd_dim.blocked = false;

    return true;
  }

  void LCD_HandleDimming(void)
  {
    if (infoSettings.lcd_idle_timer == IDLE_TIME_OFF)
      return;

    if (isPress()
        #if LCD_ENCODER_SUPPORT
          || encoder_CheckState() || encoder_ReadBtn(LCD_BUTTON_INTERVALS)
        #endif
       )
    {
      if (lcd_dim.dimmed)
      {
        if (infoSettings.block_touch_on_idle && isPress())  // if touch is blocked on idle and pressing on the LCD (not on the encoder),
          lcd_dim.blocked = true;                           // the first touch will be skipped preventing to trigger any undesired action

        lcd_dim.dimmed = false;
        LCD_SetBrightness(lcd_brightness[infoSettings.lcd_brightness]);

        #ifdef LED_COLOR_PIN
          if (infoSettings.knob_led_idle)
          {
            Knob_LED_SetColor(led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
          }
        #endif
      }

      lcd_dim.idle_ms = OS_GetTimeMs();
    }
    else
    {
      if (OS_GetTimeMs() - lcd_dim.idle_ms < (lcd_idle_times[infoSettings.lcd_idle_timer] * 1000))
        return;

      if (!lcd_dim.dimmed)
      {
        lcd_dim.blocked = false;
        lcd_dim.dimmed = true;
        LCD_SetBrightness(lcd_brightness[infoSettings.lcd_idle_brightness]);

        #ifdef LED_COLOR_PIN
          if (infoSettings.knob_led_idle)
          {
            Knob_LED_SetColor(led_colors[LED_OFF], infoSettings.neopixel_pixels);
          }
        #endif
      }
    }
  }

  void _LCD_Wake(void)
  {
    if (infoSettings.lcd_idle_timer != IDLE_TIME_OFF)
    {
      // The LCD dim function is activated. First check if it's dimmed
      if (lcd_dim.dimmed)
      {
        lcd_dim.blocked = false;
        lcd_dim.dimmed = false;
        LCD_SetBrightness(lcd_brightness[infoSettings.lcd_brightness]);

        #ifdef LED_COLOR_PIN
          if (infoSettings.knob_led_idle)
          {
            Knob_LED_SetColor(led_colors[infoSettings.knob_led_color], infoSettings.neopixel_pixels);
          }
        #endif
      }

      // Set a new idle_ms time
      lcd_dim.idle_ms = OS_GetTimeMs();
    }
  }

#endif  // LCD_LED_PWM_CHANNEL
