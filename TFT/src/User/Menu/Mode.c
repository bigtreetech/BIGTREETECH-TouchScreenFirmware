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
  #ifdef CLEAN_MODE_SWITCHING_SUPPORT
    Serial_ReSourceInit();
  #endif
  infoMenu.cur = 0;
  switch(infoSettings.mode)
  {
    case SERIAL_TSC:
    {
      #ifndef CLEAN_MODE_SWITCHING_SUPPORT
        Serial_ReSourceInit();
      #endif

      #ifdef BUZZER_PIN
        Buzzer_Config();
      #endif
      GUI_SetColor(FONT_COLOR);
      GUI_SetBkColor(BACKGROUND_COLOR);

      #ifdef UNIFIED_MENU //if Unified menu is selected
        infoMenu.menu[infoMenu.cur] = menuStatus; //status screen as default screen on boot
      #else // classic UI
        infoMenu.menu[infoMenu.cur] = menuMain;
      #endif
      #ifdef SHOW_BTT_BOOTSCREEN
        if (freshboot)
        {
          u32 startUpTime = OS_GetTimeMs();
          heatSetUpdateTime(TEMPERATURE_QUERY_FAST_DURATION);
          LOGO_ReadDisplay();
          while (OS_GetTimeMs() - startUpTime < 3000) //Display 3s logo
          {
            loopProcess();
          }
          heatSetUpdateTime(TEMPERATURE_QUERY_SLOW_DURATION);
          freshboot = false;
        }
      #endif
      break;
    }

    #ifdef ST7920_SPI

    case LCD12864:

      #ifdef BUZZER_PIN
        Buzzer_DeConfig();  // Disable buzzer in LCD12864 Simulations mode.
      #endif

      #ifdef LED_COLOR_PIN
        #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE
          knob_LED_DeInit();
        #endif
      #endif
      GUI_SetColor(ST7920_FNCOLOR);
      GUI_SetBkColor(ST7920_BKCOLOR);
      infoMenu.menu[infoMenu.cur] = menuST7920;
      break;

    #endif
  }
}
