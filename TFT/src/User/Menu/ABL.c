#include "ABL.h"
#include "includes.h"

static uint8_t slot;
static bool _isSaving = true;
static bool slotSaved = false;

void ablUpdateStatus(bool succeeded)
{
  char tmpTitle[120], tmpBuf[120];
  bool savingEnabled = false;

  switch (infoMachineSettings.blType)
  {
    case BL_BBL:
      sprintf(tmpTitle, "%s", textSelect(LABEL_ABL_SETTINGS_BBL));
      sprintf(tmpBuf, "%s", textSelect(LABEL_BL_COMPLETE));
      savingEnabled = true;
      break;

    case BL_UBL:
      sprintf(tmpTitle, "%s", textSelect(LABEL_ABL_SETTINGS_UBL));
      sprintf(tmpBuf, "%s\n %s", textSelect(LABEL_BL_COMPLETE), textSelect(LABEL_BL_SMART_FILL));
      break;

    default:
      sprintf(tmpTitle, "%s", textSelect(LABEL_ABL_SETTINGS));
      sprintf(tmpBuf, "%s", textSelect(LABEL_BL_COMPLETE));
      savingEnabled = true;
      break;
  }

  if (succeeded)                       // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(sound_success);

    if (savingEnabled && infoMachineSettings.EEPROM == 1)
    {
      sprintf(tmpBuf, "%s\n %s", tmpBuf, textSelect(LABEL_EEPROM_SAVE_INFO));

      showDialog(DIALOG_TYPE_SUCCESS, (u8*) tmpTitle, (u8*) tmpBuf,
        textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, (u8*) tmpTitle, (u8*) tmpBuf);
    }
  }
  else                                 // if if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, (u8*) tmpTitle, textSelect(LABEL_PROCESS_ABORTED));
  }
}

void ublSaveloadConfirm(void)
{
  if (!_isSaving)
  {
    storeCmd("G29 L%d\n", slot);
  }
  else
  {
    storeCmd("G29 S%d\n", slot);
    slotSaved = true;
  }
}

void menuUBLSaveLoad(void)
{
  MENUITEMS UBLSaveLoadItems = {
    // title
    LABEL_ABL_SETTINGS_UBL_SAVE,
    // icon                         label
    {{ICON_S_SAVE,                  LABEL_ABL_SLOT0},
     {ICON_S_SAVE,                  LABEL_ABL_SLOT1},
     {ICON_S_SAVE,                  LABEL_ABL_SLOT2},
     {ICON_S_SAVE,                  LABEL_ABL_SLOT3},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!_isSaving)
  {
    UBLSaveLoadItems.title.index = LABEL_ABL_SETTINGS_UBL_LOAD;
    for (int i = 0; i < 4; i++)
    {
      UBLSaveLoadItems.items[i].icon = ICON_S_LOAD;
    }
  }

  menuDrawPage(&UBLSaveLoadItems);

  while (infoMenu.menu[infoMenu.cur] == menuUBLSaveLoad)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
        slot = key_num;

        showDialog(DIALOG_TYPE_QUESTION, textSelect(UBLSaveLoadItems.title.index), textSelect(LABEL_CONFIRMATION),
          textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), ublSaveloadConfirm, NULL, NULL);
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

void menuABL(void)
{
  MENUITEMS autoLevelingItems = {
    // title
    LABEL_ABL_SETTINGS,
    // icon                         label
    {{ICON_LEVELING,                LABEL_START},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BLTOUCH,                 LABEL_BLTOUCH},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK}}
  };

  KEY_VALUES key_num = KEY_IDLE;

  switch (infoMachineSettings.blType)
  {
    case BL_BBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_BBL;
      break;

    case BL_UBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_UBL;
      autoLevelingItems.items[1].icon = ICON_S_SAVE;
      autoLevelingItems.items[1].label.index = LABEL_SAVE;
      autoLevelingItems.items[2].icon = ICON_S_LOAD;
      autoLevelingItems.items[2].label.index = LABEL_LOAD;
      break;

    default:
      break;
  }

  menuDrawPage(&autoLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuABL)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        storeCmd("G28\n");

        switch (infoMachineSettings.blType)
        {
          case BL_BBL:                                     // if Bilinear Bed Leveling
            storeCmd("G29\n");
            storeCmd("M118 A1 BBL Complete\n");
            break;

          case BL_UBL:                                     // if Unified Bed Leveling
            storeCmd("G29 P1\n");
            storeCmd("G29 P3\n");
            storeCmd("M118 A1 UBL Complete\n");
            break;

          default:                                         // if any other Auto Bed Leveling
            storeCmd("G29\n");
            storeCmd("M118 A1 ABL Complete\n");
            break;
        }
        break;

      case KEY_ICON_1:
        if (infoMachineSettings.blType == BL_UBL)
          ublSaveLoad(true);
        break;

      case KEY_ICON_2:
        if (infoMachineSettings.blType == BL_UBL)
          ublSaveLoad(false);
        break;

      case KEY_ICON_4:
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
