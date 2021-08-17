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

  while (infoMenu.menu[infoMenu.cur] == menuCustom)
  {
    curIndex = listViewGetSelectedIndex();

    if (curIndex < customcodes.count)
      mustStoreScript(customcodes.gcode[curIndex]);

    loopProcess();
  }
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
      {ICON_BACKGROUND,      LABEL_BACKGROUND},
      {ICON_BACKGROUND,      LABEL_BACKGROUND},
      {ICON_BACKGROUND,      LABEL_BACKGROUND},
      {ICON_BACKGROUND,      LABEL_BACKGROUND},
      {ICON_BACK,            LABEL_BACK},
    }
  };

  KEY_VALUES curIndex = KEY_IDLE;

  menuDrawPage(&eepromSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuEepromSettings)
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        // save to EEPROM
        if (infoMachineSettings.EEPROM == 1)
        {
          setDialogText(eepromSettingsItems.title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        break;

      case KEY_ICON_1:
        // restore from EEPROM
        if (infoMachineSettings.EEPROM == 1)
        {
          setDialogText(eepromSettingsItems.title.index, LABEL_EEPROM_RESTORE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, restoreEepromSettings, NULL, NULL);
        }
        break;

      case KEY_ICON_2:
        // reset EEPROM
        if (infoMachineSettings.EEPROM == 1)
        {
          setDialogText(eepromSettingsItems.title.index, LABEL_EEPROM_RESET_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, resetEepromSettings, NULL, NULL);
        }
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
        {ICON_BACKGROUND,              LABEL_BACKGROUND},
      #endif
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES curIndex = KEY_IDLE;
  const ITEM itemCaseLight = {ICON_CASE_LIGHT, LABEL_CASE_LIGHT};

  if (infoMachineSettings.caseLightsBrightness == ENABLED)
    machineSettingsItems.items[KEY_ICON_6] = itemCaseLight;

  menuDrawPage(&machineSettingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuMachineSettings)
  {
    curIndex = menuKeyGetValue();
    switch (curIndex)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuParameterSettings;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuCustom;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuLEDColor;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuTuning;
        break;

      #ifdef QUICK_EEPROM_BUTTON
        case KEY_ICON_5:
          infoMenu.menu[++infoMenu.cur] = menuEepromSettings;
          break;
      #endif

      case KEY_ICON_6:
        if (infoMachineSettings.caseLightsBrightness == ENABLED)
          infoMenu.menu[++infoMenu.cur] = menuCaseLight;
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
