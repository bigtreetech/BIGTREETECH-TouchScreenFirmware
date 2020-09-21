#include "ProbeOffset.h"
#include "includes.h"

#define ITEM_PROBE_OFFSET_UNIT_NUM 3

const ITEM itemProbeOffsetUnit[ITEM_PROBE_OFFSET_UNIT_NUM] = {
  // icon                           label
  {ICON_001_MM,                     LABEL_001_MM},
  {ICON_01_MM,                      LABEL_01_MM},
  {ICON_1_MM,                       LABEL_1_MM},
};

const float probeOffset_unit[ITEM_PROBE_OFFSET_UNIT_NUM] = {0.01f, 0.1f, 1};
static u8   curUnit = 0;

#define ITEM_PROBE_OFFSET_SUBMENU_NUM 4

ITEM itemProbeOffsetSubmenu[ITEM_PROBE_OFFSET_SUBMENU_NUM] = {
  // icon                           label
  {ICON_01_MM,                      LABEL_01_MM},
  {ICON_RESET_VALUE,                LABEL_RESET},
  {ICON_EEPROM_SAVE,                LABEL_SAVE},
  {ICON_DISABLE_STEPPERS,           LABEL_XY_UNLOCK},
};

static u8 curSubmenu = 0;

bool probe_offset_enabled = false;

/* Show an eror message notification */
void probeNotifyError(void)
{
  char tmpBuf[120];
  sprintf(tmpBuf, "%s %s", textSelect(LABEL_Z_OFFSET), textSelect(LABEL_OFF));
  addToast(DIALOG_TYPE_ERROR, tmpBuf);
}

/* Enable probe offset */
void probeEnableOffset(void)
{
  probe_offset_enabled = true;

  // Z offset gcode sequence start
  mustStoreCmd("M211 S0\n");                               // disable Software Endstop to move nozzle minus Zero (Z0) if necessary
  mustStoreCmd("G28\n");                                   // home printer
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 X%.2f Y%.2f\n",
    getParameter(P_PROBE_OFFSET, X_STEPPER),
    getParameter(P_PROBE_OFFSET, Y_STEPPER));              // move nozzle to XY probing point
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("G1 Z0 F%d\n",                              // move nozzle to Z0 absolute point and set feedrate
    infoSettings.axis_speed[infoSettings.move_speed]);
  mustStoreCmd("G91\n");                                   // set relative position mode
}

/* Disable probe offset */
void probeDisableOffset(void)
{
  probe_offset_enabled = false;

  // Z offset gcode sequence stop
  mustStoreCmd("G28\n");                                   // home printer
  mustStoreCmd("G91\n");                                   // set relative position mode
  mustStoreCmd("G1 Z%.2f\n", infoSettings.pause_z_raise ); // raise Z
  mustStoreCmd("G90\n");                                   // set absolute position mode
  mustStoreCmd("M211 S1\n");                               // enable Software Endstop
}

