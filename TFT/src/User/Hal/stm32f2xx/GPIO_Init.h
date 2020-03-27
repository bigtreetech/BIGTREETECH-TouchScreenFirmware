#ifndef _GPIO_INIT_H_
#define _GPIO_INIT_H_

#include "stm32f2xx_conf.h"
#include "STM32Fxx_Pins.h"
/*
* bit 0:1 gpio mode  00: Input (reset state)  01: General purpose output mode  10: Alternate function mode  11: Analog mode
* bit 2   gpio output type  0: Output push-pull (reset state) 1: Output open-drain
* bit 3:4 gpio output speed  00: Low speed 01: Medium speed 10: High speed 11: Very high speed
* bit 5:6 gpio pull-up/pull-down  00: No pull-up, pull-down 01: Pull-up 10: Pull-down 11: Reserved
* bit 7   reserved
*/
typedef enum
{
  MGPIO_MODE_IPN = (0<<5)|(3<<3)|(0<<2)|(0),
  MGPIO_MODE_IPU = (1<<5)|(3<<3)|(0<<2)|(0),
  MGPIO_MODE_IPD = (2<<5)|(3<<3)|(0<<2)|(0),
  MGPIO_MODE_OUT_PP = (0<<5)|(3<<3)|(0<<2)|(1),
  MGPIO_MODE_OUT_OD = (0<<5)|(3<<3)|(1<<2)|(1),
  MGPIO_MODE_AF_PP = (0<<5)|(3<<3)|(0<<2)|(2),
  MGPIO_MODE_AF_OD = (0<<5)|(3<<3)|(1<<2)|(2),
  MGPIO_MODE_AIN = (0<<5)|(3<<3)|(0<<2)|(3),
}GPIO_MODE;

#define GPIO_MODE_GET_MODE(n) (n & 0x3)
#define GPIO_MODE_GET_OTYPE(n) ((n>>2) & 0x1)
#define GPIO_MODE_GET_OSPEED(n) ((n>>3) & 0x3)
#define GPIO_MODE_GET_PULL(n) ((n>>5) & 0x3)

#define GPIO_MODE_AF 2
#define GPIO_AF0 0
#define GPIO_AF1 1
#define GPIO_AF2 2
#define GPIO_AF3 3
#define GPIO_AF4 4
#define GPIO_AF5 5
#define GPIO_AF6 6
#define GPIO_AF7 7
#define GPIO_AF8 8
#define GPIO_AF9 9
#define GPIO_AF10 10
#define GPIO_AF11 11
#define GPIO_AF12 12
#define GPIO_AF13 13
#define GPIO_AF14 14
#define GPIO_AF15 15

