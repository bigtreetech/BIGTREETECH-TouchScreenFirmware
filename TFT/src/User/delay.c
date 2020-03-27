#include "variants.h"

static u16 fac_ms=0;
static u8 fac_us=0;

void Delay_init(u8 clk)
{
  SysTick->CTRL&=0xfffffffb;		//bit2��գ�ѡ�ⲿʱ�� HCLK/8
  fac_us=clk/8;									//8��Ƶ���Ƶ�� ��λM ��1us����
  fac_ms=(u16)fac_us*1000;			// 1ms�Ĵ��� ��Ϊ1us��1000��
}

void Delay_us(u32 us)					//��ʱС��1800*1000us
{
  u32 temp;
  SysTick->LOAD=us*fac_us;  		//װ�ش���,1us fac_us�Σ�us΢�� us*fac_us��
  SysTick->VAL=0x00;						//��ռ�����
  SysTick->CTRL=0x01;						//��ʼ����
  do
  {
    temp=SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1<<16)));	 	//�ȴ�ʱ�䵽�������ɺ�Ӳ����SysTick->CTRL��16λ��1��
  SysTick->CTRL=0x00;						//�رռ�����
  SysTick->VAL=0x00;						//��ռ�����
}

void Delay_ms(u16 ms)					//��ʱС��1800ms
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
