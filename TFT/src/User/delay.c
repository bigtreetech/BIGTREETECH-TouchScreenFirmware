#include "delay.h"
#include "includes.h"

static uint8_t fac_us = 0;

void Delay_init(void)
{
  SysTick->CTRL &= 0xfffffffb;                                  // bit2 is cleared, select external clock HCLK / 8
  fac_us = mcuClocks.rccClocks.HCLK_Frequency / (8 * 1000000);  // 8 Frequency after frequency division Unit M is 1us times
}

void Delay_us(uint32_t us)  // Delay is less than 1800 * 1000us
{
  uint32_t temp;

  SysTick->LOAD = us * fac_us;  // Loading times, 1us fac_us times, us microsecond us * fac_us times
  SysTick->VAL = 0x00;          // Clear counter
  SysTick->CTRL = 0x01;         // Start countdown

  do
  {
    temp = SysTick->CTRL;
  } while(temp & 0x01 && !(temp & (1 << 16)));  // Wait time (after the countdown is completed, the hardware sets SysTick-> CTRL 16th bit to 1)

  SysTick->CTRL = 0x00;         // Close counter
  SysTick->VAL = 0x00;          // Clear counter
}

void Delay_ms(uint16_t ms)
{
  for (uint16_t i = 0; i < ms; i++)
  {
    Delay_us(1000);
  }
}
