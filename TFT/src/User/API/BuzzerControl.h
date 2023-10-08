#ifndef _BUZZER_CONTROL_H_
#define _BUZZER_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"  // for BUZZER_PIN etc...

typedef enum
{
  SOUND_MUTE = 0,
  SOUND_UNMUTE,
  SOUND_SUCCESS,
  SOUND_ERROR,
  SOUND_OK,
  SOUND_CANCEL,
  SOUND_NOTIFY,
  SOUND_DENY,
  SOUND_TOAST,
  SOUND_HEATED,
  SOUND_COOLED,
  SOUND_STARTUP,
  SOUND_START_PRINT,
  SOUND_GOING_DOWN,
  SOUND_COMPLETED,
  SOUND_ATTENTION,
  SOUND_OTPW_ERROR,
  SOUND_KEYPRESS,
  SOUND_SILENCE,
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
