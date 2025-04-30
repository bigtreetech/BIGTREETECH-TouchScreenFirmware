#include "PrintRestore.h"
#include "includes.h"

void menuPrintRestore(void)
{
  uint16_t key_num = IDLE_TOUCH;

  GUI_Clear(infoSettings.bg_color);
  menuSetTitle(NULL);

  GUI_DispString((LCD_WIDTH - GUI_StrPixelWidth(LABEL_LOADING)) / 2, LCD_HEIGHT / 2 - BYTE_HEIGHT, LABEL_LOADING);

  if (mountFS() == true && powerFailedExist())  // powerFailedExist function sets both infoFile.path and PLR filename
  {
    // textSelect() can use a shared buffer (if a language package is being used)
    // so we need to buffer 2 of the 3 labels provided in popupDrawPage()
    LABEL_CHAR(confirm, LABEL_CONFIRM);
    LABEL_CHAR(cancel, LABEL_CANCEL);

    popupDrawPage(DIALOG_TYPE_QUESTION, (BUTTON *) dialogGetBottomDoubleBtn(), textSelect(LABEL_POWER_FAILED), infoFile.path,
                  confirm, cancel);

    while (MENU_IS(menuPrintRestore))
    {
      key_num = KEY_GetValue(2, dialogGetDoubleBtnRect());

      switch (key_num)
      {
        case KEY_POPUP_CONFIRM:
          powerFailedSetRestore(true);

          CLOSE_MENU();     // close the menu first

          startPrinting();  // start print and open Printing menu
          break;

        case KEY_POPUP_CANCEL:
          powerFailedSetRestore(false);
          powerFailedDelete();
          // in case the calling function is menuPrintFromSource,
          // remove the filename from path to allow the files scanning from its folder avoiding a scanning error message
          exitFolder();

          CLOSE_MENU();
          break;
      }

      #ifdef SD_CD_PIN
        if (volumeExists(infoFile.source) != true)
        {
          resetInfoFile();

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
