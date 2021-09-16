#include "ModeSwitching.h"
#include "includes.h"

bool modeFreshBoot = true;
bool modeSwitching = false;

// change UI mode
void Mode_Switch(void)
{
  int8_t nowMode = GET_BIT(infoSettings.mode, 0);  // Marlin mode or Touch mode
  infoMenu.cur = 0;

  HW_InitMode(nowMode);

  switch (nowMode)
  {
    case MODE_SERIAL_TSC:
      GUI_RestoreColorDefault();

      // always init the machine settings to restart the temperature polling
      // process needed by parseAck() function to establish the connection
      initMachineSettings();

      if (infoSettings.status_screen == 1)  // if Status Screen menu is selected
        infoMenu.menu[infoMenu.cur] = menuStatus;  // status screen as default home screen on boot
      else
        infoMenu.menu[infoMenu.cur] = menuMain;  // classic UI

      #ifdef SHOW_BTT_BOOTSCREEN
        if (modeFreshBoot)
        {
          uint32_t startUpTime = OS_GetTimeMs();

          heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
          LOGO_ReadDisplay();
          updateNextHeatCheckTime();  // send "M105" after a delay, because of mega2560 will be hanged when received data at startup

          while (OS_GetTimeMs() - startUpTime < BTT_BOOTSCREEN_TIME)  // display logo BTT_BOOTSCREEN_TIME ms
          {
            loopProcess();
          }

          heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
          modeFreshBoot = false;
        }
      #endif
      break;

    case MODE_MARLIN:
      #ifdef HAS_EMULATOR
        if (infoSettings.serial_always_on == ENABLED)
          updateNextHeatCheckTime();  // send "M105" after a delay, because of mega2560 will be hanged when received data at startup

        infoMenu.menu[infoMenu.cur] = menuMarlinMode;
      #endif
      break;
  }
}

void Mode_CheckSwitching(void)
{
//  #ifndef SERIAL_ALWAYS_ON
  // IDEALLY I would like to be able to swap even when the TFT is in printing mode
  // but before I can allow that I need a way to make sure that we swap back into
  // the right mode (and correct screen) and I really want a reliable way to DETECT
  // that the TFT should be in printing mode even when the print was started externally.
  if (isPrinting() || infoHost.printing || modeSwitching)
    return;

  if (infoMenu.menu[infoMenu.cur] == menuMode)
    return;
//  #endif

  if (Touch_Enc_ReadPen(MODE_SWITCHING_INTERVAL)
    #if LCD_ENCODER_SUPPORT
      || LCD_Enc_ReadBtn(MODE_SWITCHING_INTERVAL)
    #endif
    )
  {
    // NOTE: leave this check after Touch_Enc_ReadPen() to allow the Smart Home feature to work properly
    if (infoSettings.mode >= MODE_COUNT)  // if blocked mode, then exit
      return;

    infoMenu.menu[++infoMenu.cur] = menuMode;
  }
}
