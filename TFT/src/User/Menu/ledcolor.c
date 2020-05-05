#include "ledcolor.h"
#include "GPIO_Init.h"
#include "includes.h"


#ifdef LED_COLOR_PIN

//preset color list
const LABEL itemLedcolor[LED_COLOR_NUM] = {
  //label
  LABEL_OFF,
  LABEL_WHITE,
  LABEL_RED,
  LABEL_ORANGE,
  LABEL_YELLOW,
  LABEL_GREEN,
  LABEL_BLUE,
  LABEL_INDIGO,
  LABEL_VIOLET,
};

const  uint32_t led_color[LED_COLOR_NUM] = {
  LED_OFF,
  LED_WHITE,
  LED_RED,
  LED_ORANGE,
  LED_YELLOW,
  LED_GREEN,
  LED_BLUE,
  LED_INDIGO,
  LED_VIOLET
};

void knob_LED_Init(void)
{
  GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_OUT_PP, 0);
  GPIO_SetLevel(LED_COLOR_PIN, 0);
  
  RCC->APB1ENR |= 1<<4; // TIM6 Clock enable
  TIM6->CNT = 0;
  TIM6->ARR = mcuClocks.PCLK1_Timer_Frequency / 800000 / 2 - 1;  // 800KHz
  TIM6->PSC = 1 - 1;
  TIM6->SR = (uint16_t)~(1<<0);
  
  WS2812_Send_DAT(led_color[infoSettings.knob_led_color]); //set last saved color after initialization
}

void knob_LED_DeInit(void)
{
  GPIO_InitSet(LED_COLOR_PIN,MGPIO_MODE_IPN,0);
}

void set_knob_color(int color_index)
{
  WS2812_Send_DAT(led_color[color_index]);
}

void WS2812_Send_DAT(uint32_t ws2812_dat)
{
  uint16_t led_num;
  int8_t bit;
  uint16_t code_0_tim_h_cnt = TIM6->ARR * (0.4/1.25);  // Code 0, High level hold time, 0.4us / 1.25us
  uint16_t code_1_tim_h_cnt = TIM6->ARR - code_0_tim_h_cnt;

  TIM6->CR1 |= 0x01;
  for (led_num=0; led_num < NEOPIXEL_PIXELS; led_num++)
  {
    for (bit = 23; bit >= 0; bit--)
    {
      TIM6->CNT=0;
      GPIO_SetLevel(LED_COLOR_PIN,1);
      if (ws2812_dat & (1 << bit)) {
        while (TIM6->CNT < code_1_tim_h_cnt);
      } else {
        while (TIM6->CNT < code_0_tim_h_cnt);
      }

      GPIO_SetLevel(LED_COLOR_PIN,0);
      while (!TIM6->SR);
      TIM6->SR = 0;
    }
  }
  TIM6->CR1 &= ~0x01;
}
#endif
