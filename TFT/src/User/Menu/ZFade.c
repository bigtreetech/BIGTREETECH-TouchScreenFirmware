#include "ZFade.h"
#include "includes.h"

#define Z_FADE_MIN_VALUE     0.0f
#define Z_FADE_MAX_VALUE     20.0f
#define Z_FADE_DEFAULT_VALUE 10.0f

//#define ITEM_PROBE_OFFSET_UNIT_NUM 3

//const ITEM itemProbeOffsetUnit[ITEM_PROBE_OFFSET_UNIT_NUM] = {
// icon                       label
//  {ICON_001_MM,               LABEL_001_MM},
//  {ICON_01_MM,                LABEL_01_MM},
//  {ICON_1_MM,                 LABEL_1_MM},
//};

//const float probeOffset_unit[ITEM_PROBE_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

//static u8 curUnit = 0;

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
    {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  float Z_Fade_value;
  float now = Z_Fade_value = getParameter(P_ABL_STATE, 1);

  menuDrawPage(&ZFadeItems);
  showZFade(now);

//#if LCD_ENCODER_SUPPORT
//  encoderPosition = 0;
//#endif

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
          //setParameter(P_ABL_STATE,1,Z_Fade_value-1);
          storeCmd("M420 Z%.2f\n", Z_Fade_value-1);
        }
        break;

      // increase offset
      case KEY_ICON_3:
        if (Z_Fade_value < Z_FADE_MAX_VALUE)
        {
          //setParameter(P_ABL_STATE,1,Z_Fade_value+1);
          storeCmd("M420 Z%.2f\n", Z_Fade_value+1);
        }
        break;

      //save to eeprom
      case KEY_ICON_4:
        if (infoMachineSettings.EEPROM == 1)
        {
          //storeCmd("M500\n");
          SaveEepromPrompt();
        }
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        //#if LCD_ENCODER_SUPPORT
        //  if (encoderPosition)
        //  {
        //    storeCmd("M851 Z%.2f\n", Z_Fade_value + probeOffset_unit[curUnit] * encoderPosition);
        //    Z_Fade_value += probeOffset_unit[curUnit] * encoderPosition;
        //    encoderPosition = 0;
        //  }
        //#endif
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
