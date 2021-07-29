#include "Knob_LED.h"
#include "includes.h"  // for mcuClocks, LED_COLOR_PIN etc...
#include "GPIO_Init.h"

#ifdef LED_COLOR_PIN

  // total 2.5us, run in 400Khz
  #define NEOPIXEL_T0H_US 0.35  // Neopixel code 0 high level hold time in us
  #define NEOPIXEL_T1H_US 2.15  // Neopixel code 1 high level hold time in us

  void knob_LED_Init(void)
  {
    GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_OUT_PP, 0);
    GPIO_SetLevel(LED_COLOR_PIN, 0);

    RCC->APB1ENR |= 1 << 4;  // TIM6 Clock enable
    TIM6->CNT = 0;
    TIM6->PSC = 1 - 1;
    TIM6->SR = (uint16_t)~(1 << 0);
  }

  void knob_LED_DeInit(void)
  {
    GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_IPN, 0);
  }

  void Knob_LED_SetColor(uint32_t ws2812_dat, uint8_t neopixel_pixels)
  {
    uint16_t led_num;
    int8_t bit;
    uint16_t cycle = mcuClocks.PCLK1_Timer_Frequency * (0.000001 * (NEOPIXEL_T0H_US + NEOPIXEL_T1H_US)) / 2 - 1;  // Neopixel frequency
    uint16_t code_0_tim_h_cnt = cycle * (NEOPIXEL_T0H_US / (NEOPIXEL_T0H_US + NEOPIXEL_T1H_US));  // Code 0, High level hold time,
    uint16_t code_1_tim_h_cnt = cycle - code_0_tim_h_cnt;

    __disable_irq();  // Disable interrupt, avoid disturbing the timing of WS2812
    TIM6->ARR = cycle;
    TIM6->CR1 |= 0x01;

    for (led_num=0; led_num < neopixel_pixels; led_num++)
    {
      for (bit = 23; bit >= 0; bit--)
      {
        TIM6->CNT = 0;
        WS2812_FAST_WRITE_HIGH();  // WS2812 required very high speed, so "GPIO_SetLevel(LED_COLOR_PIN, 1)" not applicable

        if (ws2812_dat & (1 << bit))
        {
          while (TIM6->CNT < code_1_tim_h_cnt);
        }
        else
        {
          while (TIM6->CNT < code_0_tim_h_cnt);
        }

        WS2812_FAST_WRITE_LOW();

        while (!TIM6->SR);

        TIM6->SR = 0;
      }
    }

    TIM6->CR1 &= ~0x01;
    __enable_irq();  // Enable interrupt
  }

#endif  // LED_COLOR_PIN
