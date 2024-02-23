#include "BuzzerControl.h"
#include "includes.h"

#ifdef BUZZER_PIN

#define SILENCE              0

#define C_SUBOCTAVE6         33
#define C_SHARP_SUBOCTAVE6   35
#define D_SUBOCTAVE6         37
#define D_SHARP_SUBOCTAVE6   39
#define E_SUBOCTAVE6         41
#define F_SUBOCTAVE6         44
#define F_SHARP_SUBOCTAVE6   46
#define G_SUBOCTAVE6         49
#define G_SHARP_SUBOCTAVE6   52
#define A_SUBOCTAVE6         55
#define A_SHARP_SUBOCTAVE6   58
#define B_SUBOCTAVE6         62

#define C_SUBOCTAVE5         65
#define C_SHARP_SUBOCTAVE5   69
#define D_SUBOCTAVE5         73
#define D_SHARP_SUBOCTAVE5   78
#define E_SUBOCTAVE5         82
#define F_SUBOCTAVE5         87
#define F_SHARP_SUBOCTAVE5   93
#define G_SUBOCTAVE5         98
#define G_SHARP_SUBOCTAVE5  104
#define A_SUBOCTAVE5        110
#define A_SHARP_SUBOCTAVE5  117
#define B_SUBOCTAVE5        123

#define C_SUBOCTAVE4        131
#define C_SHARP_SUBOCTAVE4  139
#define D_SUBOCTAVE4        147
#define D_SHARP_SUBOCTAVE4  156
#define E_SUBOCTAVE4        165
#define F_SUBOCTAVE4        175
#define F_SHARP_SUBOCTAVE4  185
#define G_SUBOCTAVE4        196
#define G_SHARP_SUBOCTAVE4  208
#define A_SUBOCTAVE4        220
#define A_SHARP_SUBOCTAVE4  233
#define B_SUBOCTAVE4        247

#define C_SUBOCTAVE3        262
#define C_SHARP_SUBOCTAVE3  277
#define D_SUBOCTAVE3        294
#define D_SHARP_SUBOCTAVE3  311
#define E_SUBOCTAVE3        330
#define F_SUBOCTAVE3        349
#define F_SHARP_SUBOCTAVE3  370
#define G_SUBOCTAVE3        392
#define G_SHARP_SUBOCTAVE3  415
#define A_SUBOCTAVE3        440
#define A_SHARP_SUBOCTAVE3  466
#define B_SUBOCTAVE3        494

#define C_SUBOCTAVE2        523
#define C_SHARP_SUBOCTAVE2  554
#define D_SUBOCTAVE2        587
#define D_SHARP_SUBOCTAVE2  622
#define E_SUBOCTAVE2        659
#define F_SUBOCTAVE2        698
#define F_SHARP_SUBOCTAVE2  740
#define G_SUBOCTAVE2        784
#define G_SHARP_SUBOCTAVE2  831
#define A_SUBOCTAVE2        880
#define A_SHARP_SUBOCTAVE2  932
#define B_SUBOCTAVE2        988

#define C_SUBOCTAVE1       1047
#define C_SHARP_SUBOCTAVE1 1109
#define D_SUBOCTAVE1       1175
#define D_SHARP_SUBOCTAVE1 1245
#define E_SUBOCTAVE1       1319
#define F_SUBOCTAVE1       1397
#define F_SHARP_SUBOCTAVE1 1480
#define G_SUBOCTAVE1       1568
#define G_SHARP_SUBOCTAVE1 1661
#define A_SUBOCTAVE1       1760
#define A_SHARP_SUBOCTAVE1 1865
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

#define NO_SOUNDS_SET 0xFF  // flag to indicate no sound set

static uint8_t origSounds = NO_SOUNDS_SET;

void Buzzer_HandleMute(bool mute)
{
  if (mute && origSounds == NO_SOUNDS_SET)  // if mute is requested and sounds not already muted, mute all sounds
  {
    origSounds = infoSettings.sounds;                                     // backup current sounds setting
    infoSettings.sounds = infoSettings.sounds & (1 << SOUND_TYPE_TOUCH);  // mute all sounds with the exception of touch sound if enabled
  }
  else if (!mute && origSounds != NO_SOUNDS_SET)  // if unmute is requested and sounds not already unmuted, restore sounds setting
  {
    infoSettings.sounds = origSounds;  // restore original sounds setting
    origSounds = NO_SOUNDS_SET;        // reset flag
  }
}

void Buzzer_Play(SOUND sound)
{
  switch (sound)
  {
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

    case SOUND_ERROR:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(C_SHARP_BASE, 200);
        Buzzer_AddSound(SILENCE, 60);
        Buzzer_AddSound(C_SHARP_BASE, 200);
        Buzzer_AddSound(SILENCE, 60);
        Buzzer_AddSound(C_SHARP_BASE, 200);
      }
      break;

    case SOUND_OK:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(G_BASE, 50);
        Buzzer_AddSound(SILENCE, 40);
        Buzzer_AddSound(C_OCTAVE1, 50);
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

    case SOUND_DENY:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_AddSound(B_SUBOCTAVE3, 10);
        Buzzer_AddSound(B_OCTAVE1, 20);
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
}

#endif  // BUZZER_PIN
