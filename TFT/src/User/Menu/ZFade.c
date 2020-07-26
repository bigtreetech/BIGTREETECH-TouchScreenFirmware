#include "ZFade.h"
#include "includes.h"

#define Z_FADE_MIN_VALUE     0.0f
#define Z_FADE_MAX_VALUE     20.0f
#define Z_FADE_DEFAULT_VALUE 10.0f

void showZFade(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void menuZFade(void)
{
  // title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS ZFadeItems = {
  // title
  LABEL_ABL_Z,
  // icon                       label
   {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_RESET_VALUE,          LABEL_RESET},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  float Z_Fade_value;
  float now = Z_Fade_value = getParameter(P_ABL_STATE, 1);

  menuDrawPage(&ZFadeItems);
  showZFade(now);
  bool changed = false;

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuZFade)
  {
    Z_Fade_value = getParameter(P_ABL_STATE, 1);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease offset
      case KEY_ICON_0:
        if (Z_Fade_value > Z_FADE_MIN_VALUE)
        {
          storeCmd("M420 Z%.2f\n", Z_Fade_value-1);
          changed = true;
        }
        break;

      // increase offset
      case KEY_ICON_3:
        if (Z_Fade_value < Z_FADE_MAX_VALUE)
        {
          storeCmd("M420 Z%.2f\n", Z_Fade_value+1);
          changed = true;
        }
        break;

      case KEY_ICON_6:
          storeCmd("M420 Z0\n");
        break;

      case KEY_ICON_7:
        if (changed == true && infoMachineSettings.EEPROM == 1)
        {
          showDialog(DIALOG_TYPE_QUESTION, textSelect(ZFadeItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                     textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        else
        {
          infoMenu.cur--;
        }
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition > 0 && Z_Fade_value < Z_FADE_MAX_VALUE) {
            storeCmd("M420 Z%.2f\n", Z_Fade_value + encoderPosition);
            Z_Fade_value += encoderPosition;
            encoderPosition = 0;
          } else if (encoderPosition < 0 && Z_Fade_value > Z_FADE_MIN_VALUE) {
            storeCmd("M420 Z%.2f\n", Z_Fade_value + encoderPosition);
            Z_Fade_value += encoderPosition;
            encoderPosition = 0;
          } else encoderPosition = 0;
        #endif
        break;
    }

    if (now != Z_Fade_value)
    {
      now = Z_Fade_value;
      showZFade(now);
    }

    loopProcess();
  }
}
