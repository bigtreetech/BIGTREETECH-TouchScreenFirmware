#include "Knob_LED.h"
#include "includes.h"  // for mcuClocks, LED_COLOR_PIN etc...
#include "GPIO_Init.h"

#ifdef LED_COLOR_PIN

uint32_t frameTimeStamp = 0;  // Frame unit need > 280us for WS2812

// total 2.5us, run in 400Khz
#define NEOPIXEL_T0H_US 0.35  // Neopixel code 0 high level hold time in us
#define NEOPIXEL_T1H_US 2.15  // Neopixel code 1 high level hold time in us

#ifdef GD32F2XX
  #define NEOPIXEL_TIMER_CLOCK_ENABLE() rcu_periph_clock_enable(RCU_TIMER5)
  #define NEOPIXEL_TIMER_CNT() TIMER_CNT(TIMER5)
  #define NEOPIXEL_TIMER_PSC() TIMER_PSC(TIMER5)
  #define NEOPIXEL_TIMER_ARR() TIMER_CAR(TIMER5)
  #define NEOPIXEL_TIMER_SR()  TIMER_INTF(TIMER5)
  #define NEOPIXEL_TIMER_CR1() TIMER_CTL0(TIMER5)
#else
  #define NEOPIXEL_TIMER_CLOCK_ENABLE() RCC->APB1ENR |= 1 << 4 // TIM6 Clock enable
  #define NEOPIXEL_TIMER_CNT() TIM6->CNT
  #define NEOPIXEL_TIMER_PSC() TIM6->PSC
  #define NEOPIXEL_TIMER_ARR() TIM6->ARR
  #define NEOPIXEL_TIMER_SR()  TIM6->SR
  #define NEOPIXEL_TIMER_CR1() TIM6->CR1
#endif

void knob_LED_Init(void)
{
  GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_OUT_PP, 0);
  GPIO_SetLevel(LED_COLOR_PIN, 0);

  NEOPIXEL_TIMER_CLOCK_ENABLE();
  NEOPIXEL_TIMER_CNT() = 0;
  NEOPIXEL_TIMER_PSC() = 1 - 1;
  NEOPIXEL_TIMER_SR() = (uint16_t)~(1 << 0);
}

void knob_LED_DeInit(void)
{
  GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_IPN, 0);
}

void Knob_LED_SetColor(uint32_t color, uint8_t neopixel_pixels)
{
  uint16_t led_num;
  int8_t bit;
  uint16_t cycle = mcuClocks.PCLK1_Timer_Frequency * (0.000001 * (NEOPIXEL_T0H_US + NEOPIXEL_T1H_US)) / 2 - 1;  // Neopixel frequency
  uint16_t code_0_tim_h_cnt = cycle * (NEOPIXEL_T0H_US / (NEOPIXEL_T0H_US + NEOPIXEL_T1H_US));  // Code 0, High level hold time,
  uint16_t code_1_tim_h_cnt = cycle - code_0_tim_h_cnt;

  while (frameTimeStamp == OS_GetTimeMs());

  __disable_irq();  // Disable interrupt, avoid disturbing the timing of WS2812
  NEOPIXEL_TIMER_ARR() = cycle;
  NEOPIXEL_TIMER_CR1() |= 0x01;

  for (led_num = 0; led_num < neopixel_pixels; led_num++)
  {
    for (bit = 23; bit >= 0; bit--)
    {
      NEOPIXEL_TIMER_CNT() = 0;
      WS2812_FAST_WRITE_HIGH();  // WS2812 required very high speed, so "GPIO_SetLevel(LED_COLOR_PIN, 1)" not applicable

      if (color & (1 << bit))
      {
        while (NEOPIXEL_TIMER_CNT() < code_1_tim_h_cnt);
      }
      else
      {
        while (NEOPIXEL_TIMER_CNT() < code_0_tim_h_cnt);
      }

      WS2812_FAST_WRITE_LOW();

      while (!NEOPIXEL_TIMER_SR());

      NEOPIXEL_TIMER_SR() = 0;
    }
  }

  NEOPIXEL_TIMER_CR1() &= ~0x01;
  __enable_irq();  // Enable interrupt

  frameTimeStamp = OS_GetTimeMs();
}

#endif  // LED_COLOR_PIN