#ifndef GPIO_AF_RTC_50Hz
  /**
    * @brief   AF 0 selection
    */
  #define GPIO_AF_RTC_50Hz      ((uint8_t)0x00)  /* RTC_50Hz Alternate Function mapping */
  #define GPIO_AF_MCO           ((uint8_t)0x00)  /* MCO (MCO1 and MCO2) Alternate Function mapping */
  #define GPIO_AF_TAMPER        ((uint8_t)0x00)  /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
  #define GPIO_AF_SWJ           ((uint8_t)0x00)  /* SWJ (SWD and JTAG) Alternate Function mapping */
  #define GPIO_AF_TRACE         ((uint8_t)0x00)  /* TRACE Alternate Function mapping */

  /**
    * @brief   AF 1 selection
    */
  #define GPIO_AF_TIM1          ((uint8_t)0x01)  /* TIM1 Alternate Function mapping */
  #define GPIO_AF_TIM2          ((uint8_t)0x01)  /* TIM2 Alternate Function mapping */

  /**
    * @brief   AF 2 selection
    */
  #define GPIO_AF_TIM3          ((uint8_t)0x02)  /* TIM3 Alternate Function mapping */
  #define GPIO_AF_TIM4          ((uint8_t)0x02)  /* TIM4 Alternate Function mapping */
  #define GPIO_AF_TIM5          ((uint8_t)0x02)  /* TIM5 Alternate Function mapping */

  /**
    * @brief   AF 3 selection
    */
  #define GPIO_AF_TIM8          ((uint8_t)0x03)  /* TIM8 Alternate Function mapping */
  #define GPIO_AF_TIM9          ((uint8_t)0x03)  /* TIM9 Alternate Function mapping */
  #define GPIO_AF_TIM10         ((uint8_t)0x03)  /* TIM10 Alternate Function mapping */
  #define GPIO_AF_TIM11         ((uint8_t)0x03)  /* TIM11 Alternate Function mapping */

  /**
    * @brief   AF 4 selection
    */
  #define GPIO_AF_I2C1          ((uint8_t)0x04)  /* I2C1 Alternate Function mapping */
  #define GPIO_AF_I2C2          ((uint8_t)0x04)  /* I2C2 Alternate Function mapping */
  #define GPIO_AF_I2C3          ((uint8_t)0x04)  /* I2C3 Alternate Function mapping */

  /**
    * @brief   AF 5 selection
    */
  #define GPIO_AF_SPI1          ((uint8_t)0x05)  /* SPI1 Alternate Function mapping */
  #define GPIO_AF_SPI2          ((uint8_t)0x05)  /* SPI2/I2S2 Alternate Function mapping */

  /**
    * @brief   AF 6 selection
    */
  #define GPIO_AF_SPI3          ((uint8_t)0x06)  /* SPI3/I2S3 Alternate Function mapping */

  /**
    * @brief   AF 7 selection
    */
  #define GPIO_AF_USART1        ((uint8_t)0x07)  /* USART1 Alternate Function mapping */
  #define GPIO_AF_USART2        ((uint8_t)0x07)  /* USART2 Alternate Function mapping */
  #define GPIO_AF_USART3        ((uint8_t)0x07)  /* USART3 Alternate Function mapping */

  /**
    * @brief   AF 8 selection
    */
  #define GPIO_AF_UART4         ((uint8_t)0x08)  /* UART4 Alternate Function mapping */
  #define GPIO_AF_UART5         ((uint8_t)0x08)  /* UART5 Alternate Function mapping */
  #define GPIO_AF_USART6        ((uint8_t)0x08)  /* USART6 Alternate Function mapping */

  /**
    * @brief   AF 9 selection
    */
  #define GPIO_AF_CAN1          ((uint8_t)0x09)  /* CAN1 Alternate Function mapping */
  #define GPIO_AF_CAN2          ((uint8_t)0x09)  /* CAN2 Alternate Function mapping */
  #define GPIO_AF_TIM12         ((uint8_t)0x09)  /* TIM12 Alternate Function mapping */
  #define GPIO_AF_TIM13         ((uint8_t)0x09)  /* TIM13 Alternate Function mapping */
  #define GPIO_AF_TIM14         ((uint8_t)0x09)  /* TIM14 Alternate Function mapping */

  /**
    * @brief   AF 10 selection
    */
  #define GPIO_AF_OTG_FS         ((uint8_t)0xA)  /* OTG_FS Alternate Function mapping */
  #define GPIO_AF_OTG_HS         ((uint8_t)0xA)  /* OTG_HS Alternate Function mapping */

  /**
    * @brief   AF 11 selection
    */
  #define GPIO_AF_ETH             ((uint8_t)0x0B)  /* ETHERNET Alternate Function mapping */

  /**
    * @brief   AF 12 selection
    */
  #define GPIO_AF_FSMC            ((uint8_t)0xC)  /* FSMC Alternate Function mapping */
  #define GPIO_AF_OTG_HS_FS       ((uint8_t)0xC)  /* OTG HS configured in FS, Alternate Function mapping */
  #define GPIO_AF_SDIO            ((uint8_t)0xC)  /* SDIO Alternate Function mapping */

  /**
    * @brief   AF 13 selection
    */
  #define GPIO_AF_DCMI          ((uint8_t)0x0D)  /* DCMI Alternate Function mapping */

  /**
    * @brief   AF 15 selection
    */
  #define GPIO_AF_EVENTOUT      ((uint8_t)0x0F)  /* EVENTOUT Alternate Function mapping */

#endif

void GPIO_InitSet(uint16_t io, GPIO_MODE mode, uint8_t AF);
void GPIO_SetLevel(uint16_t io, uint8_t level);
void GPIO_ToggleLevel(uint16_t io);
uint8_t GPIO_GetLevel(uint16_t io);

#endif
