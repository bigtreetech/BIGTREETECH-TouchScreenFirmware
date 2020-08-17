#include "UBLSaveLoad.h"
#include "includes.h"

static uint8_t slot;
static bool _isSaving = true;
static bool slotSaved = false;



void menuUBLSaveLoad(void)
{
  MENUITEMS UBLSaveLoadItems = {
  // title
  LABEL_ABL_SETTINGS_UBL_SAVE,
    // icon              label
   {{ICON_S_SAVE,        LABEL_ABL_SLOT0},
    {ICON_S_SAVE,        LABEL_ABL_SLOT1},
    {ICON_S_SAVE,        LABEL_ABL_SLOT2},
    {ICON_S_SAVE,        LABEL_ABL_SLOT3},
    {ICON_BACKGROUND,    LABEL_BACKGROUND},
    {ICON_BACKGROUND,    LABEL_BACKGROUND},
    {ICON_BACKGROUND,    LABEL_BACKGROUND},
    {ICON_BACK,          LABEL_BACK},}
  };

  if(!_isSaving)
  {
    UBLSaveLoadItems.title.index = LABEL_ABL_SETTINGS_UBL_LOAD;
    for (int i = 0; i < 4; i++)
    {
      UBLSaveLoadItems.items[i].icon = ICON_S_LOAD;
    }
  }

  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&UBLSaveLoadItems);
  while(infoMenu.menu[infoMenu.cur] == menuUBLSaveLoad)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
        slot = key_num;
        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLSaveLoadItems.title.index), textSelect(LABEL_CONFIRMATION),
                    textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveloadConfirm, NULL, NULL);
        break;
      case KEY_ICON_7:
        if (slotSaved == true && infoMachineSettings.EEPROM == 1)
        {
          slotSaved = false;
          showDialog(DIALOG_TYPE_QUESTION, textSelect(LABEL_ABL_SETTINGS_UBL), textSelect(LABEL_ABL_SLOT_EEPROM ),
                     textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        else
        {
          slotSaved = false;
          infoMenu.cur--;
        }
        break;
      default:
        break;
    }
    loopProcess();
  }
}

void ublSaveLoad(bool isSaving)
{
  _isSaving = isSaving;
  infoMenu.menu[++infoMenu.cur] = menuUBLSaveLoad;
}

void saveloadConfirm(void)
{
    if(!_isSaving)
      {
        storeCmd("G29 L%d\n", slot);
      }
    else
    {
      storeCmd("G29 S%d\n", slot);
      slotSaved = true;
    }
}
