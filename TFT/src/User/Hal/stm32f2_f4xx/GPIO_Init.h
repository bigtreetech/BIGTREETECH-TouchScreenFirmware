#ifndef _GPIO_INIT_H_
#define _GPIO_INIT_H_

#include <stdint.h>
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
  MGPIO_MODE_IPN    = (0<<5) | (3<<3) | (0<<2) | (0),
  MGPIO_MODE_IPU    = (1<<5) | (3<<3) | (0<<2) | (0),
  MGPIO_MODE_IPD    = (2<<5) | (3<<3) | (0<<2) | (0),
  MGPIO_MODE_OUT_PP = (0<<5) | (3<<3) | (0<<2) | (1),
  MGPIO_MODE_OUT_OD = (0<<5) | (3<<3) | (1<<2) | (1),
  MGPIO_MODE_AF_PP  = (0<<5) | (3<<3) | (0<<2) | (2),
  MGPIO_MODE_AF_OD  = (0<<5) | (3<<3) | (1<<2) | (2),
  MGPIO_MODE_AIN    = (0<<5) | (3<<3) | (0<<2) | (3),
} GPIO_MODE;

void GPIO_InitSet(uint16_t io, GPIO_MODE mode, uint8_t AF);
void GPIO_SetLevel(uint16_t io, uint8_t level);
void GPIO_ToggleLevel(uint16_t io);
uint8_t GPIO_GetLevel(uint16_t io);

#endif
