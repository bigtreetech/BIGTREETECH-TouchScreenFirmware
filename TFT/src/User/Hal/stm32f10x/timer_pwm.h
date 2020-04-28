#ifndef _TIMER_PWM_H_
#define _TIMER_PWM_H_

#include "stm32f10x.h"

#define _TIM1    0
#define _TIM2    1
#define _TIM3    2
#define _TIM4    3
#define _TIM5    4
#define _TIM6    5 // NOTE: TIM6 & TIM7 basic timer, can not PWM generation
#define _TIM7    6
#define _TIM8    7
#define _TIM_CNT 8

#define _TIM1_CH1  (((_TIM1)<<8) + 0)
#define _TIM1_CH2  (((_TIM1)<<8) + 1)
#define _TIM1_CH3  (((_TIM1)<<8) + 2)
#define _TIM1_CH4  (((_TIM1)<<8) + 3)

#define _TIM2_CH1  (((_TIM2)<<8) + 0)
#define _TIM2_CH2  (((_TIM2)<<8) + 1)
#define _TIM2_CH3  (((_TIM2)<<8) + 2)
#define _TIM2_CH4  (((_TIM2)<<8) + 3)

#define _TIM3_CH1  (((_TIM3)<<8) + 0)
#define _TIM3_CH2  (((_TIM3)<<8) + 1)
#define _TIM3_CH3  (((_TIM3)<<8) + 2)
#define _TIM3_CH4  (((_TIM3)<<8) + 3)

#define _TIM4_CH1  (((_TIM4)<<8) + 0)
#define _TIM4_CH2  (((_TIM4)<<8) + 1)
#define _TIM4_CH3  (((_TIM4)<<8) + 2)
#define _TIM4_CH4  (((_TIM4)<<8) + 3)

#define _TIM5_CH1  (((_TIM5)<<8) + 0)
#define _TIM5_CH2  (((_TIM5)<<8) + 1)
#define _TIM5_CH3  (((_TIM5)<<8) + 2)
#define _TIM5_CH4  (((_TIM5)<<8) + 3)

#define _TIM6_CH1  (((_TIM6)<<8) + 0)
#define _TIM6_CH2  (((_TIM6)<<8) + 1)
#define _TIM6_CH3  (((_TIM6)<<8) + 2)
#define _TIM6_CH4  (((_TIM6)<<8) + 3)

#define _TIM7_CH1  (((_TIM7)<<8) + 0)
#define _TIM7_CH2  (((_TIM7)<<8) + 1)
#define _TIM7_CH3  (((_TIM7)<<8) + 2)
#define _TIM7_CH4  (((_TIM7)<<8) + 3)

#define _TIM8_CH1  (((_TIM8)<<8) + 0)
#define _TIM8_CH2  (((_TIM8)<<8) + 1)
#define _TIM8_CH3  (((_TIM8)<<8) + 2)
#define _TIM8_CH4  (((_TIM8)<<8) + 3)

#define TIMER_GET_TIM(n) ((n>>8) & 0xFF)
#define TIMER_GET_CH(n) (n & 0xFF)

void TIM_PWM_SetDutyCycle(uint16_t tim_ch, uint8_t duty);
void TIM_PWM_Init(uint16_t tim_ch);

#endif
