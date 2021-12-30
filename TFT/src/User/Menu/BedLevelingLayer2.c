#include "BedLevelingLayer2.h"
#include "includes.h"

void menuBedLevelingLayer2(void)
{
  MENUITEMS bedLevelingLayer2Items = {
    // title
    LABEL_ABL_SETTINGS,
    // icon                          label
    {
      {ICON_LEVELING,                LABEL_START},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
      bedLevelingLayer2Items.title.index = LABEL_ABL_SETTINGS_BBL;
      break;

    case BL_UBL:
      bedLevelingLayer2Items.title.index = LABEL_ABL_SETTINGS_UBL;
      bedLevelingLayer2Items.items[1].icon = ICON_EEPROM_SAVE;
      bedLevelingLayer2Items.items[1].label.index = LABEL_SAVE;
      bedLevelingLayer2Items.items[2].icon = ICON_EEPROM_RESTORE;
      bedLevelingLayer2Items.items[2].label.index = LABEL_LOAD;
      break;

    case BL_MBL:
      bedLevelingLayer2Items.title.index = LABEL_MBL_SETTINGS;

    default:
      break;
  }

  #if DELTA_PROBE_TYPE != 2  // if not removable probe
    if (infoMachineSettings.zProbe == ENABLED)
    {
      bedLevelingLayer2Items.items[3].icon = ICON_LEVEL_CORNER;
      bedLevelingLayer2Items.items[3].label.index = LABEL_LEVEL_CORNER;

      if (infoSettings.touchmi_sensor != 0)
      {
        bedLevelingLayer2Items.items[4].icon = ICON_NOZZLE;
        bedLevelingLayer2Items.items[4].label.index = LABEL_TOUCHMI;
      }
      else
      {
        bedLevelingLayer2Items.items[4].icon = ICON_BLTOUCH;
        bedLevelingLayer2Items.items[4].label.index = LABEL_BLTOUCH;
      }

      if (infoSettings.z_steppers_alignment != 0)
      {
        bedLevelingLayer2Items.items[5].icon = ICON_Z_ALIGN;
        bedLevelingLayer2Items.items[5].label.index = LABEL_Z_ALIGN;
      }
    }
  #endif

  menuDrawPage(&bedLevelingLayer2Items);

  while (MENU_IS(menuBedLevelingLayer2))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (infoMachineSettings.leveling < BL_MBL)  // if ABL
        {
          #if DELTA_PROBE_TYPE != 2  // if not removable probe
            ablStart();
          #else  // if removable probe
            setDialogText(LABEL_WARNING, LABEL_CONNECT_PROBE, LABEL_CONTINUE, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_ALERT, ablStart, NULL, NULL);
          #endif
        }
        else  // if MBL
        {
          OPEN_MENU(menuMBL);
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

      case KEY_ICON_3:
        #if DELTA_PROBE_TYPE != 2  // if not removable probe
          if (infoMachineSettings.zProbe == ENABLED)
            OPEN_MENU(menuLevelCorner);
        #endif
        break;

      case KEY_ICON_4:
        #if DELTA_PROBE_TYPE != 2  // if not removable probe
          if (infoMachineSettings.zProbe == ENABLED)
          {
            if (infoSettings.touchmi_sensor != 0)
              OPEN_MENU(menuTouchMi);
            else
              OPEN_MENU(menuBLTouch);
          }
        #endif
        break;

      case KEY_ICON_5:
        if (infoMachineSettings.zProbe == ENABLED && infoSettings.z_steppers_alignment != 0)
        {
          storeCmd("G34\n");
          storeCmd("M18 S0 X Y Z\n");
        }
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
