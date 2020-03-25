#ifndef _LCD_INIT_H_
#define _LCD_INIT_H_


#include "variants.h"
#include "menu.h"

#ifdef LCD_LED_PIN
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

  #define ITEM_BRIGHTNESS_NUM 11
  extern const  uint32_t LCD_BRIGHTNESS[ITEM_BRIGHTNESS_NUM];
  extern const LABEL itemBrightness[ITEM_BRIGHTNESS_NUM];
  void LCD_LED_PWM_Init();
  #if defined(TFT35_V1_2) || defined(TFT35_V2_0) || defined(TFT35_V3_0)
  #define Set_LCD_Brightness(level) TIM4->CCR1= (uint32_t)(level * (F_CPUM/100));
  #else
    #define Set_LCD_Brightness(level) ;
  #endif
#endif
//TFT35 V1.0 V1.1 RM68042 8bit
//TFT35 V1.2 ili9488 16bit
//TFT28 TFT24 ili9341 16bit
#if LCD_DATA_16BIT == 1
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#else
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)
#endif

void LCD_RefreshDirection(void);
void LCD_Init(void);
#endif
