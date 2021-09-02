#ifndef _LCD_DIMMING_H_
#define _LCD_DIMMING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for LCD_LED_PWM_CHANNEL
#include "menu.h"

#ifdef LCD_LED_PWM_CHANNEL

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

  #define IDLE_TIME_OFF              0  // Off
  #define IDLE_TIME_5                5  // seconds
  #define IDLE_TIME_10              10  // seconds
  #define IDLE_TIME_30              30  // seconds
  #define IDLE_TIME_60              60  // seconds
  #define IDLE_TIME_120            120  // seconds
  #define IDLE_TIME_300            300  // seconds
  //#define IDLE_TIME_CUSTOM IDLE_TIME_5  // custom value predefined in Configuration.h

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
    LCD_IDLE_TIME_CUSTOM,
    LCD_IDLE_TIME_COUNT
  } LCD_IDLE_TIME_;

  extern const uint32_t lcd_brightness[LCD_BRIGHTNESS_COUNT];

  extern const uint32_t lcd_idle_times[LCD_IDLE_TIME_COUNT];
  extern const LABEL lcd_idle_time_names[LCD_IDLE_TIME_COUNT];

  bool LCD_IsBlocked(void);
  void LCD_Wake(void);
  void LCD_CheckDimming(void);

  #define LCD_WAKE() LCD_Wake()

#else

  #define LCD_WAKE()

#endif  // LCD_LED_PWM_CHANNEL

#ifdef __cplusplus
}
#endif

#endif
