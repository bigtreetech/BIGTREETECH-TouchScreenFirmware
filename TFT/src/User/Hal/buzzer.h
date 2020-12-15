#ifndef _BUZZER_H_
#define _BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define BUZZER_CACHE_SIZE 5
typedef struct
{
  uint32_t frequency[BUZZER_CACHE_SIZE];
  uint32_t duration[BUZZER_CACHE_SIZE];
  uint16_t wIndex;
  uint16_t rIndex;
  uint16_t count;
}BUZZER;

typedef enum
{
  sound_success,
  sound_error,
  sound_ok,
  sound_cancel,
  sound_notify,
  sound_deny,
  sound_toast,
  sound_keypress,
}SOUND;


void Buzzer_Config(void);
void Buzzer_DeConfig(void);
void Buzzer_TurnOn(const uint16_t frequency, const uint16_t duration);
void Buzzer_play(SOUND sound);
void loopBuzzer(void);

#ifdef BUZZER_PIN
#define BUZZER_PLAY(x) Buzzer_play(x)
#else
#define BUZZER_PLAY(x)
#endif

#ifdef __cplusplus
}
#endif

#endif
