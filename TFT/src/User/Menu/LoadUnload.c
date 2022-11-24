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

const MENUITEMS loadUnloadItems = {
  // title
  LABEL_LOAD_UNLOAD,
  // icon                          label
  {
    {ICON_UNLOAD,                  LABEL_UNLOAD},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_LOAD,                    LABEL_LOAD},
    {ICON_NOZZLE,                  LABEL_NOZZLE},
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_COOLDOWN,                LABEL_COOLDOWN},
    {ICON_BACK,                    LABEL_BACK},
  }
};

static uint8_t tool_index = NOZZLE0;
CMD_TYPE lastCmd = NONE;

void menuLoadUnload(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  if (eAxisBackup.handled == false)
  {
    loopProcessToCondition(&isNotEmptyCmdQueue);  // wait for the communication to be clean

    eAxisBackup.coordinate = coordinateGetAxis(E_AXIS);
    eAxisBackup.handled = true;
  }

  menuDrawPage(&loadUnloadItems);
  temperatureReDraw(tool_index, NULL, true);

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  while (MENU_IS(menuLoadUnload))
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

        case KEY_INFOBOX:  // edit nozzle temp
        {
          int16_t actTarget = heatGetTargetTemp(tool_index);
          int16_t val = editIntValue(0, infoSettings.max_temp[tool_index], 0, actTarget);

          if (val != actTarget)
            heatSetTargetTemp(tool_index, val, FROM_GUI);

          temperatureReDraw(tool_index, NULL, true);
          lastCmd = NONE;
          break;
        }

        case KEY_ICON_4:  // nozzle select
          tool_index = (tool_index + 1) % infoSettings.hotend_count;

          temperatureReDraw(tool_index, NULL, true);
          lastCmd = NONE;
          break;

        case KEY_ICON_5:  // heat menu
          heatSetCurrentIndex(tool_index);  // preselect current nozzle for "Heat" menu
          OPEN_MENU(menuHeat);
          eAxisBackup.handled = false;  // exiting from Extrude menu (user might never come back by "Back" long press in Heat menu)
          lastCmd = NONE;
          break;

        case KEY_ICON_6:  // cool down nozzle
          heatCoolDown();
          lastCmd = NONE;
          break;

        case KEY_ICON_7:  // back
          COOLDOWN_TEMPERATURE();
          lastCmd = NONE;
          CLOSE_MENU();
          eAxisBackup.handled = false;  // the user exited from menu (not any other process/popup/etc)
          break;

        default:
          temperatureReDraw(tool_index, NULL, false);
          break;
      }

      if ((lastCmd == UNLOAD_REQUESTED) || (lastCmd == LOAD_REQUESTED))
      {
        if (tool_index != heatGetCurrentTool())
          mustStoreCmd("%s\n", tool_change[tool_index]);

        switch (warmupNozzle())
        {
          case COLD:
            lastCmd = NONE;
            break;

          case SETTLING:
            break;

          case HEATED:
            if (lastCmd == UNLOAD_REQUESTED)
            { // unload
              if (infoMachineSettings.firmwareType != FW_REPRAPFW)
                mustStoreCmd("M702\n");
              else
                request_M98("sys/unload.g");

              lastCmd = UNLOAD_STARTED;
            }
            else  // LOAD_REQUESTED
            { // load
              if (infoMachineSettings.firmwareType != FW_REPRAPFW)
                mustStoreCmd("M701\n");
              else
                request_M98("sys/load.g");

              lastCmd = LOAD_STARTED;
            }
            if (isPrinting() && isPaused())
            {
              setExtrusionDuringPause(true);
            }
         }
      }
    }

    loopProcess();
  }

  if (eAxisBackup.handled == false)  // the user exited from menu (not any other process/popup/etc)
  {
    mustStoreCmd("G92 E%.5f\n", eAxisBackup.coordinate);  // reset E axis position in Marlin to pre - load/unload state
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
