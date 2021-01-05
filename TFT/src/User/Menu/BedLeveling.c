#include "BedLeveling.h"
#include "includes.h"

static inline void blUpdateState(MENUITEMS *menu)
{
  if (getParameter(P_ABL_STATE, 0) == ENABLED)
  {
    menu->items[6].icon = ICON_LEVELING_ON;
    menu->items[6].label.index = LABEL_BL_ENABLE;
  }
  else
  {
    menu->items[6].icon = ICON_LEVELING_OFF;
    menu->items[6].label.index = LABEL_BL_DISABLE;
  }
  menuDrawItem(&menu->items[6], 6);
}

void menuBedLeveling(void)
{
  MENUITEMS bedLevelingItems = {
    // title
    LABEL_ABL_SETTINGS,
    // icon                         label
    {{ICON_LEVELING,                LABEL_ABL},
     {ICON_LEVELING,                LABEL_MESH_EDITOR},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_PROBE_OFFSET,            LABEL_Z_OFFSET},
     {ICON_Z_FADE,                  LABEL_ABL_Z},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  void (*menuBL)(void) = menuABL;
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

      menuBL = menuMBL;
      break;

    default:
      break;
  }

  if (getParameter(P_ABL_STATE, 0) == ENABLED)
  {
    bedLevelingItems.items[6].icon = ICON_LEVELING_ON;
    bedLevelingItems.items[6].label.index = LABEL_BL_ENABLE;
  }
  else
  {
    bedLevelingItems.items[6].icon = ICON_LEVELING_OFF;
    bedLevelingItems.items[6].label.index = LABEL_BL_DISABLE;
  }

  menuDrawPage(&bedLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuBedLeveling)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuBL;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuMeshEditor;
        break;

      case KEY_ICON_4:
        storeCmd("M851\n");
        infoMenu.menu[++infoMenu.cur] = menuProbeOffset;
        break;

      case KEY_ICON_5:
      {
        char tempstr[30];
        sprintf(tempstr, "%Min:%.2f | Max:%.2f", Z_FADE_MIN_VALUE, Z_FADE_MAX_VALUE);

        float val = numPadFloat((u8 *) tempstr, getParameter(P_ABL_STATE, 1), 0.0f, false);
        storeCmd("M420 Z%.2f\n", NOBEYOND(Z_FADE_MIN_VALUE, val, Z_FADE_MAX_VALUE));

        menuDrawPage(&bedLevelingItems);
        break;
      }

      case KEY_ICON_6:
        if (getParameter(P_ABL_STATE, 0) == ENABLED)
          storeCmd("M420 S0\n");
        else
          storeCmd("M420 S1\n");
        break;

      case KEY_ICON_7:
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
