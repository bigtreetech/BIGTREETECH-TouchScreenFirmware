#include "ZFade.h"
#include "includes.h"

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
  const MENUITEMS ZFadeItems = {
  // title
  LABEL_ABL_Z,
  // icon                       label
   {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_EEPROM_SAVE,          LABEL_SAVE},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_RESET_VALUE,          LABEL_RESET},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  float Z_Fade_value;
  float now = Z_Fade_value = getParameter(P_ABL_STATE, 1);

  _ECHO_POPUP_TYPE pt1, pt2;
  bool             be1, be2;

  menuDrawPage(&ZFadeItems);
  showZFade(now);

  // temporary disable the following ACK notifications to avoid
  // these notifications popup every time Z fade value is changed
  getKnownEchoParam(ECHO_ID_BED_LEVELING, &pt1, &be1);
  getKnownEchoParam(ECHO_ID_FADE_HEIGHT, &pt2, &be2);

  setKnownEchoParam(ECHO_ID_BED_LEVELING, ECHO_POPUP_NONE, false);
  setKnownEchoParam(ECHO_ID_FADE_HEIGHT, ECHO_POPUP_NONE, false);

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
        }
        break;

      // increase offset
      case KEY_ICON_3:
        if (Z_Fade_value < Z_FADE_MAX_VALUE)
        {
          storeCmd("M420 Z%.2f\n", Z_Fade_value+1);
        }
        break;

      // save to EEPROM
      case KEY_ICON_4:
        if (infoMachineSettings.EEPROM == 1)
        {
          showDialog(DIALOG_TYPE_QUESTION, textSelect(ZFadeItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
            textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        break;

      // reset offset
      case KEY_ICON_6:
        storeCmd("M420 Z0\n");
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition > 0 && Z_Fade_value < Z_FADE_MAX_VALUE)
          {
            storeCmd("M420 Z%.2f\n", Z_Fade_value + encoderPosition);

            Z_Fade_value += encoderPosition;
            encoderPosition = 0;
          }
          else if (encoderPosition < 0 && Z_Fade_value > Z_FADE_MIN_VALUE)
          {
            storeCmd("M420 Z%.2f\n", Z_Fade_value + encoderPosition);

            Z_Fade_value += encoderPosition;
            encoderPosition = 0;
          }
          else
            encoderPosition = 0;
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

  // restore ACK notifications
  setKnownEchoParam(ECHO_ID_BED_LEVELING, pt1, be1);
  setKnownEchoParam(ECHO_ID_FADE_HEIGHT, pt2, be2);
}
