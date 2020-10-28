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

static float babystep;

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
      labelChar(tempstr1, LABEL_BL_COMPLETE);
      labelChar(tempstr2, LABEL_EEPROM_SAVE_INFO);
      sprintf(tmpBuf, "%s\n %s", tempstr1, tempstr2);
      setDialogText(LABEL_MBL_SETTINGS, (u8*) tmpBuf, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, LABEL_MBL_SETTINGS, LABEL_BL_COMPLETE);
    }
  }
  else                                 // if if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
  }
}

/* Show an eror message notification */
void mblNotifyError(void)
{
  char tmpBuf[120];
  labelChar(tempstr1, LABEL_MBL);
  labelChar(tempstr2, LABEL_OFF);
  sprintf(tmpBuf, "%s %s", tempstr1, tempstr2);
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
  mblRunning = false;
  BUZZER_PLAY(sound_error);
  popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
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
    labelChar(tempstr1, LABEL_OFF);
    sprintf(tempstr, "%s   ", tempstr1);
    GUI_SetColor(infoSettings.reminder_color);
  }

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  GUI_SetColor(infoSettings.font_color);
}

void mblDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

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
  float now = babystep = babystepGetValue();
  float unit;

  mblItems.items[KEY_ICON_5] = itemMblUnit[curUnit];

  menuDrawPage(&mblItems);
  mblDrawHeader(NULL);
  mblDrawValue(now);

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuMBL)
  {
    unit = mblUnit[curUnit];

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease babystep
      case KEY_ICON_0:
        if (!mblRunning)
          mblNotifyError();
        else
          babystep = babystepDecreaseValue(unit);
        break;

      // increase babystep
      case KEY_ICON_3:
        if (!mblRunning)
          mblNotifyError();
        else
          babystep = babystepIncreaseValue(unit);
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

          babystep = babystepResetValue();                 // always reset babystep to default value
        }
        break;

      // change unit
      case KEY_ICON_5:
        curUnit = (curUnit + 1) % ITEM_MBL_UNIT_NUM;

        mblItems.items[key_num] = itemMblUnit[curUnit];

        menuDrawItem(&mblItems.items[key_num], key_num);
        break;

      // reset babystep to default value
      case KEY_ICON_6:
        if (!mblRunning)
          mblNotifyError();
        else
          babystep = babystepResetValue();
        break;

      case KEY_ICON_7:
        if (mblRunning)
        {
          mblStop();

          babystep = babystepResetValue();                 // always reset offset to default value
        }

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!mblRunning)
              mblNotifyError();
            else
              babystep = babystepUpdateValueByEncoder(unit);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != babystep)
    {
      now = babystep;
      mblDrawValue(now);
    }

    loopProcess();
  }
}
