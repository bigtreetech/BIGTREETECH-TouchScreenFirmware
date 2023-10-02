#include "buzzer.h"
#include "includes.h"  // for mcuClocks etc...
#include "GPIO_Init.h"

#ifdef BUZZER_PIN

#define BUZZER_CACHE_SIZE 16  // queue to store 16 sounds (1 sound takes 4 bytes)

typedef volatile struct
{
  uint16_t frequency[BUZZER_CACHE_SIZE];
  uint16_t duration[BUZZER_CACHE_SIZE];
  volatile uint32_t wIndex;
  volatile uint32_t rIndex;
  volatile int32_t toggles;  // number of half periods for the sound
} BUZZER;

static BUZZER buzzer;

void TIM3_Config(void)
{

#ifdef GD32F2XX
  rcu_periph_clock_enable(RCU_TIMER2);                                // enable timer clock
  TIMER_CTL0(TIMER2) &= ~TIMER_CTL0_CEN;                              // disable timer
  TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF;                             // clear update interrupt flag
  TIMER_DMAINTEN(TIMER2) |= TIMER_DMAINTEN_UPIE;                      // enable update interrupt
  TIMER_CAR(TIMER2) = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;  // 1 count = 1us

#else

  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->APB1ENR |= RCC_APB1Periph_TIM3;                        // enable timer clock
  TIM3->CR1 &= ~TIM_CR1_CEN;                                  // disable timer
  TIM3->SR &= ~TIM_SR_UIF;                                    // clear update interrupt flag
  TIM3->DIER |= TIM_DIER_UIE;                                 // enable update interrupt
  TIM3->ARR = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;  // 1 count = 1us
#endif
}

// play a tone with the help of interrupts
void tone(uint16_t frequency, const uint16_t duration)
{
  uint32_t silence = (frequency == SOUND_SILENCE);      // frequency == 0 indicates silence/pause in the sound

  if (silence)
    frequency = 1000;                                   // give 1ms resolution for silence

#ifdef GD32F2XX
  nvic_irq_disable(TIMER2_IRQn);                        // disable timer interrupt
  buzzer.toggles = 2 * (frequency * duration / 1000);   // 2 toggles per period
  TIMER_CTL0(TIMER2) &= ~TIMER_CTL0_CEN;                // disable timer
  TIMER_CNT(TIMER2) = 0;                                // reset counter
  TIMER_PSC(TIMER2) = (1000000 / (2 * frequency)) - 1;  // prescaler, half period time in us
  TIMER_CTL0(TIMER2) |= TIMER_CTL0_CEN;                 // enable timer
  nvic_irq_enable(TIMER2_IRQn, 1U, 0U);                 // enable timer interrupt
#else

  NVIC_DisableIRQ(TIM3_IRQn);                           // disable timer interrupt
  buzzer.toggles = 2 * (frequency * duration / 1000);   // 2 toggles per period
  TIM3->CR1 &= ~TIM_CR1_CEN;                            // disable timer
  TIM3->CNT = 0;                                        // reset counter
  TIM3->PSC = (1000000 / (2 * frequency)) - 1;          // prescaler, half period time in us
  TIM3->CR1 |= TIM_CR1_CEN;                             // enable timer
  NVIC_EnableIRQ(TIM3_IRQn);                            // enable interrupt
#endif

  if (silence)
    buzzer.toggles = -buzzer.toggles;                   // setup for silence, negative toggles
}

#ifdef GD32F2XX

void TIMER2_IRQHandler(void)  // ====== GD32F2XX TIMER ISR ======
{
  if ((TIMER_INTF(TIMER2) & TIMER_INTF_UPIF) != 0)  // check for timer2 interrupt flag
  {
    TIMER_INTF(TIMER2) &= ~TIMER_INTF_UPIF;         // clear interrupt flag

    if (buzzer.toggles > 0)
    {
      buzzer.toggles--;
      GPIO_SetLevel(BUZZER_PIN, buzzer.toggles & 1);  // play sound
    }
    else
    {
      if (buzzer.toggles == 0)          // sound done
      {
        nvic_irq_disable(TIMER2_IRQn);  // disable timer interrupt
        loopBuzzer();                   // check for more sounds
      }
      else
      {
        buzzer.toggles++;               // silence, no toggling, only counting
      }
    }
  }
}

