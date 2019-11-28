#include "ledcolor.h"
#include "GPIO_Init.h"
#include "includes.h"


#ifdef LED_color_PIN

void led_color_Init(uint16_t psc,uint16_t arr) // 12 11
{

    GPIO_InitSet(LED_color_PIN,MGPIO_MODE_OUT_PP,0);
    GPIO_SetLevel(LED_color_PIN,0);
	//打开时钟
	RCC->APB1ENR|=1<<4;//TIM6时钟使能
	//复位
	RCC->APB1RSTR|=1<<4;
	RCC->APB1RSTR&=~(1<<4);

	TIM6->CNT=0;
	TIM6->PSC=psc-1;
	TIM6->ARR=arr;
	//TIM6->CR1|=1<<0;//使能定时器6  
}

void LED_color_PIN_IPN(void) 
{
    GPIO_InitSet(LED_color_PIN,MGPIO_MODE_IPN,0);
}

void ws2812_send_DAT(uint32_t ws2812_dat)
{
    u8 CNT;
    u8 now_flag=0;
    u8 led_color_num=0;
    uint32_t old_ws2812_dat=ws2812_dat;
    TIM6->CR1|=0x01;    
    while(!(TIM6->SR)){};
    for(led_color_num=0;led_color_num<4;led_color_num++)
    {   ws2812_dat=old_ws2812_dat;
        for(CNT=0;CNT<24;CNT++)
        {
            TIM6->CNT=0;            //1
            TIM6->SR=0;
            if(ws2812_dat & 0x800000){
            TIM6->ARR=8;
            now_flag=8;
            }
            else{
            TIM6->ARR=3;
            now_flag=3;
            }
            GPIO_SetLevel(LED_color_PIN,1);
            while(!(TIM6->SR)){};
            TIM6->CNT=0;
            TIM6->SR=0;
            TIM6->ARR=11-now_flag;
            GPIO_SetLevel(LED_color_PIN,0);
            while(!(TIM6->SR)){};
            ws2812_dat<<=1;
        }
    }
    TIM6->CR1&=~(1<<0);
}
#endif

