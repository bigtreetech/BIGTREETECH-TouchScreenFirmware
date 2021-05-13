#include "Extrude.h"
#include "includes.h"

const char *const extruderDisplayID[] = EXTRUDER_ID;
const char *const tool_change[] = TOOL_CHANGE;
static uint8_t curExtruder_index = 0;
static uint8_t extlenSteps_index = 1;
static uint8_t itemSpeed_index = 1;
static float extrudeCoordinate = 0.0f;

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
  float eBackup = 0.0f;
  float eTemp  = 0.0f;
  bool eRelative = false;
  uint32_t feedrate = 0;

  while (infoCmd.count != 0)
  {
    loopProcess();
  }
  extrudeCoordinate = eTemp = eBackup = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
  feedrate = coordinateGetFeedRate();
  eRelative = eGetRelative();

  if (eRelative) // Set extruder to absolute
    mustStoreCmd("M82\n");

  extrudeItems.items[KEY_ICON_4].icon = (infoSettings.ext_count > 1) ? ICON_NOZZLE : ICON_HEAT;
  extrudeItems.items[KEY_ICON_4].label.index = (infoSettings.ext_count > 1) ? LABEL_NOZZLE : LABEL_HEAT;
  extrudeItems.items[KEY_ICON_5] = itemExtLenSteps[extlenSteps_index];
  extrudeItems.items[KEY_ICON_6] = itemSpeed[itemSpeed_index];

  menuDrawPage(&extrudeItems);
  extruderReDraw(curExtruder_index, extrudeCoordinate, false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuExtrude)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        eTemp -= extlenSteps[extlenSteps_index];
        break;

      case KEY_INFOBOX:
      {
        float val = editFloatValue(extlenSteps[COUNT(extlenSteps) - 1] * -1, extlenSteps[COUNT(extlenSteps) - 1], 0, 0);
        eTemp += val;

        menuDrawPage(&extrudeItems);
        extruderReDraw(curExtruder_index, extrudeCoordinate, false);
        break;
      }

      case KEY_ICON_3:
        eTemp += extlenSteps[extlenSteps_index];
        break;

      case KEY_ICON_4:
        if (infoSettings.ext_count > 1)
        {
          curExtruder_index = (curExtruder_index + 1) % infoSettings.ext_count;

          extruderReDraw(curExtruder_index, extrudeCoordinate, false);
        }
        else
        {
          infoMenu.menu[++infoMenu.cur] = menuHeat;
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
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            eTemp += extlenSteps[extlenSteps_index] * encoderPosition;
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (extrudeCoordinate != eTemp)
    {
      if (curExtruder_index != heatGetCurrentTool())
        storeCmd("%s\n", tool_change[curExtruder_index]);

      if (warmupTemperature(curExtruder_index, &extrusionMinTemp_OK))
      {
        extrudeCoordinate = eTemp;
        storeCmd("G0 E%.5f F%d\n", extrudeCoordinate, infoSettings.ext_speed[itemSpeed_index]);

        extruderReDraw(curExtruder_index, extrudeCoordinate, true);
      }
    }

    loopProcess();
  }

  mustStoreCmd("G92 E%.5f\n", eBackup);
  mustStoreCmd("G0 F%d\n", feedrate);

  if (eRelative)
    mustStoreCmd("M83\n");  // Set extruder to relative
}
