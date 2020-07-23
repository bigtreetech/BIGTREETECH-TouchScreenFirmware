#include "levelingUBL.h"
#include "includes.h"

const MENUITEMS autoLevelingItemsUBL = {
// title
LABEL_ABL_SETTINGS_UBL,
// icon                        label
 {{ICON_LEVELING,              LABEL_ABL_START},
  {ICON_S_SAVE,                LABEL_EEPROM_SAVE},
  {ICON_S_LOAD,                LABEL_LOAD},
  {ICON_Z_FADE,                LABEL_ABL_Z},
  {ICON_PROBE_OFFSET,          LABEL_Z_OFFSET},
  {ICON_BABYSTEP,              LABEL_BABYSTEP},
  {ICON_BLTOUCH,               LABEL_BLTOUCH},
  {ICON_BACK,                  LABEL_BACK},}
};

void menuAutoLevelingUBL(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&autoLevelingItemsUBL);
  while(infoMenu.menu[infoMenu.cur] == menuAutoLevelingUBL)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        // UBL
        storeCmd("G28\n");
        storeCmd("G29 P1\n");
        storeCmd("G29 P3\n");
        storeCmd("M118 A1 UBL Complete\n");
        break;
      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuUBLSave;
        break;
      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuUBLLoad;
        break;
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuZFade;
      break;
      case KEY_ICON_4:
        storeCmd("M851\n");
        infoMenu.menu[++infoMenu.cur] = menuProbeOffset;
        break;
      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuBabyStep;
        break;
      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuBLTouch;
        break;
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      default:
        break;
    }
    loopProcess();
  }
}

void SaveEepromPrompt(void) {
  if (infoMachineSettings.EEPROM == 1)
  {
    Delay_ms(100);
    showDialog(DIALOG_TYPE_QUESTION, textSelect(autoLevelingItemsUBL.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
              textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
  }
}
