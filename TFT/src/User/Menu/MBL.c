#include "MBL.h"
#include "includes.h"

#define ITEM_MBL_UNIT_NUM 3

const ITEM itemMblUnit[ITEM_MBL_UNIT_NUM] = {
  // icon                           label
  {ICON_001_MM,                     LABEL_001_MM},
  {ICON_01_MM,                      LABEL_01_MM},
  {ICON_1_MM,                       LABEL_1_MM},
};

const float mblUnit[ITEM_MBL_UNIT_NUM] = {0.01f, 0.1f, 1};
static u8   curUnit = 0;

static float offset = BABYSTEP_DEFAULT_VALUE;

u8 mblPoint = 0;
bool mblRunning = false;

void mblUpdateStatus(bool succeeded)
{
  mblRunning = false;

  if (succeeded)                       // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(sound_success);

    if (infoMachineSettings.EEPROM == 1)
    {
      char tmpBuf[120];

      sprintf(tmpBuf, "%s\n %s", textSelect(LABEL_BL_COMPLETE), textSelect(LABEL_EEPROM_SAVE_INFO));

      showDialog(DIALOG_TYPE_SUCCESS, textSelect(LABEL_MBL_SETTINGS), (u8*) tmpBuf,
        textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, textSelect(LABEL_MBL_SETTINGS), textSelect(LABEL_BL_COMPLETE));
    }
  }
  else                                 // if if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, textSelect(LABEL_MBL_SETTINGS), textSelect(LABEL_PROCESS_ABORTED));
  }
}

/* Show an eror message notification */
void mblNotifyError(void)
{
  char tmpBuf[120];
  sprintf(tmpBuf, "%s %s", textSelect(LABEL_MBL), textSelect(LABEL_OFF));
  addToast(DIALOG_TYPE_ERROR, tmpBuf);
}

/* Start MBL */
void mblStart(void)
{
  mblRunning = true;
  mblPoint = 0;

  // MBL gcode sequence start
  mustStoreCmd("G29 S1\n");                                // to move to the first point for Z adjustment
}

/* Stop MBL */
void mblStop(void)
{
  if (mblRunning)
  {
    mblRunning = false;

    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, textSelect(LABEL_MBL_SETTINGS), textSelect(LABEL_PROCESS_ABORTED));
  }
}

/* Set default offset */
void mblSetDefaultOffset(void)
{
  float last_unit;
  float processed_offset = 0.0f;

  int8_t neg = 1;
  if (offset < 0.0f)
    neg = -1;

  int stepcount = (offset * neg) / BABYSTEP_MAX_UNIT;
  for (; stepcount > 0; stepcount--)
  {
    mustStoreCmd("M290 Z%.2f\n", -(BABYSTEP_MAX_UNIT * neg));
    processed_offset += BABYSTEP_MAX_UNIT;
  }

  last_unit = (offset * neg) - processed_offset;
  if (last_unit > 0.0f)
  {
    mustStoreCmd("M290 Z%.2f\n", -(last_unit * neg));
    processed_offset += last_unit;
  }
  offset -= (processed_offset * neg);
}

void mblDrawHeader(u8 *point)
{
  char tempstr[20];

  if (point != NULL)
  {
    sprintf(tempstr, "P%d   ", *point);

    GUI_SetColor(infoSettings.sd_reminder_color);
  }
  else
  {
    sprintf(tempstr, "%s   ", textSelect(LABEL_OFF));

    GUI_SetColor(infoSettings.reminder_color);
  }

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  GUI_SetColor(infoSettings.font_color);
}

void mblDrawValue()
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", offset);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);
  setLargeFont(false);
}

void menuMBL(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS mblItems = {
    // title
    LABEL_MBL_SETTINGS,
    // icon                         label
    {{ICON_DEC,                     LABEL_DEC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_INC,                     LABEL_INC},
     {ICON_RESUME,                  LABEL_START},
     {ICON_001_MM,                  LABEL_001_MM},
     {ICON_RESET_VALUE,             LABEL_RESET},
     {ICON_BACK,                    LABEL_BACK},}
  };

  #if FRIENDLY_PROBE_OFFSET_LANGUAGE == 1
    mblItems.items[0].icon = ICON_NOZZLE_DOWN;
    mblItems.items[0].label.index = LABEL_DOWN;
    mblItems.items[3].icon = ICON_NOZZLE_UP;
    mblItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now = offset;

  mblItems.items[KEY_ICON_5] = itemMblUnit[curUnit];

  menuDrawPage(&mblItems);
  mblDrawHeader(NULL);
  mblDrawValue();

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuMBL)
  {
    float max_unit = mblUnit[curUnit];

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease offset
      case KEY_ICON_0:
        if (!mblRunning)
          mblNotifyError();
        else if (offset > BABYSTEP_MIN_VALUE)
        {
          float diff = offset - BABYSTEP_MIN_VALUE;
          max_unit = (diff > max_unit) ? max_unit : diff;

          mustStoreCmd("M290 Z-%.2f\n", max_unit);

          offset -= max_unit;
        }
        break;

      // increase offset
      case KEY_ICON_3:
        if (!mblRunning)
          mblNotifyError();
        else if (offset < BABYSTEP_MAX_VALUE)
        {
          float diff = BABYSTEP_MAX_VALUE - offset;
          max_unit = (diff > max_unit) ? max_unit : diff;

          mustStoreCmd("M290 Z%.2f\n", max_unit);

          offset += max_unit;
        }
        break;

      // start MBL or probe the next mesh point
      case KEY_ICON_4:
        if (!mblRunning)
        {
          mblStart();

          mblItems.items[key_num].icon = ICON_PAGE_DOWN;
          mblItems.items[key_num].label.index = LABEL_NEXT;

          menuDrawItem(&mblItems.items[key_num], key_num);

          ++mblPoint;
          mblDrawHeader(&mblPoint);
        }
        else
        {
          storeCmd("G29 S2\n");                            // to save the Z value and move to the next point

          ++mblPoint;
          mblDrawHeader(&mblPoint);

          if (offset != BABYSTEP_DEFAULT_VALUE)
            mblSetDefaultOffset();
        }
        break;

      // change step unit
      case KEY_ICON_5:
        curUnit = (curUnit + 1) % ITEM_MBL_UNIT_NUM;

        mblItems.items[key_num] = itemMblUnit[curUnit];

        menuDrawItem(&mblItems.items[key_num], key_num);
        break;

      // reset offset to default value
      case KEY_ICON_6:
        if (!mblRunning)
          mblNotifyError();
        else
        {
          if (offset != BABYSTEP_DEFAULT_VALUE)
            mblSetDefaultOffset();
        }
        break;

      case KEY_ICON_7:
        if (mblRunning)
          mblStop();

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!mblRunning)
              mblNotifyError();
            else
            {
              mustStoreCmd("M290 Z%.2f\n", mblUnit[curUnit] * encoderPosition);

              offset += mblUnit[curUnit] * encoderPosition;
            }

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != offset)
    {
      now = offset;
      mblDrawValue();
    }

    loopProcess();
  }
}
