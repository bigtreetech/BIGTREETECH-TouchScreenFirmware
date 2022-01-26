#include "HAL_Flash.h"
#include "my_misc.h"

/*
Page 0 0x0800 0000 - 0x0800 07FF 2 Kbyte
Page 1 0x0800 0800 - 0x0800 0FFF 2 Kbyte
Page 2 0x0800 1000 - 0x0800 17FF 2 Kbyte
...
...
...
Page 255 0x0807 F800 - 0x0807 FFFF 2 Kbyte  // 512KByte
*/

#define SIGN_ADDRESS (0x08040000 - 0x800)  // reserve the last page (2KB) to save user parameters

void HAL_FlashRead(uint8_t *data, uint32_t len)
{
  uint32_t i = 0;
  for (i = 0; i < len; i++)
  {
    data[i] = *(volatile uint8_t*)(SIGN_ADDRESS + i);
  }
}

void HAL_FlashWrite(uint8_t *data, uint32_t len)
{
  uint32_t i = 0;
  fmc_unlock();
  fmc_page_erase(SIGN_ADDRESS);
  fmc_flag_clear(FMC_FLAG_BANK0_END);
  fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
  fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
  for (i = 0; i < len; i += 2)
  {
    uint16_t data16 = data[i] | (data[MIN(i+1, len-1)] << 8);  // gd32f20x needs to write at least 16 bits at a time
    fmc_halfword_program(SIGN_ADDRESS + i, data16);
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
  }
  fmc_lock();
}
