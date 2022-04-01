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
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_HEAT_FAN,                LABEL_UNIFIEDHEAT},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  bool index3And4Support = (infoMachineSettings.firmwareType == FW_MARLIN || infoMachineSettings.firmwareType == FW_REPRAPFW);
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

  if (index3And4Support)
  {
    levelStateOld = getParameter(P_ABL_STATE, 0);
    blUpdateState(&bedLevelingItems);  // update icon & label 3

    bedLevelingItems.items[4].icon = ICON_Z_FADE;
    bedLevelingItems.items[4].label.index = LABEL_ABL_Z;
  }

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
        #if DELTA_PROBE_TYPE == 0
          OPEN_MENU(menuBedLevelingLayer2);
        #else
          {
            #if DELTA_PROBE_TYPE != 2  // if not removable probe
              ablStart();
            #else  // if removable probe
              setDialogText(LABEL_WARNING, LABEL_CONNECT_PROBE, LABEL_CONTINUE, LABEL_CANCEL);
              showDialog(DIALOG_TYPE_ALERT, ablStart, NULL, NULL);
            #endif
          }
        #endif
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
        if (index3And4Support)
        {
          if (getParameter(P_ABL_STATE, 0) == ENABLED)
            storeCmd(infoMachineSettings.firmwareType != FW_REPRAPFW ? "M420 S0\n" : "G29 S2\n");
          else
            storeCmd(infoMachineSettings.firmwareType != FW_REPRAPFW ? "M420 S1\n" : "G29 S1\n");
        }
        break;

      case KEY_ICON_4:
      {
        if (index3And4Support)
        {
          float val = editFloatValue(Z_FADE_MIN_VALUE, Z_FADE_MAX_VALUE, 0.0f, getParameter(P_ABL_STATE, 1));

          if (val != getParameter(P_ABL_STATE, 1))
            storeCmd(infoMachineSettings.firmwareType != FW_REPRAPFW ? "M420 Z%.2f\n" : "M376 H%.2f\n", val);

          menuDrawPage(&bedLevelingItems);
        }
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
        COOLDOWN_TEMPERATURE();
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (index3And4Support && levelStateOld != getParameter(P_ABL_STATE, 0))
    {
      levelStateOld = getParameter(P_ABL_STATE, 0);

      blUpdateState(&bedLevelingItems);
      menuDrawItem(&bedLevelingItems.items[3], 3);
    }

    loopProcess();
  }
}
