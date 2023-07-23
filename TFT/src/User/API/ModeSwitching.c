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
        REPLACE_MENU(menuStatus);  // status screen as default home screen on boot
      else
        REPLACE_MENU(menuMain);  // classic UI

      #ifdef SHOW_BTT_BOOTSCREEN
        if (modeFreshBoot)
        {
          uint32_t startUpTime = OS_GetTimeMs();

          heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
          LOGO_ReadDisplay();
          updateNextHeatCheckTime();  // send "M105" after a delay, because of mega2560 will be hanged when received data at startup

          TASK_LOOP_WHILE(OS_GetTimeMs() - startUpTime < BTT_BOOTSCREEN_TIME);  // display logo BTT_BOOTSCREEN_TIME ms

          heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
          modeFreshBoot = false;
        }
      #endif
      break;

    case MODE_MARLIN:
      #ifdef HAS_EMULATOR
        if (infoSettings.serial_always_on == ENABLED)
          updateNextHeatCheckTime();  // send "M105" after a delay, because of mega2560 will be hanged when received data at startup

        REPLACE_MENU(menuMarlinMode);
      #endif
      break;
  }
}

void Mode_CheckSwitching(void)
{
  // we must always call Touch_Enc_ReadPen() and LCD_Enc_ReadBtn() first just to always update their internal timers
  // when an encoder/touch button is pressed or released (avoiding the Mode menu is displayed by mistake when aborting
  // a print in Marlin mode with infoSettings.serial_always_on enabled)
  bool btnPressed = Touch_Enc_ReadPen(MODE_SWITCHING_INTERVAL);

  #if LCD_ENCODER_SUPPORT
    btnPressed |= LCD_Enc_ReadBtn(MODE_SWITCHING_INTERVAL);
  #endif

  // NOTE: leave this check after Touch_Enc_ReadPen() was called to allow the Smart Home feature to work properly
  //       when infoSettings.mode is set to a blocked mode
  if (infoSettings.mode >= MODE_COUNT)  // if blocked mode, then exit
    return;

  // do not change mode if printing from any source or is already waiting mode selection
  if (isPrinting() || isPrintingFromOnboard() || modeSwitching)
    return;

  if (MENU_IS(menuMode))
    return;

  if (btnPressed)
    OPEN_MENU(menuMode);
}
