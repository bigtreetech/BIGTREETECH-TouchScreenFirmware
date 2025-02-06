#include "Monitoring.h"
#include "includes.h"

#ifdef DEBUG_MONITORING

MONITORING infoMonitoring;

// NOTE: if COMMAND_CHECKSUM feature is enabled then the total size of each command below is given by
//       the size of the plain command plus the command checksum overhead
static const char * const cmd[] = {
  "M220\n",                                                                                                // 6 chars including '\0'
  //"M221\n",                                                                                                // 6 chars including '\0'
  //"M114 E\n",                                                                                              // 8 chars including '\0'
  "M118 P0 test with short text\n",                                                                        // 30 chars including '\0'
  "M118 P0 test with medium text text text text text\n",                                                   // 51 chars including '\0'
  //"M118 P0 test with long text text text text text text text text text text\n",                            // 74 chars including '\0'
  //"M118 P0 test with very long text text text text text text text text text text text text text text \n",  // 100 chars including '\0'
  //"M43\n",                                                                                                 // 5 chars including '\0'
};

static bool stressTestMenu = false;

void monitoringSetMenu(bool stressTest)
{
  stressTestMenu = stressTest;
}

void menuMonitoring(void)
{
  const GUI_RECT fullRect = {0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1};
  uint8_t cmdChecksum = GET_BIT(infoSettings.general_settings, INDEX_COMMAND_CHECKSUM);  // save the configured COMMAND_CHECKSUM feature status
  uint16_t curCmdIndex = 0;
  char str[30];

  // clear screen
  GUI_Clear(infoSettings.bg_color);

  setMenu(MENU_TYPE_OTHER, NULL, 1, &fullRect, NULL, NULL);
  menuDrawTitle();

  // draw titles
  GUI_DispString(0, ICON_START_Y,                         (uint8_t *) "Buffered gcodes : ");
  GUI_DispString(0, ICON_START_Y + 1 * (BYTE_HEIGHT + 4), (uint8_t *) "Pending gcodes  : ");
  GUI_DispString(0, ICON_START_Y + 2 * (BYTE_HEIGHT + 4), (uint8_t *) "Free TX slots   : ");
  GUI_DispString(0, ICON_START_Y + 3 * (BYTE_HEIGHT + 4), (uint8_t *) "TX gcodes/bytes : ");
  GUI_DispString(0, ICON_START_Y + 4 * (BYTE_HEIGHT + 4), (uint8_t *) "RX acks/bytes   : ");
  GUI_DispString(0, ICON_START_Y + 5 * (BYTE_HEIGHT + 4), (uint8_t *) "Scan rate       : ");

  // draw bottom line and text
  GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH);
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT * 2), LCD_WIDTH - 20, LCD_HEIGHT, (uint8_t *) textSelect(LABEL_TOUCH_TO_EXIT));

  if (stressTestMenu)
    SET_BIT_ON(infoSettings.general_settings, INDEX_COMMAND_CHECKSUM);  // temporary enable COMMAND_CHECKSUM feature, if not already enabled

  while (MENU_IS(menuMonitoring))
  {
    if (KEY_GetValue(1, &fullRect) == 0)  // if a key was pressed and then released, exit from menu
      CLOSE_MENU();

    if (nextScreenUpdate(1000))
    {
      // draw info
      GUI_SetColor(infoSettings.status_color);

      sprintf(str, "%d   ", getCmdQueueCount());
      GUI_DispString(18 * BYTE_WIDTH, ICON_START_Y,                         (uint8_t *) str);

      sprintf(str, "%d   ", infoHost.tx_count);
      GUI_DispString(18 * BYTE_WIDTH, ICON_START_Y + 1 * (BYTE_HEIGHT + 4), (uint8_t *) str);

      sprintf(str, "%d   ", infoHost.tx_slots);
      GUI_DispString(18 * BYTE_WIDTH, ICON_START_Y + 2 * (BYTE_HEIGHT + 4), (uint8_t *) str);

      sprintf(str, "%d/%d      ", infoMonitoring.tx_cmd_rate, infoMonitoring.tx_bytes_rate);
      GUI_DispString(18 * BYTE_WIDTH, ICON_START_Y + 3 * (BYTE_HEIGHT + 4), (uint8_t *) str);

      sprintf(str, "%d/%d      ", infoMonitoring.rx_ack_rate, infoMonitoring.rx_bytes_rate);
      GUI_DispString(18 * BYTE_WIDTH, ICON_START_Y + 4 * (BYTE_HEIGHT + 4), (uint8_t *) str);

      sprintf(str, "%d   ", infoMonitoring.scan_rate_per_second);
      GUI_DispString(18 * BYTE_WIDTH, ICON_START_Y + 5 * (BYTE_HEIGHT + 4), (uint8_t *) str);

      GUI_RestoreColorDefault();
    }

    if (stressTestMenu && !isFullCmdQueue())
    {
      mustStoreCmd(cmd[curCmdIndex++]);  // send command and increment command index

      if (curCmdIndex >= COUNT(cmd))
        curCmdIndex = 0;
    }

    loopProcess();
  }

  if (stressTestMenu)
    SET_BIT_VALUE(infoSettings.general_settings, INDEX_COMMAND_CHECKSUM, cmdChecksum);  // restore the configured COMMAND_CHECKSUM feature status
}

#endif  // DEBUG_MONITORING
