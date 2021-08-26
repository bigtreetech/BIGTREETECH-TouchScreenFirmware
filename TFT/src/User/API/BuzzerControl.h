#ifndef _BUZZER_CONTROL_H_
#define _BUZZER_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for BUZZER_PIN etc...

typedef enum
{
  sound_success = 0,
  sound_error,
  sound_ok,
  sound_cancel,
  sound_notify,
  sound_deny,
  sound_toast,
  sound_keypress,
  sound_heated,
  sound_cooled,
} SOUND;

#ifdef BUZZER_PIN
  void Buzzer_play(SOUND sound);

  #define BUZZER_PLAY(x) Buzzer_play(x)
#else
  #define BUZZER_PLAY(x)
#endif

#ifdef __cplusplus
}
#endif

#endif
