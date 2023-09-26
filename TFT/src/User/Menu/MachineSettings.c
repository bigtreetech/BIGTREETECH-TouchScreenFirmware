#include "MachineSettings.h"
#include "includes.h"

void menuCustom(void)
{
  CUSTOM_GCODES customcodes;

  // load custom codes
  W25Qxx_ReadBuffer((uint8_t *)&customcodes, CUSTOM_GCODE_ADDR, sizeof(CUSTOM_GCODES));

  LABEL title = {LABEL_CUSTOM};
  LISTITEM customItems[customcodes.count];
  uint16_t curIndex = KEY_IDLE;

  // load custom gcodes to list items
  for (uint32_t i = 0; i < customcodes.count; i++)
  {
    customItems[i].icon = CHARICON_CODE;
    customItems[i].itemType = LIST_LABEL;
    customItems[i].titlelabel.address = (uint8_t*)customcodes.name[i];
  }

  listViewCreate(title, customItems, customcodes.count, NULL, true, NULL, NULL);

  TASK_LOOP_WHILE(MENU_IS(menuCustom), curIndex = listViewGetSelectedIndex();
                  if (curIndex < customcodes.count) mustStoreScript(customcodes.gcode[curIndex]));
}

#ifdef QUICK_EEPROM_BUTTON

void menuEepromSettings(void)
{
  MENUITEMS eepromSettingsItems = {
    // title
    LABEL_EEPROM_SETTINGS,
    // icon                  label
    {
      {ICON_EEPROM_SAVE,     LABEL_SAVE},
      {ICON_EEPROM_RESTORE,  LABEL_RESTORE},
      {ICON_EEPROM_RESET,    LABEL_RESET},
      {ICON_NULL,            LABEL_NULL},
      {ICON_NULL,            LABEL_NULL},
      {ICON_NULL,            LABEL_NULL},
      {ICON_NULL,            LABEL_NULL},
      {ICON_BACK,            LABEL_BACK},
    }
  };

  KEY_VALUES curIndex = KEY_IDLE;

  menuDrawPage(&eepromSettingsItems);

  while (MENU_IS(menuEepromSettings))
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        // save to EEPROM
        if (infoMachineSettings.EEPROM == 1)
          popupDialog(DIALOG_TYPE_QUESTION, eepromSettingsItems.title.index, LABEL_EEPROM_SAVE_INFO,
                      LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
        break;

      case KEY_ICON_1:
        // restore from EEPROM
        if (infoMachineSettings.EEPROM == 1)
          popupDialog(DIALOG_TYPE_QUESTION, eepromSettingsItems.title.index, LABEL_EEPROM_RESTORE_INFO,
                      LABEL_CONFIRM, LABEL_CANCEL, restoreEepromSettings, NULL, NULL);
        break;

      case KEY_ICON_2:
        // reset EEPROM
        if (infoMachineSettings.EEPROM == 1)
          popupDialog(DIALOG_TYPE_QUESTION, eepromSettingsItems.title.index, LABEL_EEPROM_RESET_INFO,
                      LABEL_CONFIRM, LABEL_CANCEL, resetEepromSettings, NULL, NULL);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}

#endif  // QUICK_EEPROM_BUTTON

void menuMachineSettings(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS machineSettingsItems = {
    // title
    LABEL_MACHINE_SETTINGS,
    // icon                          label
    {
      {ICON_PARAMETER,               LABEL_PARAMETER_SETTINGS},
      {ICON_GCODE,                   LABEL_TERMINAL},
      {ICON_CUSTOM,                  LABEL_CUSTOM},
      {ICON_RGB_SETTINGS,            LABEL_RGB_SETTINGS},
      {ICON_TUNING,                  LABEL_TUNING},
      #ifdef QUICK_EEPROM_BUTTON
        {ICON_EEPROM_SAVE,             LABEL_EEPROM_SETTINGS},
      #else
        {ICON_NULL,                    LABEL_NULL},
      #endif
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    machineSettingsItems.items[2].icon = ICON_NULL;
    machineSettingsItems.items[2].label.index = LABEL_NULL;
  }

  KEY_VALUES curIndex = KEY_IDLE;

  if (infoMachineSettings.caseLightsBrightness == ENABLED)
  {
    machineSettingsItems.items[KEY_ICON_6].icon = ICON_CASE_LIGHT;
    machineSettingsItems.items[KEY_ICON_6].label.index = LABEL_CASE_LIGHT;
  }

  menuDrawPage(&machineSettingsItems);

  while (MENU_IS(menuMachineSettings))
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuParameterSettings);
        break;

      case KEY_ICON_1:
        OPEN_MENU(menuTerminal);
        break;

      case KEY_ICON_2:
        if (infoMachineSettings.firmwareType != FW_REPRAPFW)
          OPEN_MENU(menuCustom);
        break;

      case KEY_ICON_3:
        OPEN_MENU(menuLEDColor);
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuTuning);
        break;

      #ifdef QUICK_EEPROM_BUTTON
        case KEY_ICON_5:
          OPEN_MENU(menuEepromSettings);
          break;
      #endif

      case KEY_ICON_6:
        if (infoMachineSettings.caseLightsBrightness == ENABLED)
          OPEN_MENU(menuCaseLight);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
