#include "BedLeveling.h"
#include "includes.h"

static inline void blUpdateState(MENUITEMS *menu)
{
  if (getParameter(P_ABL_STATE, 0) == ENABLED)
  {
    menu->items[3].icon = ICON_LEVELING_ON;
    menu->items[3].label.index = LABEL_BL_ENABLE;
  }
  else
  {
    menu->items[3].icon = ICON_LEVELING_OFF;
    menu->items[3].label.index = LABEL_BL_DISABLE;
  }
  menuDrawItem(&menu->items[3], 3);
}

void menuBedLeveling(void)
{
  MENUITEMS bedLevelingItems = {
    // title
    LABEL_ABL_SETTINGS,
    // icon                          label
    {
      {ICON_LEVELING,                LABEL_ABL},
      {ICON_MESH_EDITOR,             LABEL_MESH_EDITOR},
      {ICON_MESH_VALID,              LABEL_MESH_VALID},
      {ICON_LEVELING_OFF,            LABEL_BL_DISABLE},
      {ICON_Z_FADE,                  LABEL_ABL_Z},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_HEAT_FAN,                LABEL_UNIFIEDHEAT},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  int8_t levelStateOld = -1;

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
      bedLevelingItems.title.index = LABEL_ABL_SETTINGS_BBL;
      bedLevelingItems.items[0].label.index = LABEL_BBL;
      break;

    case BL_UBL:
      bedLevelingItems.title.index = LABEL_ABL_SETTINGS_UBL;
      bedLevelingItems.items[0].label.index = LABEL_UBL;
      break;

    case BL_MBL:
      bedLevelingItems.title.index = LABEL_MBL_SETTINGS;
      bedLevelingItems.items[0].label.index = LABEL_MBL;
      break;

    default:
      break;
  }

  if (getParameter(P_ABL_STATE, 0) == ENABLED)
  {
    bedLevelingItems.items[3].icon = ICON_LEVELING_ON;
    bedLevelingItems.items[3].label.index = LABEL_BL_ENABLE;
  }

  if (infoMachineSettings.zProbe == ENABLED)
  {
    bedLevelingItems.items[5].icon = ICON_PROBE_OFFSET;
    bedLevelingItems.items[5].label.index = LABEL_P_OFFSET;
  }

  menuDrawPage(&bedLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuBedLeveling)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuBedLevelingLayer2;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuMeshEditor;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuMeshValid;
        break;

      case KEY_ICON_3:
        if (getParameter(P_ABL_STATE, 0) == ENABLED)
          storeCmd(infoMachineSettings.firmwareType != FW_REPRAPFW ? "M420 S0\n" : "G29 S2\n");
        else
          storeCmd(infoMachineSettings.firmwareType != FW_REPRAPFW ? "M420 S1\n" : "G29 S1\n");
        break;

      case KEY_ICON_4:
      {
        float val = editFloatValue(Z_FADE_MIN_VALUE, Z_FADE_MAX_VALUE, 0.0f, getParameter(P_ABL_STATE, 1));

        if (val != getParameter(P_ABL_STATE, 1))
          storeCmd("M420 Z%.2f\n", val);

        menuDrawPage(&bedLevelingItems);
        break;
      }

      case KEY_ICON_5:
        if (infoMachineSettings.zProbe == ENABLED)
        {
          storeCmd("M851\n");
          zOffsetSetMenu(true);  // use Probe Offset menu
          infoMenu.menu[++infoMenu.cur] = menuZOffset;
        }
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
        break;

      case KEY_ICON_7:
        cooldownTemperature();
        infoMenu.cur--;
        break;

      default:
        break;
    }

    if (levelStateOld != getParameter(P_ABL_STATE, 0))
    {
      levelStateOld = getParameter(P_ABL_STATE, 0);
      blUpdateState(&bedLevelingItems);
    }

    loopProcess();
  }
}
