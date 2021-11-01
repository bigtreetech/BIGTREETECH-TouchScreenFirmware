#include "Extrude.h"
#include "includes.h"

const char *const extruderDisplayID[] = EXTRUDER_ID;
const char *const tool_change[] = TOOL_CHANGE;
static uint8_t curExtruder_index = 0;
static uint8_t extlenSteps_index = 1;
static uint8_t itemSpeed_index = 1;

// set the hotend to the minimum extrusion temperature if user selected "OK"
void extrusionMinTemp_OK(void)
{
  heatSetTargetTemp(curExtruder_index, infoSettings.min_ext_temp);
}

void menuExtrude(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS extrudeItems = {
    // title
    LABEL_EXTRUDE,
    // icon                          label
    {
      {ICON_UNLOAD,                  LABEL_UNLOAD},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_LOAD,                    LABEL_LOAD},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_E_5_MM,                  LABEL_5_MM},
      {ICON_NORMAL_SPEED,            LABEL_NORMAL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  float extrNewCoord  = 0.0f;
  float extrKnownCoord = 0.0f;

  if (eAxisBackup.handled == false)
  {
    loopProcessToCondition(&isNotEmptyCmdQueue);  // wait for the communication to be clean

    eAxisBackup.coordinate = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
    eAxisBackup.feedrate = coordinateGetFeedRate();
    eAxisBackup.relative = eGetRelative();
    eAxisBackup.handled = true;
  }

  extrKnownCoord = extrNewCoord = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));

  if (eAxisBackup.relative) // Set extruder to absolute
    mustStoreCmd("M82\n");

  extrudeItems.items[KEY_ICON_4].icon = (infoSettings.ext_count > 1) ? ICON_NOZZLE : ICON_HEAT;
  extrudeItems.items[KEY_ICON_4].label.index = (infoSettings.ext_count > 1) ? LABEL_NOZZLE : LABEL_HEAT;
  extrudeItems.items[KEY_ICON_5] = itemExtLenSteps[extlenSteps_index];
  extrudeItems.items[KEY_ICON_6] = itemSpeed[itemSpeed_index];

  menuDrawPage(&extrudeItems);
  extruderReDraw(curExtruder_index, extrKnownCoord, false);

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  while (MENU_IS(menuExtrude))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        extrNewCoord -= extlenSteps[extlenSteps_index];
        break;

      case KEY_INFOBOX:
      {
        float val = editFloatValue(extlenSteps[COUNT(extlenSteps) - 1] * -1, extlenSteps[COUNT(extlenSteps) - 1], 0, 0);
        extrNewCoord += val;

        extruderReDraw(curExtruder_index, extrKnownCoord, false);
        break;
      }

      case KEY_ICON_3:
      case KEY_INCREASE:
        extrNewCoord += extlenSteps[extlenSteps_index];
        break;

      case KEY_ICON_4:
        if (infoSettings.ext_count > 1)
        {
          curExtruder_index = (curExtruder_index + 1) % infoSettings.ext_count;
          extruderReDraw(curExtruder_index, extrKnownCoord, false);
        }
        else
        {
          heatSetCurrentIndex(currentTool);  // preselect current nozzle for "Heat" menu
          OPEN_MENU(menuHeat);
          eAxisBackup.handled = false;  // exiting from Extrude menu (user might never come back by "Back" long press in Heat menu)
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
        cooldownTemperature();
        CLOSE_MENU();
        eAxisBackup.handled = false;  // exiting from Extrude menu, no need for it anymore
        break;

      default:
        break;
    }

    if (extrKnownCoord != extrNewCoord)
    {
      if (curExtruder_index != heatGetCurrentTool())
        storeCmd("%s\n", tool_change[curExtruder_index]);

      switch (warmupNozzle(curExtruder_index, &extrusionMinTemp_OK))
      {
        case COLD:
          extrNewCoord = extrKnownCoord;
          break;

        case SETTLING:
          extruderReDraw(curExtruder_index, extrNewCoord, true);
          break;

        case HEATED:
          if (storeCmd("G0 E%.5f F%d\n", extrNewCoord, infoSettings.ext_speed[itemSpeed_index]))
          {
            extrKnownCoord = extrNewCoord;
            extruderReDraw(curExtruder_index, extrNewCoord, true);
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

    if (eAxisBackup.relative == true)
      mustStoreCmd("M83\n");  // Set extruder to relative
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
