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
  #if defined(MKS_TFT)
    /*
    #define LCD_WR PB14
    #define LCD_RS PD13
    #define LCD_CS PC8
    #define LCD_RD PD15
    */
    // GPIO_SetLevel
    #define LCD_CS_SET  gpio_bit_set(GPIOC, GPIO_PIN_8 )       // Chip Select Port  PC8   //片选端口
    #define LCD_RS_SET  gpio_bit_set(GPIOD, GPIO_PIN_13)       // Data / command    PD13  //数据/命令
    #define LCD_WR_SET  gpio_bit_set(GPIOB, GPIO_PIN_14)       // Write data        PB14  //写数据
    #define LCD_RD_SET  gpio_bit_set(GPIOD, GPIO_PIN_15)       // Read data         PD15  //读数据

    #define LCD_CS_CLR  gpio_bit_reset(GPIOC, GPIO_PIN_8)      // Chip select port  PC8   //片选端口
    #define LCD_RS_CLR  gpio_bit_reset(GPIOD, GPIO_PIN_13)     // Data / command    PD13  //数据/命令
    #define LCD_WR_CLR  gpio_bit_reset(GPIOB, GPIO_PIN_14)     // Write data        PB14  //写数据
    #define LCD_RD_CLR  gpio_bit_reset(GPIOD, GPIO_PIN_15)     // Read data         PD15  //读数据

    #define DATAOUT(x) do{gpio_port_write(GPIOE, x);}while(0)  // Data output  //数据输出
    #define DATAIN()   gpio_input_port_get(GPIOE)              // Data input   //数据输入
  #else
    #error "don't support LCD-GPIO yet"
  #endif
  void LCD_WR_REG(uint16_t data);
  void LCD_WR_DATA(uint16_t data);
#endif

void LCD_HardwareConfig(void);
uint16_t LCD_RD_DATA(void);

#endif
