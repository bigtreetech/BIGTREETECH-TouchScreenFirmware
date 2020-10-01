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

static float z_offset;

/* Show an eror message notification */
void probeNotifyError(void)
{
  char tmpBuf[120];
  sprintf(tmpBuf, "%s %s", textSelect(LABEL_Z_OFFSET), textSelect(LABEL_OFF));
  addToast(DIALOG_TYPE_ERROR, tmpBuf);
}

void probeDrawStatus(u8 *status)
{
  char tempstr[20];

  if (status != NULL)
  {
    sprintf(tempstr, "%s  ", status);

    if (!probeOffsetGetStatus())
      GUI_SetColor(infoSettings.reminder_color);
    else
      GUI_SetColor(infoSettings.sd_reminder_color);

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *) tempstr);
    GUI_SetColor(infoSettings.font_color);
  }
}

void probeDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *) tempstr);
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
  float now = z_offset = probeOffsetGetValue();
  float unit;

  if (!probeOffsetGetStatus())
    probeOffsetItems.items[KEY_ICON_4].label.index = LABEL_OFF;
  else
    probeOffsetItems.items[KEY_ICON_4].label.index = LABEL_ON;

  itemProbeOffsetSubmenu[0] = itemProbeOffsetUnit[curUnit];
  probeOffsetItems.items[KEY_ICON_6] = itemProbeOffsetSubmenu[curSubmenu];

  menuDrawPage(&probeOffsetItems);
  probeDrawStatus(textSelect(probeOffsetItems.items[KEY_ICON_4].label.index));
  probeDrawValue(now);

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuProbeOffset)
  {
    unit = probeOffset_unit[curUnit];

    z_offset = probeOffsetGetValue();                      // always load the current Z offset

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease Z offset
      case KEY_ICON_0:
        if (!probeOffsetGetStatus())
          probeNotifyError();
        else
          z_offset = probeOffsetDecreaseValue(unit);
        break;

      // increase Z offset
      case KEY_ICON_3:
        if (!probeOffsetGetStatus())
          probeNotifyError();
        else
          z_offset = probeOffsetIncreaseValue(unit);
        break;

      // enable/disable Z offset change
      case KEY_ICON_4:
        if (!probeOffsetGetStatus())
        {
          probeOffsetEnable();

          probeOffsetItems.items[key_num].label.index = LABEL_ON;
        }
        else
        {
          probeOffsetDisable();

          probeOffsetItems.items[key_num].label.index = LABEL_OFF;
        }

        menuDrawItem(&probeOffsetItems.items[key_num], key_num);
        probeDrawStatus(textSelect(probeOffsetItems.items[key_num].label.index));
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
          // change unit
          case 0:
            curUnit = (curUnit + 1) % ITEM_PROBE_OFFSET_UNIT_NUM;

            itemProbeOffsetSubmenu[curSubmenu] = itemProbeOffsetUnit[curUnit];
            probeOffsetItems.items[key_num] = itemProbeOffsetSubmenu[curSubmenu];

            menuDrawItem(&probeOffsetItems.items[key_num], key_num);
            break;

          // reset Z offset to default value
          case 1:
            if (!probeOffsetGetStatus())
              probeNotifyError();
            else
              z_offset = probeOffsetResetValue();
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
            if (!probeOffsetGetStatus())
              probeNotifyError();
            else
              storeCmd("M84 X Y E\n");
            break;

          default:
            break;
        }
        break;

      case KEY_ICON_7:
        if (probeOffsetGetStatus())
          probeOffsetDisable();

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!probeOffsetGetStatus())
              probeNotifyError();
            else
              z_offset = probeOffsetUpdateValueByEncoder(unit);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != z_offset)
    {
      now = z_offset;
      probeDrawValue(now);

      // babystep is reset every time Z offset changes otherwise the set babystep value will not be aligned with the new Z offset
      babyReset();
    }

    loopProcess();
  }
}
