#include "LoadUnload.h"
#include "includes.h"

#define LOAD 1
#define UNLOAD 2
#define NO_LOAD_UNLOAD 0

const MENUITEMS loadUnloadItems = {
// title
LABEL_LOAD_UNLOAD,
// icon                       label
{ {ICON_UNLOAD,               LABEL_UNLOAD},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_LOAD,                 LABEL_LOAD},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_HEAT,                 LABEL_HEAT},
  {ICON_COOLDOWN,             LABEL_COOLDOWN},
  {ICON_BACK,                 LABEL_BACK},}
};

static u8  curExt_index = 0;

void extruderIdReDraw(void)
{
  char tempstr[20];

  sprintf(tempstr, "%2s: %3d/%-3d", heatDisplayID[curExt_index], heatGetCurrentTemp(curExt_index), heatGetTargetTemp(curExt_index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void coolDown(void)
{
  for(uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heatSetTargetTemp(i, 0);
  }
}


void menuLoadUnload(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  while(infoCmd.count != 0) {loopProcess();}

  menuDrawPage(&loadUnloadItems);
  extruderIdReDraw();

  while(infoMenu.menu[infoMenu.cur] == menuLoadUnload)
  {
    key_num = menuKeyGetValue();
    if ((infoHost.wait == true) && (key_num != KEY_IDLE))  // if user pokes around while Load/Unload in progress
    {
      // in case user gets to Load/Unload menu while host is busy
      popupReminder(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);
    }
    else
    {
      switch(key_num)
      {
      case KEY_ICON_0:
        mustStoreCmd("M702 T%d\n", curExt_index);
        popupReminder(DIALOG_TYPE_INFO, LABEL_BUSY, LABEL_UNLOAD_STARTED);
        break;

      case KEY_ICON_3:
        mustStoreCmd("M701 T%d\n", curExt_index);
        popupReminder(DIALOG_TYPE_INFO, LABEL_BUSY, LABEL_LOAD_STARTED);
        break;

      case KEY_ICON_4:
        curExt_index = (curExt_index + 1) % infoSettings.hotend_count;
        extruderIdReDraw();
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_6:
        coolDown();
        break;

      case KEY_ICON_7:
        for (uint8_t i = 0; i < infoSettings.hotend_count; i++)
        {
          if (heatGetTargetTemp(i) > 0)
          {
            setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_CONFIRM, LABEL_CANCEL)
                showDialog(DIALOG_TYPE_QUESTION, coolDown, NULL, NULL);
            break;
          }
        }
        infoMenu.cur--;
        break;

      default:
        extruderIdReDraw();
        break;
      }
    }
    loopProcess();
  }
}
