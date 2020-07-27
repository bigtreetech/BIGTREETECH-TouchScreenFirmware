#include "ProbeOffset.h"
#include "includes.h"

#define PROBE_OFFSET_MIN_VALUE     -20.0f
#define PROBE_OFFSET_MAX_VALUE     20.0f
#define PROBE_OFFSET_DEFAULT_VALUE 0.0f

#define ITEM_PROBE_OFFSET_UNIT_NUM 3

const ITEM itemProbeOffsetUnit[ITEM_PROBE_OFFSET_UNIT_NUM] = {
// icon                       label
  {ICON_001_MM,               LABEL_001_MM},
  {ICON_01_MM,                LABEL_01_MM},
  {ICON_1_MM,                 LABEL_1_MM},
};

const float probeOffset_unit[ITEM_PROBE_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};

static u8 curUnit = 0;

bool z_offset_started = false;

/* Start Z offset */
void probeStartZOffset(void)
{
  // Z offset gcode sequence start
  mustStoreCmd("G28\n");                                   // home printer
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 X%.2f Y%.2f\n",
    getParameter(P_PROBE_OFFSET, X_STEPPER),
    getParameter(P_PROBE_OFFSET, Y_STEPPER));              // move nozzle to XY probing point
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("G1 Z0 F800\n");                            // move nozzle to Z0 absolute point
  mustStoreCmd("M211 S0\n");                               // disable Software Endstop to move nozzle minus Zero (Z0) if necessary
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 F%d\n",
    infoSettings.axis_speed[infoSettings.move_speed]);     // set feed rate
}

/* Stop Z offset */
void probeStopZOffset(void)
{
  // Z offset gcode sequence stop
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("M211 S1\n");                               // enable Software Endstop
  mustStoreCmd("G28\n");                                   // home printer
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 Z%.2f\n", infoSettings.pause_z_raise ); // raise Z
  mustStoreCmd("G90\n");                                   // set absolute position mode
}

void showProbeOffsetStatus(u8 *status)
{
  char tempstr[20];

  if (status != NULL)
  {
    sprintf(tempstr, "%s  ", status);

    if (!z_offset_started)
      GUI_SetColor(infoSettings.reminder_color);
    else
      GUI_SetColor(infoSettings.sd_reminder_color);

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
    GUI_SetColor(infoSettings.font_color);
  }
}

void showProbeOffset(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void menuProbeOffset(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS probeOffsetItems = {
  // title
  LABEL_Z_OFFSET,
  // icon                       label
   {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_PROBE_OFFSET,         LABEL_OFF},
    {ICON_01_MM,                LABEL_01_MM},
    {ICON_RESET_VALUE,          LABEL_RESET},
    {ICON_BACK,                 LABEL_BACK},}
  };

  #ifdef FRIENDLY_PROBE_OFFSET_LANGUAGE = 1
    probeOffsetItems.items[0].icon = ICON_NOZZLE_DOWN;
    probeOffsetItems.items[0].label.index = LABEL_NOZZLE_DOWN;
    probeOffsetItems.items[3].icon = ICON_NOZZLE_UP;
    probeOffsetItems.items[3].label.index = LABEL_NOZZLE_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float orig_probe_offset_value, probe_offset_value;
  float now = probe_offset_value = orig_probe_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);

  if (!z_offset_started)
    probeOffsetItems.items[KEY_ICON_4].label.index = LABEL_OFF;
  else
    probeOffsetItems.items[KEY_ICON_4].label.index = LABEL_ON;

  probeOffsetItems.items[KEY_ICON_5] = itemProbeOffsetUnit[curUnit];
  menuDrawPage(&probeOffsetItems);
  showProbeOffsetStatus(textSelect(probeOffsetItems.items[KEY_ICON_4].label.index));
  showProbeOffset(now);

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuProbeOffset)
  {
    probe_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);
    float max_unit = probeOffset_unit[curUnit];

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease z-offset
      case KEY_ICON_0:
        if (probe_offset_value > PROBE_OFFSET_MIN_VALUE && z_offset_started)
        {
          float diff = probe_offset_value - PROBE_OFFSET_MIN_VALUE;
          max_unit = (diff > max_unit) ? max_unit : diff;

          mustStoreCmd("M851 Z%.2f\n", probe_offset_value - max_unit);
          mustStoreCmd("G1 Z%.2f\n", -max_unit);

          probe_offset_value -= max_unit;
        }
        break;

      // increase z-offset
      case KEY_ICON_3:
        if (probe_offset_value < PROBE_OFFSET_MAX_VALUE && z_offset_started)
        {
          float diff = PROBE_OFFSET_MAX_VALUE - probe_offset_value;
          max_unit = (diff > max_unit) ? max_unit : diff;

          mustStoreCmd("M851 Z%.2f\n", probe_offset_value + max_unit);
          mustStoreCmd("G1 Z%.2f\n", max_unit);

          probe_offset_value += max_unit;
        }
        break;

      case KEY_ICON_4:
        if (!z_offset_started)
        {
          probeStartZOffset();
          z_offset_started = true;

          probeOffsetItems.items[key_num].label.index = LABEL_ON;
        }
        else
        {
          probeStopZOffset();
          z_offset_started = false;

          probeOffsetItems.items[key_num].label.index = LABEL_OFF;
        }
        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        showProbeOffsetStatus(textSelect(probeOffsetItems.items[key_num].label.index));
        break;

      // change step unit
      case KEY_ICON_5:
        curUnit = (curUnit + 1) % ITEM_PROBE_OFFSET_UNIT_NUM;
        probeOffsetItems.items[key_num] = itemProbeOffsetUnit[curUnit];
        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        break;

      // reset z-offset to default value
      case KEY_ICON_6:
        if (probe_offset_value != PROBE_OFFSET_DEFAULT_VALUE && z_offset_started)
        {
          mustStoreCmd("M851 Z%.2f\n", PROBE_OFFSET_DEFAULT_VALUE);
          mustStoreCmd("G1 Z%.2f\n", -probe_offset_value);

          probe_offset_value = PROBE_OFFSET_DEFAULT_VALUE;
        }
        break;

      case KEY_ICON_7:
        if (probe_offset_value != orig_probe_offset_value && infoMachineSettings.EEPROM == 1)
        {
          showDialog(DIALOG_TYPE_QUESTION, textSelect(probeOffsetItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                     textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
        }
        else
        {
          if (z_offset_started)
          {
            probeStopZOffset();
            z_offset_started = false;
          }

          infoMenu.cur--;
        }
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition && z_offset_started)
          {
            mustStoreCmd("M851 Z%.2f\n", probe_offset_value + probeOffset_unit[curUnit] * encoderPosition);
            mustStoreCmd("G1 Z%.2f\n", probeOffset_unit[curUnit] * encoderPosition);

            probe_offset_value += probeOffset_unit[curUnit] * encoderPosition;
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != probe_offset_value)
    {
      now = probe_offset_value;
      showProbeOffset(now);

      // baby step is reset every time z-offset changes otherwise the set babystep value will not be aligned with the new z-offset
      babyStepReset();
    }

    loopProcess();
  }
}