void showProbeOffsetStatus(u8 *status)
{
  char tempstr[20];

  if (status != NULL)
  {
    sprintf(tempstr, "%s  ", status);

    if (!probe_offset_enabled)
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
    // icon                         label
    {{ICON_DEC,                     LABEL_DEC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_INC,                     LABEL_INC},
     {ICON_PROBE_OFFSET,            LABEL_OFF},
     {ICON_PAGE_DOWN,               LABEL_NEXT},
     {ICON_001_MM,                  LABEL_001_MM},
     {ICON_BACK,                    LABEL_BACK},}
  };

  #if FRIENDLY_PROBE_OFFSET_LANGUAGE == 1
    probeOffsetItems.items[0].icon = ICON_NOZZLE_DOWN;
    probeOffsetItems.items[0].label.index = LABEL_DOWN;
    probeOffsetItems.items[3].icon = ICON_NOZZLE_UP;
    probeOffsetItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float z_offset_value;
  float now = z_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);

  if (!probe_offset_enabled)
    probeOffsetItems.items[KEY_ICON_4].label.index = LABEL_OFF;
  else
    probeOffsetItems.items[KEY_ICON_4].label.index = LABEL_ON;

  itemProbeOffsetSubmenu[0] = itemProbeOffsetUnit[curUnit];
  probeOffsetItems.items[KEY_ICON_6] = itemProbeOffsetSubmenu[curSubmenu];

  menuDrawPage(&probeOffsetItems);
  showProbeOffsetStatus(textSelect(probeOffsetItems.items[KEY_ICON_4].label.index));
  showProbeOffset(now);

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuProbeOffset)
  {
    z_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);
    float max_unit = probeOffset_unit[curUnit];

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease offset
      case KEY_ICON_0:
        if (!probe_offset_enabled)
          probeNotifyError();
        else if (z_offset_value > PROBE_OFFSET_MIN_VALUE)
        {
          float diff = z_offset_value - PROBE_OFFSET_MIN_VALUE;
          max_unit = (diff > max_unit) ? max_unit : diff;

          mustStoreCmd("M851 Z%.2f\n", z_offset_value - max_unit);
          mustStoreCmd("G1 Z%.2f\n", -max_unit);

          z_offset_value -= max_unit;
        }
        break;

      // increase offset
      case KEY_ICON_3:
        if (!probe_offset_enabled)
          probeNotifyError();
        else if (z_offset_value < PROBE_OFFSET_MAX_VALUE)
        {
          float diff = PROBE_OFFSET_MAX_VALUE - z_offset_value;
          max_unit = (diff > max_unit) ? max_unit : diff;

          mustStoreCmd("M851 Z%.2f\n", z_offset_value + max_unit);
          mustStoreCmd("G1 Z%.2f\n", max_unit);

          z_offset_value += max_unit;
        }
        break;

      // enable/disable offset change
      case KEY_ICON_4:
        if (!probe_offset_enabled)
        {
          probeEnableOffset();

          probeOffsetItems.items[key_num].label.index = LABEL_ON;
        }
        else
        {
          probeDisableOffset();

          probeOffsetItems.items[key_num].label.index = LABEL_OFF;
        }

        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        showProbeOffsetStatus(textSelect(probeOffsetItems.items[key_num].label.index));
        break;

      // change submenu
      case KEY_ICON_5:
        curSubmenu = (curSubmenu + 1) % ITEM_PROBE_OFFSET_SUBMENU_NUM;

        probeOffsetItems.items[KEY_ICON_6] = itemProbeOffsetSubmenu[curSubmenu];

        menuDrawItem(&probeOffsetItems.items[KEY_ICON_6], KEY_ICON_6);
        break;

      // handle submenu
      case KEY_ICON_6:
        switch (curSubmenu)
        {
          // change step unit
          case 0:
            curUnit = (curUnit + 1) % ITEM_PROBE_OFFSET_UNIT_NUM;

            itemProbeOffsetSubmenu[curSubmenu] = itemProbeOffsetUnit[curUnit];
            probeOffsetItems.items[key_num] = itemProbeOffsetSubmenu[curSubmenu];

            menuDrawItem(&probeOffsetItems.items[key_num], key_num);
            break;

          // reset offset to default value
          case 1:
            if (!probe_offset_enabled)
              probeNotifyError();
            else
            {
              if (z_offset_value != PROBE_OFFSET_DEFAULT_VALUE)
              {
                mustStoreCmd("M851 Z%.2f\n", PROBE_OFFSET_DEFAULT_VALUE);
                mustStoreCmd("G1 Z%.2f\n", -z_offset_value);

                z_offset_value = PROBE_OFFSET_DEFAULT_VALUE;
              }
            }
            break;

          // save to EEPROM
          case 2:
            if (infoMachineSettings.EEPROM == 1)
            {
              showDialog(DIALOG_TYPE_QUESTION, textSelect(probeOffsetItems.title.index), textSelect(LABEL_EEPROM_SAVE_INFO),
                textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
            }
            break;

          // unlock XY axis
          case 3:
            if (!probe_offset_enabled)
              probeNotifyError();
            else
              storeCmd("M84 X Y E\n");
            break;

          default:
            break;
        }
        break;

      case KEY_ICON_7:
        if (probe_offset_enabled)
          probeDisableOffset();

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!probe_offset_enabled)
              probeNotifyError();
            else
            {
              mustStoreCmd("M851 Z%.2f\n", z_offset_value + probeOffset_unit[curUnit] * encoderPosition);
              mustStoreCmd("G1 Z%.2f\n", probeOffset_unit[curUnit] * encoderPosition);

              z_offset_value += probeOffset_unit[curUnit] * encoderPosition;
            }

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != z_offset_value)
    {
      now = z_offset_value;
      showProbeOffset(now);

      // baby step is reset every time z-offset changes otherwise the set babystep value will not be aligned with the new z-offset
      babyStepReset();
    }

    loopProcess();
  }
}