#else

void TIM3_IRQHandler(void)  // ====== STM32FXX TIMER ISR ======
{
  if ((TIM3->SR & TIM_SR_UIF) != 0)  // check for TIM3 interrupt flag
  {
    TIM3->SR &= ~TIM_SR_UIF;         // clear interrupt flag

    if (buzzer.toggles > 0)
    {
      buzzer.toggles--;
      GPIO_SetLevel(BUZZER_PIN, buzzer.toggles & 1);
    }
    else
    {
      if (buzzer.toggles == 0)       // sound done
      {
        NVIC_DisableIRQ(TIM3_IRQn);  // disable timer interrupt
        loopBuzzer();                // check for more sounds
      }
      else
      {
        buzzer.toggles++;            // silence, no toggling, only counting
      }
    }
  }
}
#endif

void Buzzer_Config(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_OUT_PP, 0);
  TIM3_Config();
  buzzer.wIndex = buzzer.rIndex = buzzer.toggles = 0;
}

void Buzzer_DeConfig(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_IPN, 0);
  buzzer.wIndex = buzzer.rIndex = buzzer.toggles = 0;

#ifdef GD32F2XX
  nvic_irq_disable(TIMER2_IRQn);          // disable timer interrupt
  TIMER_CTL0(TIMER2) &= ~TIMER_CTL0_CEN;  // stop timer
#else
  NVIC_DisableIRQ(TIM3_IRQn);             // disable timer interrupt
  TIM3->CR1 &= ~TIM_CR1_CEN;              // stop timer
#endif
}

// store the sound in the queue, and start the sound if no sound is already playing
void Buzzer_TurnOn(const uint16_t frequency, const uint16_t duration)
{
  if (duration == 0)  // nothing to do, frequency == 0 indicates silence
    return;

  if (((buzzer.wIndex + 1) % BUZZER_CACHE_SIZE) == buzzer.rIndex)  // queue is full, drop sound
    return;

  buzzer.duration[buzzer.wIndex] = duration;                // store sound duration
  buzzer.frequency[buzzer.wIndex] = MIN(8000, frequency);   // prevent interrupting too fast
  buzzer.wIndex = (buzzer.wIndex + 1) % BUZZER_CACHE_SIZE;  // update wIndex

#ifdef GD32F2XX
  if ((TIMER_CTL0(TIMER2) & TIMER_CTL0_CEN))  // check if the timer is running before playing the tone
    return;
#else
  if ((TIM3->CR1 & TIM_CR1_CEN))              // check if the timer is running before playing the  tone
    return;
#endif

  loopBuzzer();
}

// play a sound from the sound queue, called by timer interrupt!
void loopBuzzer(void)
{
#ifdef GD32F2XX
  TIMER_CTL0(TIMER2) &= ~TIMER_CTL0_CEN;  // stop timer
#else
  TIM3->CR1 &= ~TIM_CR1_CEN;              // stop timer
#endif

  if (buzzer.rIndex != buzzer.wIndex)  // play queued sound
  {
    uint16_t freq     = buzzer.frequency[buzzer.rIndex];
    uint16_t duration = buzzer.duration[buzzer.rIndex];
    buzzer.rIndex = (buzzer.rIndex + 1) % BUZZER_CACHE_SIZE;

    if ((freq > SOUND_KEYPRESS))
      tone(freq, duration);
    else
      Buzzer_play(freq);
  }
  else
  {
    GPIO_SetLevel(BUZZER_PIN, BUZZER_STOP_LEVEL);  // make sure to leave the buzzer in the unpowered state
  }
}

#endif  // BUZZER_PIN
