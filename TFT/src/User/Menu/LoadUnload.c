#include "LoadUnload.h"
#include "includes.h"

typedef enum
{
  NONE = 0,
  LOAD_REQUESTED,
  UNLOAD_REQUESTED,
  LOAD_STARTED,
  UNLOAD_STARTED,
} CMD_TYPE;

// 1 title, ITEM_PER_PAGE items (icon + label)
const MENUITEMS loadUnloadItems = {
  // title
  LABEL_LOAD_UNLOAD,
  // icon                          label
  {
    {ICON_UNLOAD,                  LABEL_UNLOAD},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_LOAD,                    LABEL_LOAD},
    {ICON_NOZZLE,                  LABEL_NOZZLE},
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_COOLDOWN,                LABEL_COOLDOWN},
    {ICON_BACK,                    LABEL_BACK},
  }
};

static uint8_t tool_index = NOZZLE0;
CMD_TYPE lastCmd = NONE;

// set the hotend to the minimum extrusion temperature if user selected "OK"
void loadMinTemp_OK(void)
{
  heatSetTargetTemp(tool_index, infoSettings.min_ext_temp);
}

void menuLoadUnload(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  if (eAxisBackup.backedUp == false)
  {
    loopProcessToCondition(&isNotEmptyCmdQueue);  // wait for the communication to be clean

    eAxisBackup.coordinate = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
    eAxisBackup.backedUp = true;
  }

  menuDrawPage(&loadUnloadItems);
  temperatureReDraw(tool_index, NULL, false);

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  while (infoMenu.menu[infoMenu.cur] == menuLoadUnload)
  {
    key_num = menuKeyGetValue();

    // show reminder for process running if any button is pressed
    if (infoHost.wait == true && key_num != KEY_IDLE)
    {
      if ((lastCmd == UNLOAD_REQUESTED) || (lastCmd == UNLOAD_STARTED))
      { // unloading in progress
        popupReminder(DIALOG_TYPE_INFO, LABEL_UNLOAD, LABEL_UNLOAD_STARTED);
      }
      else if ((lastCmd == LOAD_REQUESTED) || (lastCmd == LOAD_STARTED))
      { // loading in progress
        popupReminder(DIALOG_TYPE_INFO, LABEL_LOAD, LABEL_LOAD_STARTED);
      }
      else
      { // in case user gets to Load/Unload menu while host is busy
        popupReminder(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);
      }
    }
    else
    {
      switch (key_num)
      {
        case KEY_ICON_0:  // Unload
          lastCmd = UNLOAD_REQUESTED;
          break;

        case KEY_ICON_3:  // Load
          lastCmd = LOAD_REQUESTED;
          break;

        case KEY_ICON_4:  // nozzle select
          tool_index = (tool_index + 1) % infoSettings.hotend_count;

          temperatureReDraw(tool_index, NULL, false);
          lastCmd = NONE;
          break;

        case KEY_ICON_5:  // heat menu
          infoMenu.menu[++infoMenu.cur] = menuHeat;
          eAxisBackup.backedUp = false;  // exiting from Extrude menu (user might never come back by "Back" long press in Heat menu)
          lastCmd = NONE;
          break;

        case KEY_ICON_6:  // cool down nozzle
          heatCoolDown();
          lastCmd = NONE;
          break;

        case KEY_ICON_7:  // back
          cooldownTemperature();
          lastCmd = NONE;
          infoMenu.cur--;
          eAxisBackup.backedUp = false;  // the user exited from menu (not any other process/popup/etc)
          break;

        default:
          temperatureReDraw(tool_index, NULL, true);
          break;
      }

      if ((lastCmd == UNLOAD_REQUESTED) || (lastCmd == LOAD_REQUESTED))
      {
        switch (warmupNozzle(tool_index, loadMinTemp_OK))
        {
          case COLD:
            lastCmd = NONE;
            break;

          case SETTLING:
            break;

          case HEATED:
            if (lastCmd == UNLOAD_REQUESTED)
            { // unload
              mustStoreCmd("M702 T%d\n", tool_index);
              lastCmd = UNLOAD_STARTED;
            }
            else  // LOAD_REQUESTED
            { // load
              mustStoreCmd("M701 T%d\n", tool_index);
              lastCmd = LOAD_STARTED;
            }
         }
      }
    }

    loopProcess();
  }

  if (eAxisBackup.backedUp == false)  // the user exited from menu (not any other process/popup/etc)
  {
    mustStoreCmd("G92 E%.5f\n", eAxisBackup.coordinate);  // reset E axis position in Marlin to pre - load/unload state
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
