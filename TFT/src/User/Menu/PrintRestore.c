#include "PrintRestore.h"
#include "includes.h"

void menuPrintRestore(void)
{
  uint16_t key_num = IDLE_TOUCH;

  powerFailedClear();
  GUI_Clear(infoSettings.bg_color);

  GUI_DispString((LCD_WIDTH - GUI_StrPixelWidth(LABEL_LOADING)) / 2, LCD_HEIGHT / 2 - BYTE_HEIGHT, LABEL_LOADING);

  if (mountFS() == true && powerFailedExist())
  {
    char okTxt[MAX_LANG_LABEL_LENGTH];
    char cancelTxt[MAX_LANG_LABEL_LENGTH];
    loadLabelText((uint8_t*)okTxt, LABEL_CONFIRM);
    loadLabelText((uint8_t*)cancelTxt, LABEL_CANCEL);

    popupDrawPage(DIALOG_TYPE_QUESTION, bottomDoubleBtn, textSelect(LABEL_POWER_FAILED), (uint8_t* )infoFile.title,
                  (uint8_t*)okTxt, (uint8_t*)cancelTxt);

    while (MENU_IS(menuPrintRestore))
    {
      key_num = KEY_GetValue(2, doubleBtnRect);
      switch (key_num)
      {
        case KEY_POPUP_CONFIRM:
          powerFailedGetData();
          infoMenu.menu[1] = menuPrintFromSource;
          infoMenu.menu[2] = menuBeforePrinting;
          infoMenu.cur = 2;
          break;

        case KEY_POPUP_CANCEL:
          powerFailedDelete();
          ExitDir();
          CLOSE_MENU();
          break;
      }

      #ifdef SD_CD_PIN
        if (volumeExists(infoFile.source) != true)
        {
          resetInfoFile();
          powerFailedClear();
          CLOSE_MENU();
        }
      #endif

      loopProcess();
    }
  }
  else
  {
    CLOSE_MENU();
  }
}
