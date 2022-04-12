#include "MeshTuner.h"
#include "includes.h"

static uint8_t curUnit_index = 0;

// Init mesh point
static inline void meshInitPoint(uint16_t col, uint16_t row, float value)
{
//  probeHeightEnable();  // temporary disable software endstops and save ABL state

  // Z offset gcode sequence start
  mustStoreCmd("G42 I%d J%d\n", col, row);  // move nozzle to X and Y coordinates corresponding to the column and row in the bed leveling mesh grid
  probeHeightStart(value, false);           // lower nozzle to provided absolute Z point
  probeHeightRelative();                    // set relative position mode
}

// Reset mesh point
static inline void meshResetPoint(void)
{
  // Z offset gcode sequence stop
  if (infoMachineSettings.zProbe == ENABLED)
    probeHeightStop(infoSettings.level_z_raise);  // raise nozzle

  probeHeightAbsolute();  // set absolute position mode

//  probeHeightDisable();  // restore original software endstops state and ABL state
}

void meshDraw(uint16_t col, uint16_t row, COORDINATE *val)
{
  char tempstr[24], tempstr2[24], tempstr3[24];

  if (infoMachineSettings.leveling == BL_MBL)
    sprintf(tempstr2, "I:%d J:%d ZO:%.3f", col, row, infoParameters.MblOffset[0]);  // temp string
  else
    sprintf(tempstr2, "I:%d J:%d ZH:%.3f", col, row, val->axis[Z_AXIS] - infoSettings.level_z_pos);  // temp string

  sprintf(tempstr, "%-19s", tempstr2);               // I, J
  sprintf(tempstr2, "X:%.3f Y:%.3f", val->axis[X_AXIS], val->axis[Y_AXIS]);  // temp string
  sprintf(tempstr3, "%-19s", tempstr2);              // X, Y
  sprintf(tempstr2, "  %.3f  ", val->axis[Z_AXIS]);  // Z

  GUI_SetColor(infoSettings.status_color);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
  GUI_DispString(exhibitRect.x0, exhibitRect.y1 - BYTE_HEIGHT, (uint8_t *) tempstr3);

  GUI_SetColor(infoSettings.font_color);
  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr2);
  setFontSize(FONT_SIZE_NORMAL);
}

float menuMeshTuner(uint16_t col, uint16_t row, float value)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS meshItems = {
    // title
    LABEL_MESH_TUNER,
    // icon                          label
    {
      #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
        {ICON_NOZZLE_DOWN,             LABEL_DOWN},
      #else
        {ICON_DEC,                     LABEL_DEC},
      #endif
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
        {ICON_NOZZLE_UP,               LABEL_UP},
      #else
        {ICON_INC,                     LABEL_INC},
      #endif
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_APPLY,                   LABEL_CONFIRM},
      {ICON_STOP,                    LABEL_CANCEL},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  COORDINATE now, curValue;
  float unit;
  float shim;

  if (infoMachineSettings.leveling == BL_MBL)
    shim = 0.0f;
  else
    shim = infoSettings.level_z_pos;

  meshInitPoint(col, row, value + shim);  // initialize mesh point + shim

  coordinateGetAllActual(&now);

  INVERT_Z_AXIS_ICONS(&meshItems);
  meshItems.items[KEY_ICON_4] = itemMoveLen[curUnit_index];

  menuDrawPage(&meshItems);
  meshDraw(col, row, &now);

  while (true)
  {
    unit = moveLenSteps[curUnit_index];
    coordinateGetAllActual(&curValue);
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      // decrease Z height
      case KEY_ICON_0:
      case KEY_DECREASE:
        probeHeightMove(unit, -1);
        break;

      // increase Z height
      case KEY_ICON_3:
      case KEY_INCREASE:
        probeHeightMove(unit, 1);
        break;

      // change unit
      case KEY_ICON_4:
        curUnit_index = (curUnit_index + 1) % ITEM_FINE_MOVE_LEN_NUM;

        meshItems.items[key_num] = itemMoveLen[curUnit_index];

        menuDrawItem(&meshItems.items[key_num], key_num);
        break;

      // reset Z height
      case KEY_ICON_5:
        probeHeightMove(curValue.axis[Z_AXIS] - (value + shim), -1);
        break;

      // return new Z height
      case KEY_ICON_6:
        meshResetPoint();  // reset mesh point

        return curValue.axis[Z_AXIS] - shim;  // return current Z height - shim
        break;

      // return original Z height
      case KEY_ICON_7:
        meshResetPoint();  // reset mesh point

        return value;  // return original Z height
        break;

      default:
        break;
    }

    if (memcmp(&now, &curValue, sizeof(COORDINATE)))
    {
      coordinateGetAllActual(&now);
      meshDraw(col, row, &now);
    }

    probeHeightQueryCoord();

    loopProcess();

    if (MENU_IS_NOT(menuMeshEditor))
    {
      infoMenu.menu[infoMenu.cur]();

      menuDrawPage(&meshItems);
      meshDraw(col, row, &now);
    }
  }
}
