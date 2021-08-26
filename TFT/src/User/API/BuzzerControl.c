#include "BuzzerControl.h"
#include "includes.h"

#ifdef BUZZER_PIN

void Buzzer_play(SOUND sound)
{
  // if (infoSettings.allSound != 1)
    // return;
  switch (sound)
  {
    case sound_ok:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(3500, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(5500, 50);
      break;

    case sound_success:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(3500, 50);
      Buzzer_TurnOn(0, 50);
      Buzzer_TurnOn(3500, 50);
      Buzzer_TurnOn(0, 50);
      Buzzer_TurnOn(3500, 50);
      break;

    case sound_cancel:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(5500, 50);
      Buzzer_TurnOn(0, 20);
      Buzzer_TurnOn(3500, 40);
      break;

    case sound_notify:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(3090, 50);
      Buzzer_TurnOn(0, 50);
      Buzzer_TurnOn(4190, 50);
      break;

    case sound_error:
      Buzzer_TurnOn(2200, 200);
      Buzzer_TurnOn(0, 60);
      Buzzer_TurnOn(2200, 200);
      Buzzer_TurnOn(0, 60);
      Buzzer_TurnOn(2200, 200);
      break;

    case sound_deny:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(500, 10);
      Buzzer_TurnOn(10000, 20);
      break;

    case sound_toast:
      if (infoSettings.toastSound != 1)
        break;
      Buzzer_TurnOn(1500, 30);
      Buzzer_TurnOn(2000, 30);
      break;

    case sound_heated:
      if (infoSettings.heaterSound != 1)
        break;
      Buzzer_TurnOn(3136, 50);
      Buzzer_TurnOn(0, 50);
      Buzzer_TurnOn(3951, 50);
      Buzzer_TurnOn(0, 100);
      Buzzer_TurnOn(3951, 50);
      break;

    case sound_cooled:
      if (infoSettings.heaterSound != 1)
        break;
      Buzzer_TurnOn(3951, 50);
      Buzzer_TurnOn(0, 50);
      Buzzer_TurnOn(3136, 50);
      Buzzer_TurnOn(0, 100);
      Buzzer_TurnOn(3136, 50);
      break;

    case sound_keypress:
    default:
      if (infoSettings.touchSound != 1)
        break;
      Buzzer_TurnOn(BUZZER_FREQUENCY_HZ, BUZZER_FREQUENCY_DURATION_MS);
      break;
  }
}

#endif  // BUZZER_PIN
