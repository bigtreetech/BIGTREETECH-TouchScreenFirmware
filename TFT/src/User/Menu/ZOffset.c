#include "ZOffset.h"
#include "includes.h"

#define ITEM_Z_OFFSET_SUBMENU_NUM 4

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

void zOffsetDrawStatus(bool status, uint8_t *val)
{
  char tempstr[20];

  sprintf(tempstr, "%-15s", val);

  if (!status)
    GUI_SetColor(infoSettings.reminder_color);
  else
    GUI_SetColor(infoSettings.sd_reminder_color);

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);

  GUI_SetColor(infoSettings.font_color);
  setLargeFont(true);
  GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"mm");
  setLargeFont(false);
}

void zOffsetDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

void zOffsetSetMenu(bool probeOffset)
{
  probeOffsetMenu = probeOffset;
}

void menuZOffset(void)
{
  ITEM itemZOffsetSubmenu[ITEM_Z_OFFSET_SUBMENU_NUM] = {
    // icon                        label
    {ICON_01_MM,                   LABEL_01_MM},
    {ICON_RESET_VALUE,             LABEL_RESET},
    {ICON_EEPROM_SAVE,             LABEL_SAVE},
    {ICON_DISABLE_STEPPERS,        LABEL_XY_UNLOCK},
  };

  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS zOffsetItems = {
    // title
    LABEL_PROBE_OFFSET,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_PROBE_OFFSET,            LABEL_OFF},
      {ICON_PAGE_DOWN,               LABEL_NEXT},
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
    zOffsetItems.items[0].icon = ICON_NOZZLE_DOWN;
    zOffsetItems.items[0].label.index = LABEL_DOWN;
    zOffsetItems.items[3].icon = ICON_NOZZLE_UP;
    zOffsetItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now, z_offset;
  float unit;
  bool (* offsetGetStatus)(void);                       // get current status
  void (* offsetEnable)(bool);                          // enable Z offset
  void (* offsetDisable)(void);                         // disable Z offset
  float (* offsetDecreaseValue)(float);                 // decrease current Z offset
  float (* offsetIncreaseValue)(float);                 // increase current Z offset
  float (* offsetResetValue)(void);                     // reset current Z offset
  float (* offsetGetValue)(void);                       // get current Z offset
  float (* offsetUpdateValueByEncoder)(float, int8_t);  // update current Z offset by encoder

  if (probeOffsetMenu)
  { // use Probe Offset menu
    zOffsetItems.title.index = LABEL_PROBE_OFFSET;
    offsetGetStatus = probeOffsetGetStatus;
    offsetEnable = probeOffsetEnable;
    offsetDisable = probeOffsetDisable;
    offsetDecreaseValue = probeOffsetDecreaseValue;
    offsetIncreaseValue = probeOffsetIncreaseValue;
    offsetResetValue = probeOffsetResetValue;
    offsetGetValue = probeOffsetGetValue;
    offsetUpdateValueByEncoder = probeOffsetUpdateValueByEncoder;
  }
  else
  { // use Home Offset menu
    zOffsetItems.title.index = LABEL_HOME_OFFSET;
    offsetGetStatus = homeOffsetGetStatus;
    offsetEnable = homeOffsetEnable;
    offsetDisable = homeOffsetDisable;
    offsetDecreaseValue = homeOffsetDecreaseValue;
    offsetIncreaseValue = homeOffsetIncreaseValue;
    offsetResetValue = homeOffsetResetValue;
    offsetGetValue = homeOffsetGetValue;
    offsetUpdateValueByEncoder = homeOffsetUpdateValueByEncoder;
  }

  now = z_offset = offsetGetValue();

  zOffsetItems.items[KEY_ICON_4].label = itemToggle[offsetGetStatus()];

  itemZOffsetSubmenu[0] = itemMoveLen[curUnit_index];
  zOffsetItems.items[KEY_ICON_6] = itemZOffsetSubmenu[curSubmenu_index];

  menuDrawPage(&zOffsetItems);
  zOffsetDrawStatus(offsetGetStatus(), textSelect(zOffsetItems.items[KEY_ICON_4].label.index));
  zOffsetDrawValue(now);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuZOffset)
  {
    unit = moveLenSteps[curUnit_index];

    z_offset = offsetGetValue();  // always load current Z offset

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease Z offset
      case KEY_ICON_0:
        if (!offsetGetStatus())
          zOffsetNotifyError(false);
        else
          z_offset = offsetDecreaseValue(unit);
        break;

      case KEY_INFOBOX:
        if (offsetGetStatus())
          zOffsetNotifyError(true);
        else
          infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
        break;

      // increase Z offset
      case KEY_ICON_3:
        if (!offsetGetStatus())
          zOffsetNotifyError(false);
        else
          z_offset = offsetIncreaseValue(unit);
        break;

      // enable/disable Z offset change
      case KEY_ICON_4:
        if (!offsetGetStatus())
          offsetEnable(true);
        else
          offsetDisable();

        zOffsetItems.items[key_num].label = itemToggle[offsetGetStatus()];

        menuDrawItem(&zOffsetItems.items[key_num], key_num);
        zOffsetDrawStatus(offsetGetStatus(), textSelect(zOffsetItems.items[key_num].label.index));
        break;

      // change submenu
      case KEY_ICON_5:
        curSubmenu_index = (curSubmenu_index + 1) % ITEM_Z_OFFSET_SUBMENU_NUM;
        zOffsetItems.items[KEY_ICON_6] = itemZOffsetSubmenu[curSubmenu_index];

        menuDrawItem(&zOffsetItems.items[KEY_ICON_6], KEY_ICON_6);
        break;

      // handle submenu
      case KEY_ICON_6:
        switch (curSubmenu_index)
        {
          // change unit
          case 0:
            curUnit_index = (curUnit_index + 1) % ITEM_FINE_MOVE_LEN_NUM;
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
            {
              setDialogText(zOffsetItems.title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
              showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
            }
            break;

          // unlock XY axis
          case 3:
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

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!offsetGetStatus())
              zOffsetNotifyError(false);
            else
              z_offset = offsetUpdateValueByEncoder(unit, encoderPosition > 0 ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != z_offset)
    {
      now = z_offset;
      zOffsetDrawValue(now);

      // reset babystep every time Z offset is changed otherwise the set babystep value
      // will not be aligned with the new Z offset
      babystepReset();
    }

    loopProcess();
  }
}
