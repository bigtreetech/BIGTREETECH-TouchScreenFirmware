#include "LoadUnload.h"
#include "includes.h"

#define NONE   0
#define LOAD   1
#define UNLOAD 2

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
static uint8_t lastCmd = NONE;

// set the hotend to the minimum extrusion temperature if user selected "OK"
void loadMinTemp_OK(void)
{
  heatSetTargetTemp(tool_index, infoSettings.min_ext_temp);
}

void menuLoadUnload(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  while (infoCmd.count != 0)
  {
    loopProcess();
  }

  menuDrawPage(&loadUnloadItems);
  temperatureReDraw(tool_index, NULL, false);

  while (infoMenu.menu[infoMenu.cur] == menuLoadUnload)
  {
    key_num = menuKeyGetValue();

    if (infoHost.wait == true && key_num != KEY_IDLE &&
        key_num != KEY_ICON_7)  // show reminder for process running if presses any button other than back button
    {
      if (lastCmd == UNLOAD)
      { // unloading
        popupReminder(DIALOG_TYPE_INFO, LABEL_UNLOAD, LABEL_UNLOAD_STARTED);
      }
      else if (lastCmd == LOAD)
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
      switch (key_num)
      {
        case KEY_ICON_0:  // Unload
        case KEY_ICON_3:  // Load
          if (!warmupTemperature(tool_index, loadMinTemp_OK))
          {
          }
          else if (key_num == KEY_ICON_0)
          { // unload
            mustStoreCmd("M702 T%d\n", tool_index);
            lastCmd = UNLOAD;
          }
          else
          { // load
            mustStoreCmd("M701 T%d\n", tool_index);
            lastCmd = LOAD;
          }
          break;

        case KEY_ICON_4:
          tool_index = (tool_index + 1) % infoSettings.hotend_count;

          temperatureReDraw(tool_index, NULL, false);
          lastCmd = NONE;
          break;

        case KEY_ICON_5:
          infoMenu.menu[++infoMenu.cur] = menuHeat;
          lastCmd = NONE;
          break;

        case KEY_ICON_6:
          heatCoolDown();
          lastCmd = NONE;
          break;

        case KEY_ICON_7:
          cooldownTemperature();
          lastCmd = NONE;
          infoMenu.cur--;
          break;

        default:
          temperatureReDraw(tool_index, NULL, true);
          break;
      }
    }

    loopProcess();
  }
}
