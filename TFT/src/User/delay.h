#ifndef _DELAY_H_
#define _DELAY_H_
#include "stdint.h"

void Delay_init(uint8_t clk);   //初始化
void Delay_ms(uint16_t ms);     //延时毫秒级
void Delay_us(uint32_t us);     //延时微秒级
void Delay(uint32_t time);      //软件延时

#endif
