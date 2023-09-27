#include "BuzzerControl.h"
#include "includes.h"

#define SILENCE            0
#define B_SUBOCTAVE3       494
#define F_SHARP_SUBOCTAVE1 1480
#define B_SUBOCTAVE1       1976
#define C_BASE             2093
#define C_SHARP_BASE       2217
#define D_BASE             2349
#define D_SHARP_BASE       2489
#define E_BASE             2637
#define F_BASE             2794
#define F_SHARP_BASE       2960
#define G_BASE             3136
#define G_SHARP_BASE       3322
#define A_BASE             3520
#define A_SHARP_BASE       3729
#define B_BASE             3951
#define C_OCTAVE1          4186
#define C_SHARP_OCTAVE1    4435
#define D_OCTAVE1          4699
#define D_SHARP_OCTAVE1    4978
#define E_OCTAVE1          5274
#define F_OCTAVE1          5588
#define F_SHARP_OCTAVE1    5920
#define G_OCTAVE1          6272
#define G_SHARP_OCTAVE1    6645
#define A_OCTAVE1          7040
#define A_SHARP_OCTAVE1    7459
#define B_OCTAVE1          7902
#define C_OCTAVE2          8372
#define C_SHARP_OCTAVE2    8870
#define D_OCTAVE2          9397
#define D_SHARP_OCTAVE2    9956
#define E_OCTAVE2          10548
#define F_OCTAVE2          11175
#define F_SHARP_OCTAVE2    11840
#define G_OCTAVE2          12544
#define G_SHARP_OCTAVE2    13290
#define A_OCTAVE2          14080
#define A_SHARP_OCTAVE2    14917
#define B_OCTAVE2          15804

#ifdef BUZZER_PIN

void Buzzer_play(SOUND sound)
{
  switch (sound)
  {
    case SOUND_OK:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(A_BASE, 50);
        Buzzer_AddSound(SILENCE, 40);
        Buzzer_AddSound(E_OCTAVE1, 50);
      }
      break;

    case SOUND_SUCCESS:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(A_BASE, 50);
        Buzzer_AddSound(SILENCE, 50);
        Buzzer_AddSound(A_BASE, 50);
        Buzzer_AddSound(SILENCE, 50);
        Buzzer_AddSound(A_BASE, 50);
      }
      break;

    case SOUND_CANCEL:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(E_OCTAVE1, 50);
        Buzzer_AddSound(SILENCE, 20);
        Buzzer_AddSound(A_BASE, 40);
      }
      break;

    case SOUND_NOTIFY:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(G_BASE, 50);
        Buzzer_AddSound(SILENCE, 50);
        Buzzer_AddSound(C_OCTAVE1, 50);
      }
      break;

    case SOUND_ERROR:
    {
      Buzzer_AddSound(C_SHARP_BASE, 200);
      Buzzer_AddSound(SILENCE, 60);
      Buzzer_AddSound(C_SHARP_BASE, 200);
      Buzzer_AddSound(SILENCE, 60);
      Buzzer_AddSound(C_SHARP_BASE, 200);
    }
    break;

    case SOUND_DENY:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(B_SUBOCTAVE3, 10);
        Buzzer_AddSound(E_OCTAVE2, 20);
      }
      break;

    case SOUND_TOAST:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_TOAST))
      {
        Buzzer_AddSound(F_SHARP_SUBOCTAVE1, 30);
        Buzzer_AddSound(B_SUBOCTAVE1, 30);
      }
      break;

    case SOUND_HEATED:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_HEATER))
      {
        Buzzer_AddSound(G_BASE, 50);
        Buzzer_AddSound(SILENCE, 50);
        Buzzer_AddSound(B_BASE, 50);
        Buzzer_AddSound(SILENCE, 100);
        Buzzer_AddSound(B_BASE, 50);
      }
      break;

    case SOUND_COOLED:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_HEATER))
      {
        Buzzer_AddSound(B_BASE, 50);
        Buzzer_AddSound(SILENCE, 50);
        Buzzer_AddSound(G_BASE, 50);
        Buzzer_AddSound(SILENCE, 100);
        Buzzer_AddSound(G_BASE, 50);
      }
      break;

    case SOUND_KEYPRESS:
    default:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_TOUCH))
        Buzzer_AddSound(BUZZER_FREQUENCY_HZ, BUZZER_FREQUENCY_DURATION_MS);
      break;
  }
} // Buzzer_play

#endif  // BUZZER_PIN
