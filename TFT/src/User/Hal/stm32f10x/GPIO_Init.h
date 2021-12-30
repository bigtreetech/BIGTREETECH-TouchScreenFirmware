#ifndef _GPIO_INIT_H_
#define _GPIO_INIT_H_

#include "variants.h"
#include "STM32Fxx_Pins.h"

/*
bit 0:1 - 00:input(reset vale)  01:output 10MHz  10:output 2Mhz  11:output 50MHz
bit 2:3 - Input  00:Analog input  01:input floating(reset value)  10:pull up/down  11:Reserved
        - Output 00:Output push-pull  01:Output open-drain  10:AF push-pull  11:AF open-drain
*/
typedef enum
{
  MGPIO_MODE_AIN = (0<<2)|(0),
  MGPIO_MODE_IPN = (1<<2)|(0),
  MGPIO_MODE_IPU = (2<<2)|(0),
  MGPIO_MODE_IPD = (3<<2)|(0),
  MGPIO_MODE_OUT_PP = (0<<2)|(3),
  MGPIO_MODE_OUT_OD = (1<<2)|(3),
  MGPIO_MODE_AF_PP = (2<<2)|(3),
  MGPIO_MODE_AF_OD = (3<<2)|(3),
} GPIO_MODE;

void GPIO_InitSet(uint16_t io, GPIO_MODE mode, uint8_t AF);
void GPIO_SetLevel(uint16_t io, uint8_t level);
void GPIO_ToggleLevel(uint16_t io);
uint8_t GPIO_GetLevel(uint16_t io);

#endif
