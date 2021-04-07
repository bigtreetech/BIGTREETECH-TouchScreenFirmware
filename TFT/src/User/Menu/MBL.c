#include "MBL.h"
#include "includes.h"

static uint8_t curUnit_index = 0;
uint8_t mblPoint = 0;
bool mblRunning = false;

// Start MBL
static inline void mblStart(void)
{
  mblRunning = true;
  mblPoint = 0;

  probeHeightEnable();  // temporary disable software endstops

  // MBL gcode sequence start
  mustStoreCmd("G28\n");
  mustStoreCmd("G29 S1\n");                           // home and move to first point for Z height adjustment
  probeHeightStart(infoSettings.level_z_pos, false);  // raise nozzle
  probeHeightRelative();                              // set relative position mode
}

// Stop MBL
static inline void mblStop(void)
{
  mblRunning = false;

  if (infoMachineSettings.zProbe == ENABLED)
    probeHeightStop(infoSettings.level_z_raise);  // raise nozzle

  probeHeightAbsolute();  // set absolute position mode

  probeHeightDisable();  // restore original software endstops state
}

// Abort MBL
static inline void mblAbort(void)
{
  // MBL gcode sequence stop
  mustStoreCmd("G29 S5\n");  // reset and disable mesh

  mblStop();

  BUZZER_PLAY(sound_error);

  popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
}

// called by parseAck() to notify MBL process status
void mblUpdateStatus(bool succeeded)
{
  mblStop();

  if (succeeded)  // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(sound_success);

    LABELCHAR(tempMsg, LABEL_BL_COMPLETE);

    if (infoMachineSettings.EEPROM == 1)
    {
      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

      setDialogText(LABEL_MBL_SETTINGS, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, LABEL_MBL_SETTINGS, (uint8_t *) tempMsg);
    }
  }
  else  // if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
  }
}

// Show an error notification
void mblNotifyError(bool isStarted)
{
  LABELCHAR(tempMsg, LABEL_MBL);

  if (!isStarted)
    sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_OFF));
  else
    sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_ON));

  addToast(DIALOG_TYPE_ERROR, tempMsg);
}

void mblDrawHeader(uint8_t *point)
{
  char tempstr[20];

  if (point != NULL)
  {
    sprintf(tempstr, "P:%-4d", *point);
    GUI_SetColor(infoSettings.sd_reminder_color);
  }
  else
  {
    sprintf(tempstr, "%-15s", textSelect(LABEL_OFF));
    GUI_SetColor(infoSettings.reminder_color);
  }

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
  GUI_SetColor(infoSettings.font_color);
  setLargeFont(true);
  GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "mm");
  setLargeFont(false);
}

void mblDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

void menuMBL(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS mblItems = {
    // title
    LABEL_MBL_SETTINGS,
    // icon                          label
    {
      #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
        {ICON_NOZZLE_DOWN,             LABEL_DOWN},
      #else
        {ICON_DEC,                     LABEL_DEC},
      #endif
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
        {ICON_NOZZLE_UP,               LABEL_UP},
      #else
        {ICON_INC,                     LABEL_INC},
      #endif
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_RESUME,                  LABEL_START},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

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
          mblNotifyError(false);
        else
          probeHeightMove(unit, -1);
        break;

      case KEY_INFOBOX:
        if (mblRunning)
          mblNotifyError(true);
        else
          infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
        break;

      // increase Z height
      case KEY_ICON_3:
        if (!mblRunning)
          mblNotifyError(false);
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
          mblNotifyError(false);
        else
          probeHeightMove(curValue, -1);
        break;

      // start MBL or move to next mesh point
      case KEY_ICON_6:
        if (!mblRunning)
        { // start MBL
          mblStart();

          mblItems.items[key_num].icon = ICON_PAGE_DOWN;
          mblItems.items[key_num].label.index = LABEL_NEXT;

          menuDrawItem(&mblItems.items[key_num], key_num);

          ++mblPoint;
          mblDrawHeader(&mblPoint);
        }
        else
        {
          storeCmd("G29 S2\n");  // save Z height and move to next mesh point

          probeHeightStart(infoSettings.level_z_pos, false);  // raise nozzle

          ++mblPoint;
          mblDrawHeader(&mblPoint);
        }
        break;

      case KEY_ICON_7:
        if (mblRunning)
          mblAbort();

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!mblRunning)
              mblNotifyError(false);
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
