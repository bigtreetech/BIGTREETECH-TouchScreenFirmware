#include "Leveling.h"
#include "includes.h"

void menuLeveling(void)
{
  MENUITEMS LevelingItems = {
    // title
    LABEL_LEVELING,
    // icon                          label
    {
      {ICON_PROBE_OFFSET,            LABEL_H_OFFSET},
      {ICON_MANUAL_LEVEL,            LABEL_LEVELING},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_HEAT_FAN,                LABEL_UNIFIEDHEAT},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoMachineSettings.leveling != BL_DISABLED)
  {
    LevelingItems.items[2].icon = ICON_LEVELING;
    LevelingItems.items[2].label.index = LABEL_BED_LEVELING;
  }

  menuDrawPage(&LevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuLeveling)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        storeCmd("M206\n");
        zOffsetSetMenu(false);  // use Home Offset menu
        infoMenu.menu[++infoMenu.cur] = menuZOffset;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
        break;

      case KEY_ICON_2:
        if (infoMachineSettings.leveling != BL_DISABLED)
          infoMenu.menu[++infoMenu.cur] = menuBedLeveling;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
        break;

      case KEY_ICON_7:
        for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
        {
          if (heatGetTargetTemp(i) > 0)
          {
            setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
            break;
          }
        }
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
