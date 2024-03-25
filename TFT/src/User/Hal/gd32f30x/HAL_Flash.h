#ifndef _HAL_FLASH_H_
#define _HAL_FLASH_H_

#include <stdint.h>

void HAL_FlashRead(uint8_t * data, uint32_t len);
void HAL_FlashWrite(uint8_t * data, uint32_t len);

#endif
