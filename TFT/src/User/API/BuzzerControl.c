#include "BuzzerControl.h"
#include "includes.h"

#ifdef BUZZER_PIN

void Buzzer_play(SOUND sound)
{
  // if (infoSettings.allSound != 1)
    // return;
  switch (sound)
  {
    case SOUND_OK:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_TurnOn(3500, 50);
        Buzzer_TurnOn(0, 40);
        Buzzer_TurnOn(5500, 50);
      }
      break;

    case SOUND_SUCCESS:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_TurnOn(3500, 50);
        Buzzer_TurnOn(0, 50);
        Buzzer_TurnOn(3500, 50);
        Buzzer_TurnOn(0, 50);
        Buzzer_TurnOn(3500, 50);
      }
      break;

    case SOUND_CANCEL:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_TurnOn(5500, 50);
        Buzzer_TurnOn(0, 20);
        Buzzer_TurnOn(3500, 40);
      }
      break;

    case SOUND_NOTIFY:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_TurnOn(3090, 50);
        Buzzer_TurnOn(0, 50);
        Buzzer_TurnOn(4190, 50);
      }
      break;

    case SOUND_ERROR:
    {
      Buzzer_TurnOn(2200, 200);
      Buzzer_TurnOn(0, 60);
      Buzzer_TurnOn(2200, 200);
      Buzzer_TurnOn(0, 60);
      Buzzer_TurnOn(2200, 200);
    }
    break;

    case SOUND_DENY:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_ALERT))
      {
        Buzzer_TurnOn(500, 10);
        Buzzer_TurnOn(10000, 20);
      }
      break;

    case SOUND_TOAST:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_TOAST))
      {
        Buzzer_TurnOn(1500, 30);
        Buzzer_TurnOn(2000, 30);
      }
      break;

    case SOUND_HEATED:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_HEATER))
      {
        Buzzer_TurnOn(3136, 50);
        Buzzer_TurnOn(0, 50);
        Buzzer_TurnOn(3951, 50);
        Buzzer_TurnOn(0, 100);
        Buzzer_TurnOn(3951, 50);
      }
      break;

    case SOUND_COOLED:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_HEATER))
      {
        Buzzer_TurnOn(3951, 50);
        Buzzer_TurnOn(0, 50);
        Buzzer_TurnOn(3136, 50);
        Buzzer_TurnOn(0, 100);
        Buzzer_TurnOn(3136, 50);
      }
      break;

    case SOUND_KEYPRESS:
    default:
      if (GET_BIT(infoSettings.sounds, SOUND_TYPE_TOUCH))
        Buzzer_TurnOn(BUZZER_FREQUENCY_HZ, BUZZER_FREQUENCY_DURATION_MS);
      break;
  }
} // Buzzer_play

#endif  // BUZZER_PIN
