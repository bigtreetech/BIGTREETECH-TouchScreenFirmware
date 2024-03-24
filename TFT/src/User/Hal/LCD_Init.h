#ifndef _LCD_INIT_H_
#define _LCD_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>    // for uint8_t etc.
#include "variants.h"  // for LCD_DATA_16BIT, LCD_LED_PWM_CHANNEL etc.

#if LCD_DATA_16BIT == 1
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#else
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)
#endif

#ifdef LCD_LED_PWM_CHANNEL
  #define LCD_SET_BRIGHTNESS(percentage) TIM_PWM_SetDutyCycle(LCD_LED_PWM_CHANNEL, percentage)
#else
  #define LCD_SET_BRIGHTNESS(percentage)
#endif

#ifdef SCREEN_SHOT_TO_SD
  uint32_t LCD_ReadPixel_24Bit(int16_t x, int16_t y);
#endif

void LCD_Init(void);
void LCD_RefreshDirection(uint8_t rotate);
void LCD_SetWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);

#ifdef __cplusplus
}
#endif

#endif
