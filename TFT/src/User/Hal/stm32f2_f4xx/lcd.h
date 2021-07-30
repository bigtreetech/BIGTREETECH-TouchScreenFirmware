#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include "variants.h"  // for STM32_HAS_FSMC etc...

#ifdef STM32_HAS_FSMC

  typedef struct
  {
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
  } LCD_TypeDef;

  #define LCD_BASE        ((uint32_t)(0x60000000 | 0x00FFFFFE))  // 1111 1111 1111 1111 1111 1110
  #define LCD             ((LCD_TypeDef *) LCD_BASE)

  #define LCD_WR_REG(regval) do{ LCD->LCD_REG = regval; }while(0)
  #define LCD_WR_DATA(data)  do{ LCD->LCD_RAM = data; }while(0)

#else
  #error "don't support LCD-GPIO yet"
#endif

void LCD_HardwareConfig(void);
uint16_t LCD_RD_DATA(void);

#endif
