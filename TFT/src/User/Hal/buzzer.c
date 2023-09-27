#include "buzzer.h"
#include "includes.h"  // for mcuClocks etc...
#include "GPIO_Init.h"

#ifdef BUZZER_PIN

#define BUZZER_CACHE_SIZE 5

typedef struct
{
  uint32_t frequency[BUZZER_CACHE_SIZE];
  uint32_t duration[BUZZER_CACHE_SIZE];
  uint16_t wIndex;
  uint16_t rIndex;
  uint16_t count;
} BUZZER;

volatile static BUZZER buzzer;
volatile uint32_t toggles = 0;

static inline void BuzzerTimerConfig(void)
{
#ifdef GD32F2XX
  nvic_irq_enable(TIMER2_IRQn, 1U, 0U);

  rcu_periph_clock_enable(RCU_TIMER2);
  TIMER_CTL0(TIMER2)     &= ~TIMER_CTL0_CEN;
  TIMER_DMAINTEN(TIMER2) |= TIMER_DMAINTEN_UPIE;
  TIMER_INTF(TIMER2)     &= ~TIMER_INTF_UPIF;

  // initial values
  TIMER_CAR(TIMER2) = 10 - 1;
  TIMER_PSC(TIMER2) = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;
#else
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->APB1ENR |= RCC_APB1Periph_TIM3;
  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM3->DIER |= TIM_DIER_UIE;
  TIM3->SR &= ~TIM_SR_UIF;

  // initial values
  TIM3->ARR = 10 - 1;
  TIM3->PSC = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;
#endif
}

#ifdef GD32F2XX
void TIMER2_IRQHandler(void)
{
  if ((TIMER_INTF(TIMER2) & TIMER_INTF_UPIF) != 0)  // update interrupt flag
  {
    if (toggles != 0)
    {
      toggles--;
      GPIO_ToggleLevel(BUZZER_PIN);
    }
    else
    {
      TIMER_CTL0(TIMER2) &= ~TIMER_CTL0_CEN;  // disable timer
      TIMER_CNT(TIMER2) = 0;

      if (buzzer.count == 0)  // end sound generation
      {
        GPIO_SetLevel(BUZZER_PIN, BUZZER_STOP_LEVEL);

        // initial values
        TIMER_CAR(TIMER2) = 10 - 1;
        TIMER_PSC(TIMER2) = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;
      }
      else  // step to the sound to play
      {
        toggles = 2 * ((buzzer.frequency[buzzer.rIndex]) * buzzer.duration[buzzer.rIndex] / 1000);  // must have an even value

        if (buzzer.frequency[buzzer.rIndex] == 0)  // handle pause (silence)
        { // handle pause (silence)
          TIMER_CAR(TIMER2) = (10 * buzzer.duration[buzzer.rIndex]) - 1;    // keep values in range (TIMER2 CAR is only 16 bit)
          TIMER_PSC(TIMER2) = mcuClocks.PCLK1_Timer_Frequency / 10000 - 1;  // keep values in range (TIMER2 PSC is only 16 bit)
        }
        else
        { // handle sound
          TIMER_CAR(TIMER2) = (1000000 / (2 * buzzer.frequency[buzzer.rIndex])) - 1;  // keep values in range (TIMER2 CAR is only 16 bit)
          TIMER_PSC(TIMER2) = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;          // keep values in range (TIMER2 PSC is only 16 bit)
        }

        TIMER_SWEVG(TIMER2) |= TIMER_SWEVG_UPG;  // initialize PSC and CAR
        TIMER_CTL0(TIMER2)  |= TIMER_CTL0_CEN;   // re-enable timer

        if (++buzzer.rIndex == BUZZER_CACHE_SIZE)
          buzzer.rIndex = 0;

        buzzer.count--;
      }
    }
    TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF;  // clear interrupt flag
  }
}
#else
void TIM3_IRQHandler(void)
{
  if ((TIM3->SR & TIM_SR_UIF) != 0)  // update interrupt flag
  {
    if (toggles != 0)
    {
      toggles--;
      GPIO_ToggleLevel(BUZZER_PIN);
    }
    else
    {
      TIM3->CR1 &= ~TIM_CR1_CEN;  // stop timer
      TIM3->CNT = 0;

      if (buzzer.count == 0)  // end sound generation
      {
        GPIO_SetLevel(BUZZER_PIN, BUZZER_STOP_LEVEL);

        // initial values
        TIM3->ARR = 10 - 1;
        TIM3->PSC = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;
      }
      else  // step to the next sound
      {
        toggles = 2 * ((buzzer.frequency[buzzer.rIndex]) * buzzer.duration[buzzer.rIndex] / 1000);  // must have an even value

        if (buzzer.frequency[buzzer.rIndex] == 0)  // handle pause (silence)
        { // handle pause (silence)
          TIM3->ARR = (10 * buzzer.duration[buzzer.rIndex]) - 1;    // keep values in range (TIMER3 ARR is only 16 bit)
          TIM3->PSC = mcuClocks.PCLK1_Timer_Frequency / 10000 - 1;  // keep values in range (TIMER3 PSC is only 16 bit)
        }
        else
        { // handle sound
          TIM3->ARR = (1000000 / (2 * buzzer.frequency[buzzer.rIndex])) - 1;  // keep values in range (TIMER3 ARR is only 16 bit)
          TIM3->PSC = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;          // keep values in range (TIMER3 PSC is only 16 bit)
        }

        TIM3->EGR |= TIM_EGR_UG;   // initialize PSC and ARR
        TIM3->CR1 |= TIM_CR1_CEN;  // re-enable timer

        if (++buzzer.rIndex == BUZZER_CACHE_SIZE)
          buzzer.rIndex = 0;

        buzzer.count--;
      }
    }
    TIM3->SR &= ~TIM_SR_UIF;  // clear interrupt flag
  }
}
#endif

void Buzzer_Config(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_OUT_PP, 0);
  BuzzerTimerConfig();
}

void Buzzer_DeConfig(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_IPN, 0);
}

void Buzzer_AddSound(const uint16_t frequency, const uint16_t duration)
{
  // If the sound cache is full than the oldest sound data
  // will be overwritten with the newest one that arrives.

  buzzer.duration[buzzer.wIndex] = duration;
  buzzer.frequency[buzzer.wIndex] = frequency;

  if (++buzzer.wIndex == BUZZER_CACHE_SIZE)
    buzzer.wIndex = 0;

  if (buzzer.count == BUZZER_CACHE_SIZE)
  {
    // disable timer to avoid an eventual race condition
    #ifdef GD32F2XX
      TIMER_CTL0(TIMER2) &= ~TIMER_CTL0_CEN;
    #else
      TIM3->CR1 &= ~TIM_CR1_CEN;
    #endif

    buzzer.rIndex = buzzer.wIndex;
  }
  else
  {
    buzzer.count++;
  }

  // (re)enable timer
  #ifdef GD32F2XX
    TIMER_CTL0(TIMER2) |= TIMER_CTL0_CEN;
  #else
    TIM3->CR1 |= TIM_CR1_CEN;
  #endif
}

#endif  // BUZZER_PIN
