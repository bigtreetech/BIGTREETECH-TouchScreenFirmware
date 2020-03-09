#include "variants.h"

static u16 fac_ms=0;
static u8 fac_us=0;

void Delay_init(u8 clk)
{
  SysTick->CTRL&=0xfffffffb;		//bit2清空，选外部时钟 HCLK/8
  fac_us=clk/8;									//8分频后的频率 单位M 即1us次数
  fac_ms=(u16)fac_us*1000;			// 1ms的次数 即为1us的1000倍
}

void Delay_us(u32 us)					//延时小于1800*1000us
{
  u32 temp;
  SysTick->LOAD=us*fac_us;  		//装载次数,1us fac_us次，us微秒 us*fac_us次
  SysTick->VAL=0x00;						//清空计数器
  SysTick->CTRL=0x01;						//开始倒数
  do
  {
    temp=SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1<<16)));	 	//等待时间到达（倒数完成后，硬件将SysTick->CTRL第16位置1）
  SysTick->CTRL=0x00;						//关闭计数器
  SysTick->VAL=0x00;						//清空计数器
}

void Delay_ms(u16 ms)					//延时小于1800ms
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

void Delay(int time)
{
  int i;
  while(time--)
  for(i=0;i<120;i++);
}
