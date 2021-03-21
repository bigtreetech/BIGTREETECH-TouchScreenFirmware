#include "MeshTuner.h"
#include "includes.h"

static uint8_t curUnit_index = 0;

// Init mesh point
static inline void meshInitPoint(uint16_t col, uint16_t row, float value)
{
//  probeHeightEnable();  // temporary disable software endstops

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

//  probeHeightDisable();  // restore original software endstops state
}

void meshDrawHeader(uint16_t col, uint16_t row)
{
  char tempstr[20];

  sprintf(tempstr, "I: %d  J: %d", col, row);

  GUI_SetColor(infoSettings.sd_reminder_color);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
  GUI_SetColor(infoSettings.font_color);
}

void meshDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.3f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

float menuMeshTuner(uint16_t col, uint16_t row, float value)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS meshItems = {
    // title
    LABEL_MESH_TUNER,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_APPLY,                   LABEL_CONFIRM},
      {ICON_STOP,                    LABEL_CANCEL},
    }
  };

  #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
    meshItems.items[0].icon = ICON_NOZZLE_DOWN;
    meshItems.items[0].label.index = LABEL_DOWN;
    meshItems.items[3].icon = ICON_NOZZLE_UP;
    meshItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now, curValue;
  float unit;

  meshInitPoint(col, row, value);  // initialize mesh point

  now = curValue = coordinateGetAxisActual(Z_AXIS);

  meshItems.items[KEY_ICON_4] = itemMoveLen[curUnit_index];

  menuDrawPage(&meshItems);
  meshDrawHeader(col, row);
  meshDrawValue(now);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (true)
  {
    unit = moveLenSteps[curUnit_index];

    curValue = coordinateGetAxisActual(Z_AXIS);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease Z height
      case KEY_ICON_0:
        probeHeightMove(unit, -1);
        break;

      // increase Z height
      case KEY_ICON_3:
        probeHeightMove(unit, 1);
        break;

      // change unit
      case KEY_ICON_4:
        curUnit_index = (curUnit_index + 1) % ITEM_FINE_MOVE_LEN_NUM;

        meshItems.items[key_num] = itemMoveLen[curUnit_index];

        menuDrawItem(&meshItems.items[key_num], key_num);
        break;

      // reset Z height to 0
      case KEY_ICON_5:
        probeHeightMove(curValue, -1);
        break;

      // return new Z height
      case KEY_ICON_6:
        meshResetPoint();  // reset mesh point

        return curValue;  // return current Z height
        break;

      // return original Z height
      case KEY_ICON_7:
        meshResetPoint();  // reset mesh point

        return value;  // return original Z height
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            probeHeightMove(unit, encoderPosition > 0 ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != curValue)
    {
      now = curValue;
      meshDrawValue(now);
    }

    probeHeightQueryCoord();

    loopProcess();

    if (infoMenu.menu[infoMenu.cur] != menuMeshEditor)
    {
      infoMenu.menu[infoMenu.cur]();

      menuDrawPage(&meshItems);
      meshDrawHeader(col, row);
      meshDrawValue(now);
    }
  }
}
