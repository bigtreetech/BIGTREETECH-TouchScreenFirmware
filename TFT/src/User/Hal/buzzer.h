#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "stdint.h"

#define BUZZER_CACHE_SIZE 5
typedef struct
{
  uint32_t frequency[BUZZER_CACHE_SIZE];
  uint32_t duration[BUZZER_CACHE_SIZE];
  uint16_t wIndex;
  uint16_t rIndex;
  uint16_t count;
}BUZZER;


void Buzzer_Config(void);
void Buzzer_DeConfig(void);
void Buzzer_TurnOn(const uint32_t frequency, const uint32_t duration);
void loopBuzzer(void);

#endif
