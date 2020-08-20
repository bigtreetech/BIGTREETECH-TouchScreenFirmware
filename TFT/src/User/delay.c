#include "includes.h"

static u16 fac_ms=0;
static u8 fac_us=0;

void Delay_init(void)
{
  SysTick->CTRL&=0xfffffffb;  // bit2 is cleared, select external clock HCLK / 8
  fac_us=mcuClocks.rccClocks.HCLK_Frequency/(8*1000000); // 8 Frequency after frequency division Unit M is 1us times
  fac_ms=(u16)fac_us * 1000;  // The number of times in 1ms is 1000 times of 1us
}

void Delay_us(u32 us)					//Delay is less than 1800 * 1000us
{
  u32 temp;
  SysTick->LOAD=us*fac_us;  		//Loading times, 1us fac_us times, us microsecond us * fac_us times
  SysTick->VAL=0x00;						//Clear counter
  SysTick->CTRL=0x01;						//Start countdown
  do
  {
    temp=SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1<<16)));	 	//Wait time (after the countdown is completed, the hardware sets SysTick-> CTRL 16th bit to 1)
  SysTick->CTRL=0x00;						//Close counter
  SysTick->VAL=0x00;						//Clear counter
}

void Delay_ms(u16 ms)					//Delay is less than 1800ms
{
  u32 temp;
  SysTick->LOAD=(u32)ms*fac_ms;
  SysTick->VAL=0x00;
  SysTick->CTRL=0x01;
  do
  {
    temp=SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1<<16)));
  SysTick->CTRL=0x00;
  SysTick->VAL=0x00;
}
