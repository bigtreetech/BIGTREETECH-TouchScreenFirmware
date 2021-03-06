#include "LoadUnload.h"
#include "includes.h"

#define LOAD 1
#define UNLOAD 2
#define NONE 0

const MENUITEMS loadUnloadItems = {
  // title
  LABEL_LOAD_UNLOAD,
  // icon                         label
  {{ICON_UNLOAD,                  LABEL_UNLOAD},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_LOAD,                    LABEL_LOAD},
   {ICON_NOZZLE,                  LABEL_NOZZLE},
   {ICON_HEAT,                    LABEL_HEAT},
   {ICON_COOLDOWN,                LABEL_COOLDOWN},
   {ICON_BACK,                    LABEL_BACK},}
};

static u8 curExt_index = 0;
static u8 lastcmd = NONE;

void extruderIdReDraw(void)
{
  char tempstr[20];

  sprintf(tempstr, "%2s: %3d/%-3d", heatDisplayID[curExt_index], heatGetCurrentTemp(curExt_index), heatGetTargetTemp(curExt_index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

// set the hotend to the minimum extrusion temperature if user selected "OK"
void loadMinTemp_OK(void)
{
  heatSetTargetTemp(curExt_index, infoSettings.min_ext_temp);
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

    if (infoHost.wait == true && key_num != KEY_IDLE &&
        key_num != KEY_ICON_7)  // show reminder for process running if presses any button other than bacnk button
    {
      if (lastcmd == UNLOAD)
      { // unloading
        popupReminder(DIALOG_TYPE_INFO, LABEL_UNLOAD, LABEL_UNLOAD_STARTED);
      }
      else if (lastcmd == LOAD)
      { // loading
        popupReminder(DIALOG_TYPE_INFO, LABEL_LOAD, LABEL_LOAD_STARTED);
      }
      else
      { // in case user gets to Load/Unload menu while host is busy
        popupReminder(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);
      }
    }
    else
    {
      switch(key_num)
      {
      case KEY_ICON_0: // Unload
      case KEY_ICON_3: // Load
        if (heatGetCurrentTemp(curExt_index) < infoSettings.min_ext_temp)
        { // low temperature warning
          char tempMsg[120];
          LABELCHAR(tempStr, LABEL_EXT_TEMPLOW);
          sprintf(tempMsg, tempStr, infoSettings.min_ext_temp);
          strcat(tempMsg, "\n");
          sprintf(tempStr, (char *)textSelect(LABEL_HEAT_HOTEND), infoSettings.min_ext_temp);
          strcat(tempMsg, tempStr);

          setDialogText(LABEL_WARNING, (uint8_t *)tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ERROR, loadMinTemp_OK, NULL, NULL);
        }
        else if (key_num == KEY_ICON_0)
        { // unload
          mustStoreCmd("M702 T%d\n", curExt_index);
          lastcmd = UNLOAD;
        }
        else
        { // load
          mustStoreCmd("M701 T%d\n", curExt_index);
          lastcmd = LOAD;
        }
        break;

      case KEY_ICON_4:
        curExt_index = (curExt_index + 1) % infoSettings.hotend_count;
        extruderIdReDraw();
        lastcmd = NONE;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        lastcmd = NONE;
        break;

      case KEY_ICON_6:
        heatCoolDown();
        lastcmd = NONE;
        break;

      case KEY_ICON_7:
        if (!isPrinting())
        {
          for (uint8_t i = 0; i < infoSettings.hotend_count; i++)
          {
            if (heatGetTargetTemp(i) > 0)
            {
              setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_CONFIRM, LABEL_CANCEL);
              showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
              break;
            }
          }
        }
        infoMenu.cur--;
        lastcmd = NONE;
        break;

      default:
        extruderIdReDraw();
        break;
      }
    }
    loopProcess();
  }
}
