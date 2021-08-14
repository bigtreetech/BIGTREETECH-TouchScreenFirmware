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
    #define LCD_CS_SET  GPIOC->BSRR=1<<8   // Chip Select Port  PC8   //片选端口
    #define LCD_RS_SET  GPIOD->BSRR=1<<13  // Data / command    PD13  //数据/命令
    #define LCD_WR_SET  GPIOB->BSRR=1<<14  // Write data        PB14  //写数据
    #define LCD_RD_SET  GPIOD->BSRR=1<<15  // Read data         PD15  //读数据

    #define LCD_CS_CLR  GPIOC->BRR=1<<8    // Chip select port  PC8   //片选端口
    #define LCD_RS_CLR  GPIOD->BRR=1<<13   // Data / command    PD13  //数据/命令
    #define LCD_WR_CLR  GPIOB->BRR=1<<14   // Write data        PB14  //写数据
    #define LCD_RD_CLR  GPIOD->BRR=1<<15   // Read data         PD15  //读数据

    #define DATAOUT(x) do{GPIOE->ODR = x;}while(0)  // Data output  //数据输出
    #define DATAIN()     GPIOE->IDR                 // Data input   //数据输入
  #else
    /* TFT24-V1.1
    * PB6   :LCD-RD
    * PB7   :LCD-WR
    * PB9   :LCD-CS
    * PB8   :LCD-RS
    */
    #define LCD_CS_SET  GPIOB->BSRR=1<<9  // Chip Select Port  PB9  //片选端口
    #define LCD_RS_SET  GPIOB->BSRR=1<<8  // Data / command    PB8  //数据/命令
    #define LCD_WR_SET  GPIOB->BSRR=1<<7  // Write data        PB7  //写数据
    #define LCD_RD_SET  GPIOB->BSRR=1<<6  // Read data         PB6  //读数据

    #define LCD_CS_CLR  GPIOB->BRR=1<<9   // Chip Select Port  PB9  //片选端口
    #define LCD_RS_CLR  GPIOB->BRR=1<<8   // Data / command    PB8  //数据/命令
    #define LCD_WR_CLR  GPIOB->BRR=1<<7   // Write data        PB7  //写数据
    #define LCD_RD_CLR  GPIOB->BRR=1<<6   // Read data         PB6  //读数据

    // PB0~15, As a data line  //PB0~15,作为数据线
    #define DATAOUT(x) do{GPIOC->ODR = x;}while(0)  // Data output  //数据输出
    #define DATAIN()     GPIOC->IDR                 // Data input   //数据输入
  #endif
  void LCD_WR_REG(uint16_t data);
  void LCD_WR_DATA(uint16_t data);

#endif

void LCD_HardwareConfig(void);
uint16_t LCD_RD_DATA(void);

#endif
