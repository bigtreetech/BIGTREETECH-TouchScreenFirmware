#ifndef _STM32_FLASH_H_
#define _STM32_FLASH_H_

#include "stm32f2xx.h"

void STM32_FlashRead(u8 *data, u32 len);
void STM32_FlashWrite(u8 *data, u32 len);

#endif
