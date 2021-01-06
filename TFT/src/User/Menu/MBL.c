#include "MBL.h"
#include "includes.h"

#define PROBE_HEIGHT_INITIAL_HEIGHT 0.2f                   // 0.2 mm

static u8 curUnit_index = 0;

u8 mblPoint = 0;
bool mblRunning = false;

/* called by parseAck() to notify MBL process status */
void mblUpdateStatus(bool succeeded)
{
  mblRunning = false;

  probeHeightStop();                                       // raise nozzle

  probeHeightDisable();                                    // restore original software endstops state

  if (succeeded)                                           // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(sound_success);

    LABELCHAR(tempMsg, LABEL_BL_COMPLETE);

    if (infoMachineSettings.EEPROM == 1)
    {
      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

      setDialogText(LABEL_MBL_SETTINGS, (u8 *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, LABEL_MBL_SETTINGS, (u8 *) tempMsg);
    }
  }
  else                                                     // if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
  }
}

/* Show an error notification */
void mblNotifyError(void)
{
  LABELCHAR(tempMsg, LABEL_MBL);

  sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_OFF));

  addToast(DIALOG_TYPE_ERROR, tempMsg);
}

/* Start MBL */
static inline void mblStart(void)
{
  mblRunning = true;
  mblPoint = 0;

  probeHeightEnable();                                     // temporary disable software endstops

  // MBL gcode sequence start
  mustStoreCmd("G29 S1\n");                                // home and move to first point for Z height adjustment

  probeHeightStart(PROBE_HEIGHT_INITIAL_HEIGHT);           // lower nozzle to provided absolute Z point
}

/* Stop MBL */
static inline void mblStop(void)
{
  mblRunning = false;

  // MBL gcode sequence stop
  mustStoreCmd("G29 S5\n");                                // reset and disable mesh

  probeHeightStop();                                       // raise nozzle

  probeHeightDisable();                                    // restore original software endstops state

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
    sprintf(tempstr, "%s   ", textSelect(LABEL_OFF));

    GUI_SetColor(infoSettings.reminder_color);
  }

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *) tempstr);
  GUI_SetColor(infoSettings.font_color);
}

void mblDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (u8 *) tempstr);
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
     {ICON_001_MM,                  LABEL_001_MM},
     {ICON_RESET_VALUE,             LABEL_RESET},
     {ICON_RESUME,                  LABEL_START},
     {ICON_BACK,                    LABEL_BACK},}
  };

  #if FRIENDLY_PROBE_OFFSET_LANGUAGE == 1
    mblItems.items[0].icon = ICON_NOZZLE_DOWN;
    mblItems.items[0].label.index = LABEL_DOWN;
    mblItems.items[3].icon = ICON_NOZZLE_UP;
    mblItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now, curValue;
  float unit;

  now = curValue = coordinateGetAxisActual(Z_AXIS);

  mblItems.items[KEY_ICON_4] = itemMoveLen[curUnit_index];

  if (mblRunning)
  {
    mblItems.items[KEY_ICON_6].icon = ICON_PAGE_DOWN;
    mblItems.items[KEY_ICON_6].label.index = LABEL_NEXT;
  }

  menuDrawPage(&mblItems);
  mblDrawHeader(!mblRunning ? NULL : &mblPoint);
  mblDrawValue(now);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuMBL)
  {
    unit = moveLenSteps[curUnit_index];

    curValue = coordinateGetAxisActual(Z_AXIS);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      // decrease Z height
      case KEY_ICON_0:
        if (!mblRunning)
          mblNotifyError();
        else
          probeHeightMove(unit, -1);
        break;

      // increase Z height
      case KEY_ICON_3:
        if (!mblRunning)
          mblNotifyError();
        else
          probeHeightMove(unit, 1);
        break;

      // change unit
      case KEY_ICON_4:
        curUnit_index = (curUnit_index + 1) % ITEM_FINE_MOVE_LEN_NUM;

        mblItems.items[key_num] = itemMoveLen[curUnit_index];

        menuDrawItem(&mblItems.items[key_num], key_num);
        break;

      // reset Z height to 0
      case KEY_ICON_5:
        if (!mblRunning)
          mblNotifyError();
        else
          probeHeightMove(curValue, -1);
        break;

      // start MBL or move to next mesh point
      case KEY_ICON_6:
        if (!mblRunning)
        {                                                  // start MBL
          mblStart();

          mblItems.items[key_num].icon = ICON_PAGE_DOWN;
          mblItems.items[key_num].label.index = LABEL_NEXT;

          menuDrawItem(&mblItems.items[key_num], key_num);

          ++mblPoint;
          mblDrawHeader(&mblPoint);
        }
        else
        {
          storeCmd("G29 S2\n");                            // save Z height and move to next mesh point

          probeHeightStart(PROBE_HEIGHT_INITIAL_HEIGHT);   // lower nozzle to provided absolute Z point

          ++mblPoint;
          mblDrawHeader(&mblPoint);
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
              probeHeightMove(unit, encoderPosition > 0 ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != curValue)
    {
      now = curValue;
      mblDrawValue(now);
    }

    probeHeightQueryCoord();

    loopProcess();
  }
}
