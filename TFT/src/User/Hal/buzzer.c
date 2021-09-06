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

static BUZZER buzzer;
volatile uint32_t buzzerEndTime = 0;
volatile uint32_t toggles = 0;

void TIM3_Config(void)
{
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
  TIM3->ARR = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;  // 20hz to 1Mhz
}

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
    }
    TIM3->SR &= ~TIM_SR_UIF;  // clear interrupt flag
  }
}

void Buzzer_Config(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_OUT_PP, 0);
  TIM3_Config();
}

void Buzzer_DeConfig(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_IPN, 0);
}

void Buzzer_TurnOn(const uint16_t frequency, const uint16_t duration)
{
  while (buzzer.count == BUZZER_CACHE_SIZE)
  {
    loopBuzzer();
  }
  buzzer.duration[buzzer.wIndex] = duration;
  buzzer.frequency[buzzer.wIndex] = frequency;
  buzzer.wIndex = (buzzer.wIndex + 1) % BUZZER_CACHE_SIZE;
  buzzer.count++;
}

void tone(const uint16_t frequency, const uint16_t duration)
{
  if (frequency == 0 || duration == 0) return;

  NVIC_DisableIRQ(TIM3_IRQn);
  toggles = 2 * (frequency * duration / 1000);  // must have an even value

  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM3->CNT =0;
  TIM3->PSC = (1000000 / (2 * frequency)) - 1;
  TIM3->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(TIM3_IRQn);
}

void loopBuzzer(void)
{
  if (!buzzerEndTime)
  {
    if (buzzer.count == 0) return;
    buzzerEndTime = OS_GetTimeMs() + buzzer.duration[buzzer.rIndex];
    if (buzzer.frequency[buzzer.rIndex] > 0)
    {
      tone(buzzer.frequency[buzzer.rIndex], buzzer.duration[buzzer.rIndex]);
    }
    buzzer.rIndex = (buzzer.rIndex + 1) % BUZZER_CACHE_SIZE;
    buzzer.count--;
  }
  else if (OS_GetTimeMs() > buzzerEndTime && toggles == 0)
  {
    buzzerEndTime = 0;
    TIM3->CR1 &= ~TIM_CR1_CEN;  // stop timer (for safety)
    GPIO_SetLevel(BUZZER_PIN, BUZZER_STOP_LEVEL);
  }
}

#endif  // BUZZER_PIN
