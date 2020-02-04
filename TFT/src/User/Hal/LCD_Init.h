#ifndef _LCD_INIT_H_
#define _LCD_INIT_H_


#include "variants.h"

//TFT35 V1.0 V1.1 RM68042 8bit
//TFT35 V1.2 ili9488 16bit
//TFT28 TFT24 ili9341 16bit
#if LCD_DATA_16BIT == 1
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#else
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)
#endif

void LCD_RefreshDirection(void);
void LCD_Init(void);          //

#endif
