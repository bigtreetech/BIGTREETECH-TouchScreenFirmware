#include "Extrude.h"
#include "includes.h"

static u8 curExtruder_index = 0;
static u8 extlenSteps_index = 1;
static u8 itemSpeed_index = 1;
static float extrudeCoordinate = 0.0f;
const char *const tool_change[] = TOOL_CHANGE;
const char *const extruderDisplayID[] = EXTRUDER_ID;

void extrudeCoordinateReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", extruderDisplayID[curExtruder_index]);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  }

  sprintf(tempstr, "  %.2f  ", extrudeCoordinate);
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void menuExtrude(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  float eSaved = 0.0f;
  float eTemp  = 0.0f;
  bool eRelative = false;
  u32 feedrate = 0;

  MENUITEMS extrudeItems = {
    // title
    LABEL_EXTRUDE,
    // icon                      label
    {{ICON_UNLOAD,               LABEL_UNLOAD},
     {ICON_BACKGROUND,           LABEL_BACKGROUND},
     {ICON_BACKGROUND,           LABEL_BACKGROUND},
     {ICON_LOAD,                 LABEL_LOAD},
     {ICON_NOZZLE,               LABEL_NOZZLE},
     {ICON_E_5_MM,               LABEL_5_MM},
     {ICON_NORMAL_SPEED,         LABEL_NORMAL_SPEED},
     {ICON_BACK,                 LABEL_BACK},}
  };

  while (infoCmd.count != 0)
  {
    loopProcess();
  }
  extrudeCoordinate = eTemp = eSaved = coordinateGetAxisTarget(E_AXIS);
  feedrate = coordinateGetFeedRate();
  eRelative = eGetRelative();

  menuDrawPage(&extrudeItems);
  extrudeCoordinateReDraw(false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  if (eRelative) // Set extruder to absolute
    mustStoreCmd("M82\n");

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
          float val = 0;
          char titlestr[30];

          sprintf(titlestr, "Min:%i | Max:%i", (extlenSteps[COUNT(extlenSteps) - 1]) * -1, extlenSteps[COUNT(extlenSteps) - 1]);
          val = numPadFloat((u8 *) titlestr, 0, 0, true);
          eTemp += val;

          menuDrawPage(&extrudeItems);
          extrudeCoordinateReDraw(false);
        }
        break;

      case KEY_ICON_3:
        eTemp += extlenSteps[extlenSteps_index];
        break;

      case KEY_ICON_4:
        curExtruder_index = (curExtruder_index + 1) % infoSettings.ext_count;
        extrudeCoordinateReDraw(false);
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
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            eTemp += extlenSteps[extlenSteps_index]*encoderPosition;
            encoderPosition = 0;
          }
        #endif
        break;
    }
    if (extrudeCoordinate != eTemp)
    {
      if (curExtruder_index != heatGetCurrentTool())
        storeCmd("%s\n", tool_change[curExtruder_index]);

      if (heatGetCurrentTemp(curExtruder_index) < infoSettings.min_ext_temp)
      {
        char tempMsg[120];
        labelChar(tempStr, LABEL_EXT_TEMPLOW);
        sprintf(tempMsg, tempStr, infoSettings.min_ext_temp);
        popupReminder(DIALOG_TYPE_ERROR,LABEL_COLD_EXT, (u8*)tempMsg);
      }
      else
      {
        extrudeCoordinateReDraw(true);
        extrudeCoordinate = eTemp;
        storeCmd("G0 E%.5f F%d\n", extrudeCoordinate, infoSettings.ext_speed[itemSpeed_index]);
      }
    }
    loopProcess();
  }
  mustStoreCmd("G92 E%.5f\n",eSaved);
  mustStoreCmd("G0 F%d\n",feedrate);
  if (eRelative)
    mustStoreCmd("M83\n"); // Set extruder to relative
}
