#include "Mode.h"
#include "includes.h"

bool serialHasBeenInitialized = false;
bool freshboot = true;

void Serial_ReSourceDeInit(void)
{
  if (!serialHasBeenInitialized) return;
  serialHasBeenInitialized = false;
  Serial_DeInit();
}

void Serial_ReSourceInit(void)
{
  if (serialHasBeenInitialized) return;
  serialHasBeenInitialized = true;

  memset(&infoHost, 0, sizeof(infoHost));
  reminderSetUnConnected(); // reset connect status
  Serial_Init(infoSettings.baudrate);
}

void infoMenuSelect(void)
{
  infoMenu.cur = 0;
  switch(infoSettings.mode)
  {
    case SERIAL_TSC:
    {
      Serial_ReSourceInit();
      #ifdef BUZZER_PIN
        Buzzer_Config();
      #endif

      #ifdef LED_COLOR_PIN
        #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE
          knob_LED_Init():
        #endif
      #endif

      #if ENC_ACTIVE_SIGNAL
        setEncActiveSignal(0);
      #endif
      GUI_RestoreColorDefault();
      if(infoSettings.unified_menu == 1) //if Unified menu is selected
        infoMenu.menu[infoMenu.cur] = menuStatus; //status screen as default screen on boot
      else
        infoMenu.menu[infoMenu.cur] = classicMenu;   // classic UI

      #ifdef SHOW_BTT_BOOTSCREEN
        if (freshboot)
        {
          u32 startUpTime = OS_GetTimeMs();
          heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
          LOGO_ReadDisplay();
          updateNextHeatCheckTime(); // send "M105" 1s later not now, because of mega2560 will be hanged when received data at startup
          while (OS_GetTimeMs() - startUpTime < 3000) //Display 3s logo
          {
            loopProcess();
          }
          heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
          freshboot = false;
        }
      #endif
      break;
    }

    case MARLIN:
      #if ENC_ACTIVE_SIGNAL
        setEncActiveSignal(1);
      #endif
      if (infoSettings.serial_alwaysOn == 1)
      {
        Serial_ReSourceInit();
      }
      #ifdef BUZZER_PIN
        Buzzer_DeConfig();  // Disable buzzer in LCD12864 Simulations mode.
      #endif

      #ifdef LED_COLOR_PIN
        #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE
          knob_LED_DeInit();
        #endif
      #endif

      #if defined(ST7920_SPI) || defined(LCD2004_simulator)
        infoMenu.menu[infoMenu.cur] = menuMarlinMode;
      #endif
      break;
  }
}
