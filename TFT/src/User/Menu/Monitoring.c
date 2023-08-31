#include "Monitoring.h"
#include "includes.h"

void monitoringDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, 0, NULL, NULL, &monitoringDrawMenu);

  // clear screen
  GUI_Clear(infoSettings.bg_color);

  GUI_SetColor(WHITE);

  // draw titles
  GUI_DispString(0, 2 * BYTE_HEIGHT, (uint8_t *)"Buffered gcodes : ");
  GUI_DispString(0, 4 * BYTE_HEIGHT, (uint8_t *)"Pending gcodes  : ");
  GUI_DispString(0, 6 * BYTE_HEIGHT, (uint8_t *)"Free TX slots   : ");

  // draw bottom line and text
  GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH);
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT * 2), LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));
}

void menuMonitoring(void)
{
  char str[30];

  const GUI_RECT fullRect = {0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1};

  monitoringDrawMenu();

  while (MENU_IS(menuMonitoring))
  {
    if (nextScreenUpdate(1000))
    {
      // draw info
      GUI_SetColor(YELLOW);

      sprintf(str, "%d   ", getQueueCount());
      GUI_DispString(18 * BYTE_WIDTH, 2 * BYTE_HEIGHT, (uint8_t *)str);

      sprintf(str, "%d   ", infoHost.tx_count);
      GUI_DispString(18 * BYTE_WIDTH, 4 * BYTE_HEIGHT, (uint8_t *)str);

      sprintf(str, "%d   ", infoHost.tx_slots);
      GUI_DispString(18 * BYTE_WIDTH, 6 * BYTE_HEIGHT, (uint8_t *)str);
    }

    loopProcess();

    if (KEY_GetValue(1, &fullRect) == 0)  // if a key was pressed and then released, exit from menu
      break;
  }

  GUI_RestoreColorDefault();

  CLOSE_MENU();
}
