#ifndef _LCD_INIT_H_
#define _LCD_INIT_H_


#include "variants.h"
#include "menu.h"

#ifdef LCD_LED_PWM_CHANNEL
  #define LCD_5_PERCENT    5
  #define LCD_10_PERCENT   10
  #define LCD_20_PERCENT   20
  #define LCD_30_PERCENT   30
  #define LCD_40_PERCENT   40
  #define LCD_50_PERCENT   50
  #define LCD_60_PERCENT   60
  #define LCD_70_PERCENT   70
  #define LCD_80_PERCENT   80
  #define LCD_90_PERCENT   90
  #define LCD_100_PERCENT  100

  #define LCD_DIM_OFF         0    // Off
  #define LCD_DIM_5_SECONDS   5    // Seconds
  #define LCD_DIM_10_SECONDS  10   // Seconds
  #define LCD_DIM_30_SECONDS  30   // Seconds
  #define LCD_DIM_60_SECONDS  60   // Seconds
  #define LCD_DIM_120_SECONDS 120  // Seconds
  #define LCD_DIM_300_SECONDS 300  // Seconds
  /*
   // Custom value, will be predefined in configuration.h
   #define LCD_DIM_CUSTOM_SECONDS LCD_DIM_5_SECONDS
  */

  typedef struct
  {
    uint32_t idle_time_counter;
    bool idle_timer_reset;
    bool _last_dim_state;
  } LCD_AUTO_DIM;
  extern LCD_AUTO_DIM lcd_dim;

  #define ITEM_SECONDS_NUM 8
  #define ITEM_BRIGHTNESS_NUM 11

  extern const uint32_t LCD_DIM_IDLE_TIME[ITEM_SECONDS_NUM];
  extern const LABEL itemDimTime[ITEM_SECONDS_NUM];

  extern const  uint32_t LCD_BRIGHTNESS[ITEM_BRIGHTNESS_NUM];
  extern const LABEL itemBrightness[ITEM_BRIGHTNESS_NUM];

  void LCD_Dim_Idle_Timer_init(void);
  void LCD_Dim_Idle_Timer_Reset(void);
  void LCD_Dim_Idle_Timer(void);
  void LCD_LED_PWM_Init(void);

  #define Set_LCD_Brightness(percentage) TIM_PWM_SetDutyCycle(LCD_LED_PWM_CHANNEL, percentage)
#endif // LCD_LED_PWM_CHANNEL

#if LCD_DATA_16BIT == 1
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#else
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)
#endif

void LCD_RefreshDirection(void);
void LCD_Init(void);
#endif
