#include "levelingUBLSave.h"
#include "includes.h"

char* slot;
bool sLeveled = false;

const MENUITEMS UBLSave = {
// title
LABEL_ABL_SETTINGS_UBL_SAVE,
// icon                        label
 {{ICON_LEVELING,              LABEL_ABL_SLOT0},
  {ICON_LEVELING,              LABEL_ABL_SLOT1},
  {ICON_LEVELING,              LABEL_ABL_SLOT2},
  {ICON_LEVELING,              LABEL_ABL_SLOT3},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACK,                  LABEL_BACK},}
};

void menuUBLSave(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&UBLSave);
  while(infoMenu.menu[infoMenu.cur] == menuUBLSave)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        slot = "0";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLSave.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveConfirm, NULL, NULL);
        break;
      case KEY_ICON_1:
        slot = "1";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLSave.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveConfirm, NULL, NULL);
        break;
      case KEY_ICON_2:
        slot = "2";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLSave.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveConfirm, NULL, NULL);
        break;
      case KEY_ICON_3:
        slot = "3";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLSave.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveConfirm, NULL, NULL);
        break;
      case KEY_ICON_7:
        if (sLeveled == true && infoMachineSettings.EEPROM == 1)
        {
          sLeveled = false;
          showDialog(DIALOG_TYPE_QUESTION, textSelect(LABEL_ABL_SETTINGS_UBL), textSelect(LABEL_EEPROM_SAVE_INFO),
                     textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        else
        {
          infoMenu.cur--;
          sLeveled = false;
        }
        break;
      default:
        break;
    }
    loopProcess();
  }
}

void saveConfirm(void)
{
    storeCmd("G29 A\n");  // Ensure UBL is enabled
    storeCmd("G29 S%s\n", slot);
    sLeveled = true;
}