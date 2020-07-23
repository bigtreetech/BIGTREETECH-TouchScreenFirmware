#include "levelingUBLLoad.h"
#include "includes.h"

char* slot;
bool lLeveled = false;

const MENUITEMS UBLLoad = {
// title
LABEL_ABL_SETTINGS_UBL_LOAD,
// icon                        label
 {{ICON_S_LOAD,                LABEL_ABL_SLOT0},
  {ICON_S_LOAD,                LABEL_ABL_SLOT1},
  {ICON_S_LOAD,                LABEL_ABL_SLOT2},
  {ICON_S_LOAD,                LABEL_ABL_SLOT3},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACKGROUND,            LABEL_BACKGROUND},
  {ICON_BACK,                  LABEL_BACK},}
};

void menuUBLLoad(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&UBLLoad);
  while(infoMenu.menu[infoMenu.cur] == menuUBLLoad)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        slot = "0";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLLoad.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), loadConfirm, NULL, NULL);
        break;
      case KEY_ICON_1:
        slot = "1";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLLoad.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), loadConfirm, NULL, NULL);
        break;
      case KEY_ICON_2:
        slot = "2";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLLoad.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), loadConfirm, NULL, NULL);
        break;
      case KEY_ICON_3:
        slot = "3";
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLLoad.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), loadConfirm, NULL, NULL);
        break;
      case KEY_ICON_7:
        if (lLeveled == true && infoMachineSettings.EEPROM == 1)
        {
          lLeveled = false;
          showDialog(DIALOG_TYPE_QUESTION, textSelect(LABEL_ABL_SETTINGS_UBL), textSelect(LABEL_ABL_SLOT_EEPROM ),
                     textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        else
        {
          infoMenu.cur--;
          lLeveled = false;
        }
        break;
      default:
        break;
    }
    loopProcess();
  }
}

void loadConfirm(void)
{
    storeCmd("G29 L%s\n", slot);
    lLeveled = true;
}
