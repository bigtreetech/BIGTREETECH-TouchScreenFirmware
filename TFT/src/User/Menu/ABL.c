#include "ABL.h"
#include "includes.h"

static uint8_t ublSlot;
static bool ublIsSaving = true;
static bool ublSlotSaved = false;
bool heat = false;

/* called by parseAck() to notify ABL process status */
void ablUpdateStatus(bool succeeded)
{
  bool savingEnabled = true;

  init_label(tempTitle);
  tempTitle.index = LABEL_ABL_SETTINGS;
  labelChar(tempMsg, LABEL_BL_COMPLETE);

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
    {
      tempTitle.index = LABEL_ABL_SETTINGS_BBL;
      break;
    }
    case BL_UBL:
    {
      tempTitle.index = LABEL_ABL_SETTINGS_UBL;

      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_BL_SMART_FILL));

      savingEnabled = false;
      break;
    }
    default:
      break;
  }

  if (succeeded)                                           // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(sound_success);

    if (savingEnabled && infoMachineSettings.EEPROM == 1)
    {
      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

      setDialogText(tempTitle.index, (u8 *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, tempTitle.index, (u8 *) tempMsg);
    }
  }
  else                                                     // if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, tempTitle.index, LABEL_PROCESS_ABORTED);
  }
}

void ublSaveloadConfirm(void)
{
  if (!ublIsSaving)
  {
    storeCmd("G29 L%d\n", ublSlot);
  }
  else
  {
    storeCmd("G29 S%d\n", ublSlot);
    ublSlotSaved = true;
  }
}

void menuUBLSaveLoad(void)
{
  MENUITEMS UBLSaveLoadItems = {
    // title
    LABEL_ABL_SETTINGS_UBL_SAVE,
    // icon                         label
    {{ICON_EEPROM_SAVE,             LABEL_ABL_SLOT0},
     {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT1},
     {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT2},
     {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT3},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!ublIsSaving)
  {
    UBLSaveLoadItems.title.index = LABEL_ABL_SETTINGS_UBL_LOAD;
    for (int i = 0; i < 4; i++)
    {
      UBLSaveLoadItems.items[i].icon = ICON_EEPROM_RESTORE;
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
        ublSlot = key_num;

        setDialogText(UBLSaveLoadItems.title.index, LABEL_CONFIRMATION, LABEL_CONFIRM, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_QUESTION, ublSaveloadConfirm, NULL, NULL);
        break;

      case KEY_ICON_7:
        if (ublSlotSaved == true && infoMachineSettings.EEPROM == 1)
        {
          ublSlotSaved = false;

          setDialogText(LABEL_ABL_SETTINGS_UBL, LABEL_ABL_SLOT_EEPROM, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        else
        {
          ublSlotSaved = false;

          infoMenu.cur--;
        }
        break;

      default:
        break;
    }

    loopProcess();
  }
}

void menuUBLSave(void)
{
  ublIsSaving = true;
  infoMenu.menu[++infoMenu.cur] = menuUBLSaveLoad;
}

void menuUBLLoad(void)
{
  ublIsSaving = false;
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
     {ICON_HEAT,                    LABEL_PREHEAT},
     {ICON_BACK,                    LABEL_BACK}}
  };

  KEY_VALUES key_num = KEY_IDLE;

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_BBL;
      break;

    case BL_UBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_UBL;
      autoLevelingItems.items[1].icon = ICON_EEPROM_SAVE;
      autoLevelingItems.items[1].label.index = LABEL_SAVE;
      autoLevelingItems.items[2].icon = ICON_EEPROM_RESTORE;
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

        switch (infoMachineSettings.leveling)
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
        if (infoMachineSettings.leveling == BL_UBL)
          menuUBLSave();
        break;

      case KEY_ICON_2:
        if (infoMachineSettings.leveling == BL_UBL)
          menuUBLLoad();
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuBLTouch;
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuPreheat;
        heat = true;
        break;    

      case KEY_ICON_7:
       if (heat == true)
        {
          for(uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
          {
            heatSetTargetTemp(i, 0);
          }
          heat = false;
        }  
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
