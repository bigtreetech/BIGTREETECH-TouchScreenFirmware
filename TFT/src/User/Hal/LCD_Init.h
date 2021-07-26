#ifndef _LCD_INIT_H_
#define _LCD_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"
#include "menu.h"

#ifdef LCD_LED_PWM_CHANNEL

  #define ITEM_SECONDS_NUM     8
  #define ITEM_BRIGHTNESS_NUM 12

  extern const uint32_t LCD_DIM_IDLE_TIME[ITEM_SECONDS_NUM];
  extern const LABEL itemDimTime[ITEM_SECONDS_NUM];

  extern const uint32_t LCD_BRIGHTNESS[ITEM_BRIGHTNESS_NUM];
  extern const LABEL itemBrightness[ITEM_BRIGHTNESS_NUM];

  void LCD_LED_PWM_Init(void);
  bool LCD_isBlocked(void);
  void loopDimTimer(void);
  void _wakeLCD(void);

  #define Set_LCD_Brightness(percentage) TIM_PWM_SetDutyCycle(LCD_LED_PWM_CHANNEL, percentage)
  #define wakeLCD() _wakeLCD()

#else

  #define wakeLCD()

#endif  // LCD_LED_PWM_CHANNEL

#if LCD_DATA_16BIT == 1
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#else
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)
#endif

uint32_t LCD_ReadPixel_24Bit(int16_t x, int16_t y);
void LCD_RefreshDirection(void);
void LCD_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
void LCD_Init(void);

#ifdef __cplusplus
}
#endif

#endif
