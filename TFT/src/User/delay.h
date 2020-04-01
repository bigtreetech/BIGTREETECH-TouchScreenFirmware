#ifndef _DELAY_H_
#define _DELAY_H_
#include "stdint.h"

void Delay_init(uint8_t clk);   //��ʼ��
void Delay_ms(uint16_t ms);     //��ʱ���뼶
void Delay_us(uint32_t us);     //��ʱ΢�뼶
void Delay(uint32_t time);      //������ʱ

#endif
