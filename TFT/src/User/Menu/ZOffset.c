#include "ZOffset.h"
#include "includes.h"

static bool probeOffsetMenu = false;
static uint8_t curUnit_index = 0;
static uint8_t curSubmenu_index = 0;

// Show an error notification
void zOffsetNotifyError(bool isStarted)
{
  LABELCHAR(tempMsg, LABEL_PROBE_OFFSET)

  if (!probeOffsetMenu)
    sprintf(tempMsg, "%s", textSelect(LABEL_HOME_OFFSET));

  if (!isStarted)
    sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_OFF));
  else
    sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_ON));

  addToast(DIALOG_TYPE_ERROR, tempMsg);
}

void zOffsetDraw(bool status, float val)
{
  char tempstr[20], tempstr2[20], tempstr3[30];

  if (!status)
  {
    sprintf(tempstr, "%-15s", textSelect(itemToggle[status].index));
    sprintf(tempstr3, "%-15s", "");
    sprintf(tempstr2, "  %.2f  ", val);

    GUI_SetColor(infoSettings.reminder_color);
  }
  else
  {
    sprintf(tempstr, "ZO:%.2f  ", val);
    sprintf(tempstr3, "%s:%.3f", textSelect(LABEL_SHIM), infoSettings.level_z_pos);
    sprintf(tempstr2, "  %.2f  ", val + infoSettings.level_z_pos);

    GUI_SetColor(infoSettings.status_color);
  }

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
  GUI_DispString(exhibitRect.x0, exhibitRect.y1 - BYTE_HEIGHT, (uint8_t *) tempstr3);

  GUI_SetColor(infoSettings.font_color);
  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr2);
  setFontSize(FONT_SIZE_NORMAL);
}

void zOffsetSetMenu(bool probeOffset)
{
  probeOffsetMenu = probeOffset;
}

