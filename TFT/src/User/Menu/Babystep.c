#include "Babystep.h"
#include "includes.h"

static u8 moveLenSteps_index = 0;

void babyReDraw(float babystep, float z_offset, bool force_z_offset, bool skip_header)
{
  if (!skip_header)
  {
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, LABEL_BABYSTEP);

    if (infoMachineSettings.zProbe == ENABLED)
      GUI_DispString(exhibitRect.x0, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_PROBE_OFFSET);
    else
      GUI_DispString(exhibitRect.x0, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_HOME_OFFSET);
  }

  char tempstr[20];

  GUI_POINT point_bs = {exhibitRect.x1, exhibitRect.y0 + BYTE_HEIGHT};
  GUI_POINT point_of = {exhibitRect.x1, exhibitRect.y0 + BYTE_HEIGHT * 2 + LARGE_BYTE_HEIGHT};

  setLargeFont(true);

  sprintf(tempstr, "% 6.2f", babystep);
  GUI_DispStringRight(point_bs.x, point_bs.y, (u8 *) tempstr);

  sprintf(tempstr, "% 6.2f", z_offset);

  if (force_z_offset)
    GUI_SetColor(infoSettings.reminder_color);
  else
    GUI_SetColor(infoSettings.font_color);

  GUI_DispStringRight(point_of.x, point_of.y, (u8 *) tempstr);
  GUI_SetColor(infoSettings.font_color); // restore default font color
  setLargeFont(false);
}

void menuBabystep(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS babyStepItems = {
    // title
    LABEL_BABYSTEP,
    // icon                         label
    {{ICON_DEC,                     LABEL_DEC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_INC,                     LABEL_INC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_001_MM,                  LABEL_001_MM},
     {ICON_RESET_VALUE,             LABEL_RESET},
     {ICON_BACK,                    LABEL_BACK},}
  };

  #if FRIENDLY_PROBE_OFFSET_LANGUAGE == 1
    babyStepItems.items[0].icon = ICON_NOZZLE_DOWN;
    babyStepItems.items[0].label.index = LABEL_DOWN;
    babyStepItems.items[3].icon = ICON_NOZZLE_UP;
    babyStepItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now_babystep, babystep, orig_babystep;
  float now_z_offset, z_offset, orig_z_offset;
  float unit;
  bool force_z_offset;
  float (* offsetGetValue)(void);                          // get current Z offset
  float (* offsetSetValue)(float);                         // set current Z offset

  if (infoMachineSettings.zProbe == ENABLED)
  {
    offsetGetValue = probeOffsetGetValue;
    offsetSetValue = probeOffsetSetValue;
  }
  else
  {
    offsetGetValue = homeOffsetGetValue;
    offsetSetValue = homeOffsetSetValue;
  }

  now_babystep = babystep = orig_babystep = babystepGetValue();
  now_z_offset = z_offset = orig_z_offset = offsetGetValue();
  force_z_offset = false;

  if (infoMachineSettings.EEPROM == 1)
  {
    babyStepItems.items[KEY_ICON_4].icon = ICON_EEPROM_SAVE;
    babyStepItems.items[KEY_ICON_4].label.index = LABEL_SAVE;
  }

  babyStepItems.items[KEY_ICON_5] = itemMoveLen[moveLenSteps_index];

  menuDrawPage(&babyStepItems);
  babyReDraw(now_babystep, now_z_offset, force_z_offset, false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuBabystep)
  {
    unit = moveLenSteps[moveLenSteps_index];

    babystep = babystepGetValue();                         // always load current babystep

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease babystep / Z offset
      case KEY_ICON_0:
        babystep = babystepDecreaseValue(unit);
        break;

      // increase babystep / Z offset
      case KEY_ICON_3:
        babystep = babystepIncreaseValue(unit);
        break;

      // save to EEPROM and apply Z offset
      case KEY_ICON_4:
        if (infoMachineSettings.EEPROM == 1)
        {
          offsetSetValue(z_offset);                        // set new Z offset

          setDialogText(babyStepItems.title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        break;

      // change unit
      case KEY_ICON_5:
        moveLenSteps_index = (moveLenSteps_index + 1) % ITEM_FINE_MOVE_LEN_NUM;
        babyStepItems.items[key_num] = itemMoveLen[moveLenSteps_index];
        menuDrawItem(&babyStepItems.items[key_num], key_num);
        break;

      // reset babystep to default value
      case KEY_ICON_6:
        babystep = orig_babystep = babystepResetValue();
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            babystep = babystepUpdateValueByEncoder(unit, encoderPosition > 0 ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    z_offset = offsetGetValue();                           // always load current Z offset

    if (now_babystep != babystep || now_z_offset != z_offset)
    {
      if (now_z_offset != z_offset || (orig_babystep - 0.005f <= babystep && babystep <= orig_babystep + 0.005f))
      {
        // if current Z offset is changed applying babystep changes (e.g. BABYSTEP_ZPROBE_OFFSET is set in Marlin FW)
        // or babystep is almost the same as the initial one,
        // we don't force Z offset change
        now_z_offset = z_offset;
        force_z_offset = false;
      }
      else if (orig_z_offset == z_offset)
      {
        // if current Z offset is not changed applying babystep changes (e.g. no BABYSTEP_ZPROBE_OFFSET is set in Marlin FW),
        // we force Z offset change
        z_offset += babystep - orig_babystep;
        force_z_offset = true;
      }

      now_babystep = babystep;
      babyReDraw(now_babystep, z_offset, force_z_offset, true);
    }

    loopProcess();
  }
}
