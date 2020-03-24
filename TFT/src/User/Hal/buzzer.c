#include "buzzer.h"
#include "GPIO_Init.h"
#include "includes.h"

#ifdef BUZZER_PIN

void TIM3_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC->APB1ENR |= RCC_APB1Periph_TIM3;
	TIM3->CR1 &= ~(0x01);
	TIM3->DIER |= 1<<0;
  TIM3->SR = (uint16_t)~(1<<0);
 	TIM3->ARR = F_CPUM - 1; // 20hz to 1Mhz
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

static BUZZER buzzer;
volatile uint32_t buzzerEndTime = 0;

void tone(const uint32_t frequency, const uint32_t duration);
void loopBuzzer(void) {
  const uint32_t now = OS_GetTimeMs();

  if (!buzzerEndTime) {
    if (buzzer.count == 0) return;

    buzzerEndTime = now + buzzer.duration[buzzer.rIndex];

    if (buzzer.frequency[buzzer.rIndex] > 0) {
      tone(buzzer.frequency[buzzer.rIndex], buzzer.duration[buzzer.rIndex]);
    }
    
    buzzer.rIndex = (buzzer.rIndex + 1) % BUZZER_CACHE_SIZE;
    buzzer.count--;
  }
  else if (now > buzzerEndTime) {
    buzzerEndTime = 0;
    GPIO_SetLevel(BUZZER_PIN, BUZZER_STOP_LEVEL);
  }
}

void Buzzer_TurnOn(const uint32_t frequency, const uint32_t duration)
{
    if(infoSettings.silent) return;  
  while (buzzer.count == BUZZER_CACHE_SIZE) {
    loopBuzzer();
  }
  buzzer.duration[buzzer.wIndex] = duration;
  buzzer.frequency[buzzer.wIndex] = frequency;
  buzzer.wIndex = (buzzer.wIndex + 1) % BUZZER_CACHE_SIZE;
  buzzer.count++;
}


volatile uint32_t toggles = 0;
void tone(const uint32_t frequency, const uint32_t duration) {
  if (frequency == 0 || duration == 0) return;
  
  NVIC_DisableIRQ(TIM3_IRQn);
  toggles = 2 * frequency * duration / 1000;

  TIM3->CR1 &= ~(0x01);
	TIM3->CNT =0;
	TIM3->PSC = (1000000 / (2 * frequency)) - 1;
  TIM3->CR1 |= 0x01;

  NVIC_EnableIRQ(TIM3_IRQn);
}


void TIM3_IRQHandler(void) {
  if ((TIM3->SR & 0x01) != 0) {   // update interrupt flag
    TIM3->SR = (uint16_t)~(1<<0); // clear interrupt flag
    if (toggles != 0) {
      if(toggles > 0) toggles--;
       GPIO_ToggleLevel(BUZZER_PIN);
    } else {
      TIM3->CR1 &= ~(0x01); // stop timer
    }
  }
}

#endif
