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
  // title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS probeOffsetItems = {
  // title
  LABEL_Z_OFFSET,
  // icon                       label
   {{ICON_DEC,                  LABEL_DEC},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_BACKGROUND,           LABEL_BACKGROUND},
    {ICON_INC,                  LABEL_INC},
    {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
    {ICON_01_MM,                LABEL_01_MM},
    {ICON_RESET_VALUE,          LABEL_RESET},
    {ICON_BACK,                 LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  float probe_offset_value;
  float now = probe_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);

  probeOffsetItems.items[KEY_ICON_5] = itemProbeOffsetUnit[curUnit];
  menuDrawPage(&probeOffsetItems);
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
      // decrease offset
      case KEY_ICON_0:
        if (probe_offset_value > PROBE_OFFSET_MIN_VALUE)
        {
          float diff = probe_offset_value - PROBE_OFFSET_MIN_VALUE;
          max_unit = (diff > max_unit) ? max_unit : diff;

          if (storeCmd("M851 Z%.2f\n", probe_offset_value - max_unit))
            probe_offset_value -= max_unit;
        }
        break;

      // increase offset
      case KEY_ICON_3:
        if (probe_offset_value < PROBE_OFFSET_MAX_VALUE)
        {
          float diff = PROBE_OFFSET_MAX_VALUE - probe_offset_value;
          max_unit = (diff > max_unit) ? max_unit : diff;

          if (storeCmd("M851 Z%.2f\n", probe_offset_value + max_unit))
            probe_offset_value += max_unit;
        }
        break;

      //save to eeprom
      case KEY_ICON_4:
        if (infoMachineSettings.EEPROM == 1)
        {
          storeCmd("M500\n");
        }
        break;

      // change step unit
      case KEY_ICON_5:
        curUnit = (curUnit + 1) % ITEM_PROBE_OFFSET_UNIT_NUM;
        probeOffsetItems.items[key_num] = itemProbeOffsetUnit[curUnit];
        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        break;

      // reset offset to default value
      case KEY_ICON_6:
        if (storeCmd("M851 Z%.2f\n", PROBE_OFFSET_DEFAULT_VALUE))
          probe_offset_value = PROBE_OFFSET_DEFAULT_VALUE;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            storeCmd("M851 Z%.2f\n", probe_offset_value + probeOffset_unit[curUnit] * encoderPosition);
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
