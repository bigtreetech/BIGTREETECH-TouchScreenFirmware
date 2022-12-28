#include "Extrude.h"
#include "includes.h"

const char *const extruderDisplayID[] = EXTRUDER_ID;
const char *const tool_change[] = TOOL_CHANGE;
static uint8_t curExtruder_index = 0;
static uint8_t extlenSteps_index = 1;
static uint8_t itemSpeed_index = 1;

void menuExtrude(void)
{
  MENUITEMS extrudeItems = {
    // title
    LABEL_EXTRUDE,
    // icon                          label
    {
      {ICON_UNLOAD,                  LABEL_UNLOAD},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_LOAD,                    LABEL_LOAD},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_E_5_MM,                  LABEL_5_MM},
      {ICON_NORMAL_SPEED,            LABEL_NORMAL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  float extrLength = 0.0f;
  float extrAmount = 0.0f;

  extrudeItems.items[KEY_ICON_4].icon = (infoSettings.ext_count > 1) ? ICON_NOZZLE : ICON_HEAT;
  extrudeItems.items[KEY_ICON_4].label.index = (infoSettings.ext_count > 1) ? LABEL_NOZZLE : LABEL_HEAT;
  extrudeItems.items[KEY_ICON_5] = itemExtLenSteps[extlenSteps_index];
  extrudeItems.items[KEY_ICON_6] = itemSpeed[itemSpeed_index];

  menuDrawPage(&extrudeItems);

  if (eAxisBackup.handled == false)
  {
    loopProcessToCondition(&isNotEmptyCmdQueue);  // wait for the communication to be clean

    eAxisBackup.coordinate = coordinateGetAxis(E_AXIS);
    eAxisBackup.feedrate = coordinateGetFeedRate();
    eAxisBackup.relative = eGetRelative();
    eAxisBackup.handled = true;
  }
  else
  {
    extrAmount = coordinateGetAxis(E_AXIS) - eAxisBackup.coordinate;
  }

  extruderReDraw(curExtruder_index, extrAmount, true);

  if (eAxisBackup.relative == false) // Set extruder to relative
  {
    mustStoreCmd("M83\n");
  }

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  while (MENU_IS(menuExtrude))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        extrLength = 0 - extlenSteps[extlenSteps_index];
        break;

      case KEY_INFOBOX:
        extrLength = editFloatValue(0 - extlenSteps[COUNT(extlenSteps) - 1], extlenSteps[COUNT(extlenSteps) - 1], 0, 0);
        extruderReDraw(curExtruder_index, extrAmount, true);
        break;

      case KEY_ICON_3:
      case KEY_INCREASE:
        extrLength = extlenSteps[extlenSteps_index];
        break;

      case KEY_ICON_4:
        if (infoSettings.ext_count > 1)
        {
          curExtruder_index = (curExtruder_index + 1) % infoSettings.ext_count;
          extruderReDraw(curExtruder_index, extrAmount, true);
        }
        else
        {
          heatSetCurrentIndex(curExtruder_index);  // preselect current nozzle for "Heat" menu
          OPEN_MENU(menuHeat);
          menuHeat();  // call from here to retain E axis parameters

          if (MENU_IS(menuExtrude))  // user exited from heating menu by short pressing "Back"
          {
            menuDrawPage(&extrudeItems);
            extruderReDraw(curExtruder_index, extrAmount, true);
          }
          else  // user exited from heating menu by long pressing "Back"
          {
            eAxisBackup.handled = false;  // exiting from Extrude menu, trigger E axis parameters restore
          }
        }
        break;

      case KEY_ICON_5:
        extlenSteps_index = (extlenSteps_index + 1) % ITEM_EXT_LEN_NUM;
        extrudeItems.items[key_num] = itemExtLenSteps[extlenSteps_index];

        menuDrawItem(&extrudeItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        itemSpeed_index = (itemSpeed_index + 1) % ITEM_SPEED_NUM;
        extrudeItems.items[key_num] = itemSpeed[itemSpeed_index];

        menuDrawItem(&extrudeItems.items[key_num], key_num);
        break;

      case KEY_ICON_7:
        COOLDOWN_TEMPERATURE();
        CLOSE_MENU();
        eAxisBackup.handled = false;  // exiting from Extrude menu, trigger E axis parameters restore
        break;

      default:
        break;
    }

    if (extrLength != 0)
    {
      if (curExtruder_index != heatGetCurrentTool())
        mustStoreCmd("%s\n", tool_change[curExtruder_index]);

      switch (warmupNozzle())
      {
        case COLD:
          extrLength = 0;
          break;

        case SETTLING:
          extruderReDraw(curExtruder_index, extrAmount + extrLength, false);
          break;

        case HEATED:
          if (storeCmd("G1 E%.5f F%d\n", extrLength, infoSettings.ext_speed[itemSpeed_index]))
          {
            extrAmount += extrLength;
            extrLength = 0;
            extruderReDraw(curExtruder_index, extrAmount, false);
            if (isPrinting() && isPaused())
            {
              setExtrusionDuringPause(true);
            }
          }
          break;
      }
    }

    loopProcess();
  }

  if (eAxisBackup.handled == false)  // the user exited from menu (not any other process/popup/etc)
  { // restore E axis coordinate, feedrate and relativeness to pre-extrude state
    mustStoreCmd("G92 E%.5f\n", eAxisBackup.coordinate);
    mustStoreCmd("G0 F%d\n", eAxisBackup.feedrate);

    if (eAxisBackup.relative == false)
      mustStoreCmd("M82\n");  // Set extruder to absolute
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
