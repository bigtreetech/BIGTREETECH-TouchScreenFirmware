#include "BedLeveling.h"
#include "includes.h"

void blUpdateState(MENUITEMS * menu)
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
}

#if DELTA_PROBE_TYPE == 2  // if Delta printer with removable probe
  void deltaMeshEditor(void)
  {
    OPEN_MENU(menuMeshEditor);
  }

  void deltaZOffset(void)
  {
    storeCmd("M851\n");
    zOffsetSetMenu(true);  // use Probe Offset menu
    OPEN_MENU(menuZOffset);
  }
#endif

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
      {ICON_NULL,                    LABEL_NULL},
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

  blUpdateState(&bedLevelingItems);

  if (infoMachineSettings.zProbe == ENABLED)
  {
    bedLevelingItems.items[5].icon = ICON_PROBE_OFFSET;
    bedLevelingItems.items[5].label.index = LABEL_P_OFFSET;
  }

  menuDrawPage(&bedLevelingItems);

  while (MENU_IS(menuBedLeveling))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuBedLevelingLayer2);
        break;

      case KEY_ICON_1:
        #if DELTA_PROBE_TYPE != 2
          OPEN_MENU(menuMeshEditor);
        #else
          setDialogText(LABEL_WARNING, LABEL_DISCONNECT_PROBE, LABEL_CONTINUE, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, deltaMeshEditor, NULL, NULL);
        #endif
        break;

      case KEY_ICON_2:
        OPEN_MENU(menuMeshValid);
        break;

      case KEY_ICON_3:
        if (getParameter(P_ABL_STATE, 0) == ENABLED)
        {
          switch (infoMachineSettings.firmwareType)
          {
            case FW_MARLIN:
              storeCmd("M420 S0\n");
              break;

            case FW_REPRAPFW:
              storeCmd("G29 S2\n");
              break;

            default:
              break;
          }
        }
        else
        {
          switch (infoMachineSettings.firmwareType)
          {
            case FW_MARLIN:
              storeCmd("M420 S1\n");
              break;

            case FW_REPRAPFW:
              storeCmd("G29 S1\n");
              break;

            default:
              break;
          }
        }
        break;

      case KEY_ICON_4:
      {
        float val = editFloatValue(Z_FADE_MIN_VALUE, Z_FADE_MAX_VALUE, 0.0f, getParameter(P_ABL_STATE, 1));

        if (val != getParameter(P_ABL_STATE, 1))
        {
          switch (infoMachineSettings.firmwareType)
          {
            case FW_MARLIN:
              storeCmd("M420 Z%.2f\n", val);
              break;

            case FW_REPRAPFW:
              storeCmd("M376 H%.2f\n", val);
              break;

            default:
              break;
          }
        }
          
        menuDrawPage(&bedLevelingItems);
        break;
      }

      case KEY_ICON_5:
        if (infoMachineSettings.zProbe == ENABLED)
        {
          #if DELTA_PROBE_TYPE != 2
            storeCmd("M851\n");
            zOffsetSetMenu(true);  // use Probe Offset menu
            OPEN_MENU(menuZOffset);
          #else
            setDialogText(LABEL_WARNING, LABEL_DISCONNECT_PROBE, LABEL_CONTINUE, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, deltaZOffset, NULL, NULL);
          #endif
        }
        break;

      case KEY_ICON_6:
        OPEN_MENU(menuUnifiedHeat);
        break;

      case KEY_ICON_7:
        cooldownTemperature();
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (levelStateOld != getParameter(P_ABL_STATE, 0))
    {
      levelStateOld = getParameter(P_ABL_STATE, 0);

      blUpdateState(&bedLevelingItems);
      menuDrawItem(&bedLevelingItems.items[3], 3);
    }

    loopProcess();
  }
}
