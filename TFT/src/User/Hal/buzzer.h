#ifndef _BUZZER_H_
#define _BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "variants.h"  // for BUZZER_PIN etc...

#ifdef BUZZER_PIN
  void Buzzer_Config(void);
  void Buzzer_DeConfig(void);
  void Buzzer_TurnOn(const uint16_t frequency, const uint16_t duration);
  void loopBuzzer(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
