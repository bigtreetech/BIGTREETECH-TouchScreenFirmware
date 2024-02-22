#ifndef _BUZZER_CONTROL_H_
#define _BUZZER_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"  // for BUZZER_PIN etc...

typedef enum
{
  // alert sound types
  SOUND_SUCCESS = 0,
  SOUND_ERROR,
  SOUND_OK,
  SOUND_CANCEL,
  SOUND_NOTIFY,
  SOUND_DENY,
  // toast sound types
  SOUND_TOAST,
  // heater sound types
  SOUND_HEATED,
  SOUND_COOLED,
  // touch sound types
  SOUND_KEYPRESS,
} SOUND;

#ifdef BUZZER_PIN
  // mute (with the exception of touch type sound, if enabled) or unmute the TFT if not already muted/unmuted respectively
  void Buzzer_HandleMute(bool mute);

  void Buzzer_Play(SOUND sound);

  #define BUZZER_PLAY(x) Buzzer_Play(x)
#else
  #define BUZZER_PLAY(x)
#endif

#ifdef __cplusplus
}
#endif

#endif