void menuZOffset(void)
{
  ITEM itemZOffsetSubmenu[] = {
    // icon                        label
    {ICON_01_MM,                   LABEL_01_MM},
    {ICON_RESET_VALUE,             LABEL_RESET},
    {ICON_EEPROM_SAVE,             LABEL_SAVE},
    {ICON_BABYSTEP,                LABEL_SHIM},
    {ICON_DISABLE_STEPPERS,        LABEL_XY_UNLOCK},
  };

  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS zOffsetItems = {
    // title
    LABEL_PROBE_OFFSET,
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
      {ICON_PROBE_OFFSET,            LABEL_OFF},
      {ICON_PAGE_DOWN,               LABEL_NEXT},
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  float now, z_offset;
  float unit;
  void (* offsetEnable)(float);        // enable Z offset
  void (* offsetDisable)(void);        // disable Z offset
  bool (* offsetGetStatus)(void);      // get current status
  float (* offsetGetValue)(void);      // get current Z offset
  float (* offsetResetValue)(void);    // reset current Z offset
  float (* offsetUpdateValue)(float);  // update current Z offset

  if (probeOffsetMenu)
  { // use Probe Offset menu
    zOffsetItems.title.index = LABEL_PROBE_OFFSET;
    offsetEnable = probeOffsetEnable;
    offsetDisable = probeOffsetDisable;
    offsetGetStatus = probeOffsetGetStatus;
    offsetGetValue = probeOffsetGetValue;
    offsetResetValue = probeOffsetResetValue;
    offsetUpdateValue = probeOffsetUpdateValue;
  }
  else
  { // use Home Offset menu
    zOffsetItems.title.index = LABEL_HOME_OFFSET;
    offsetEnable = homeOffsetEnable;
    offsetDisable = homeOffsetDisable;
    offsetGetStatus = homeOffsetGetStatus;
    offsetGetValue = homeOffsetGetValue;
    offsetResetValue = homeOffsetResetValue;
    offsetUpdateValue = homeOffsetUpdateValue;
  }

  now = z_offset = offsetGetValue();

  INVERT_Z_AXIS_ICONS(&zOffsetItems);
  zOffsetItems.items[KEY_ICON_4].label = itemToggle[offsetGetStatus()];

  itemZOffsetSubmenu[0] = itemMoveLen[curUnit_index];
  zOffsetItems.items[KEY_ICON_6] = itemZOffsetSubmenu[curSubmenu_index];

  menuDrawPage(&zOffsetItems);
  zOffsetDraw(offsetGetStatus(), now);

  while (MENU_IS(menuZOffset))
  {
    unit = moveLenSteps[curUnit_index];
    z_offset = offsetGetValue();  // always load current Z offset
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      // decrease Z offset
      case KEY_ICON_0:
      case KEY_DECREASE:
        if (!offsetGetStatus())
          zOffsetNotifyError(false);
        else
          z_offset = offsetUpdateValue(-unit);
        break;

      case KEY_INFOBOX:
        if (offsetGetStatus())
          zOffsetNotifyError(true);
        else
          OPEN_MENU(menuUnifiedHeat);
        break;

      // increase Z offset
      case KEY_ICON_3:
      case KEY_INCREASE:
        if (!offsetGetStatus())
          zOffsetNotifyError(false);
        else
          z_offset = offsetUpdateValue(unit);
        break;

      // enable/disable Z offset change
      case KEY_ICON_4:
        if (!offsetGetStatus())
          offsetEnable(infoSettings.level_z_pos);
        else
          offsetDisable();

        zOffsetItems.items[key_num].label = itemToggle[offsetGetStatus()];

        menuDrawItem(&zOffsetItems.items[key_num], key_num);
        zOffsetDraw(offsetGetStatus(), z_offset);
        break;

      // change submenu
      case KEY_ICON_5:
        curSubmenu_index = (curSubmenu_index + 1) % COUNT(itemZOffsetSubmenu);
        zOffsetItems.items[KEY_ICON_6] = itemZOffsetSubmenu[curSubmenu_index];

        menuDrawItem(&zOffsetItems.items[KEY_ICON_6], KEY_ICON_6);
        break;

      // handle submenu
      case KEY_ICON_6:
        switch (curSubmenu_index)
        {
          // change unit
          case 0:
            curUnit_index = (curUnit_index + 1) % COUNT(itemZOffsetSubmenu);
            itemZOffsetSubmenu[curSubmenu_index] = itemMoveLen[curUnit_index];
            zOffsetItems.items[key_num] = itemZOffsetSubmenu[curSubmenu_index];

            menuDrawItem(&zOffsetItems.items[key_num], key_num);
            break;

          // reset Z offset to default value
          case 1:
            if (!offsetGetStatus())
              zOffsetNotifyError(false);
            else
              z_offset = offsetResetValue();
            break;

          // save to EEPROM
          case 2:
            if (infoMachineSettings.EEPROM == 1)
              popupDialog(DIALOG_TYPE_QUESTION, zOffsetItems.title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
            break;

          // set level Z pos (shim)
          case 3:
            infoSettings.level_z_pos = editFloatValue(LEVELING_Z_POS_MIN, LEVELING_Z_POS_MAX,
                                                      LEVELING_Z_POS_DEFAULT, infoSettings.level_z_pos);
            zOffsetDraw(offsetGetStatus(), now);
            break;

          // unlock XY axis
          case 4:
            if (!offsetGetStatus())
              zOffsetNotifyError(false);
            else
              storeCmd("M84 X Y E\n");
            break;

          default:
            break;
        }
        break;

      case KEY_ICON_7:
        if (offsetGetStatus())
          offsetDisable();

        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (now != z_offset)
    {
      now = z_offset;
      zOffsetDraw(offsetGetStatus(), now);

      // reset babystep every time Z offset is changed otherwise the set babystep value
      // will not be aligned with the new Z offset
      babystepSetValue(BABYSTEP_DEFAULT_VALUE);
    }

    loopProcess();
  }

  saveSettings();  // save settings
}
