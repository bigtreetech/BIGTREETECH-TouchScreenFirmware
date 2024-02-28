#ifndef _LCD_DIMMING_H_
#define _LCD_DIMMING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for LCD_LED_PWM_CHANNEL, KNOB_LED_COLOR_PIN etc...
#include "menu.h"

#ifdef LCD_LED_PWM_CHANNEL

  typedef enum
  {
    LCD_BRIGHTNESS_0 = 0,
    LCD_BRIGHTNESS_5,
    LCD_BRIGHTNESS_10,
    LCD_BRIGHTNESS_20,
    LCD_BRIGHTNESS_30,
    LCD_BRIGHTNESS_40,
    LCD_BRIGHTNESS_50,
    LCD_BRIGHTNESS_60,
    LCD_BRIGHTNESS_70,
    LCD_BRIGHTNESS_80,
    LCD_BRIGHTNESS_90,
    LCD_BRIGHTNESS_100,
    LCD_BRIGHTNESS_COUNT
  } LCD_BRIGHTNESS_;

  typedef enum
  {
    LCD_IDLE_TIME_OFF = 0,
    LCD_IDLE_TIME_5,
    LCD_IDLE_TIME_10,
    LCD_IDLE_TIME_30,
    LCD_IDLE_TIME_60,
    LCD_IDLE_TIME_120,
    LCD_IDLE_TIME_300,
    LCD_IDLE_TIME_CUSTOM,  // the related custom value is IDLE_TIME_CUSTOM predefined in Configuration.h
    LCD_IDLE_TIME_COUNT
  } LCD_IDLE_TIME_;

  extern const uint8_t lcd_brightness[LCD_BRIGHTNESS_COUNT];

  extern const uint16_t lcd_idle_times[LCD_IDLE_TIME_COUNT];
  extern const LABEL lcd_idle_time_names[LCD_IDLE_TIME_COUNT];

  bool LCD_IsBlocked(void);
  void LCD_Wake(void);
  void LCD_CheckDimming(void);

  #define LCD_WAKE() LCD_Wake()

#else

  #define LCD_WAKE()

#endif  // LCD_LED_PWM_CHANNEL

#if defined(LCD_LED_PWM_CHANNEL) && defined(KNOB_LED_COLOR_PIN)
  void LCD_SetKnobLedIdle(bool enabled);

  #define LCD_SET_KNOB_LED_IDLE(enabled) LCD_SetKnobLedIdle(enabled)
#else
  #define LCD_SET_KNOB_LED_IDLE(enabled)
#endif  // LCD_LED_PWM_CHANNEL && KNOB_LED_COLOR_PIN

#ifdef __cplusplus
}
#endif

#endif
