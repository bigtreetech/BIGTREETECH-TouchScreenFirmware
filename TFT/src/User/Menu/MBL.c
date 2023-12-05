#include "MBL.h"
#include "includes.h"

static uint8_t curUnit_index = 0;
uint8_t mblPoint = 0;
bool mblRunning = false;

// Start MBL
static inline void mblStart(void)
{
  mblRunning = true;

  probeHeightEnable();  // temporary disable software endstops and save ABL state

  // MBL gcode sequence start
  mustStoreCmd("G28\n");
  mustStoreCmd("G29 S1\n");  // home and move to first point for Z height adjustment

  #ifdef MBL_START_Z
    probeHeightStart(infoSettings.level_z_pos, false);  // raise nozzle
  #endif

  probeHeightRelative();  // set relative position mode
}

// Stop MBL
static inline void mblStop(void)
{
  mblRunning = false;
  mblPoint = 0;

  if (infoMachineSettings.zProbe == ENABLED)
    probeHeightStop(infoSettings.level_z_raise);  // raise nozzle

  probeHeightAbsolute();  // set absolute position mode

  probeHeightDisable();  // restore original software endstops state and ABL state
}

// Abort MBL
static inline void mblAbort(void)
{
  // MBL gcode sequence stop
  mustStoreCmd("G29 S5\n");  // reset and disable mesh

  mblStop();

  BUZZER_PLAY(SOUND_ERROR);

  popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
}

// called by parseAck() to notify MBL process status
void mblUpdateStatus(bool succeeded)
{
  mblStop();

  if (succeeded)  // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(SOUND_SUCCESS);

    LABELCHAR(tempMsg, LABEL_BL_COMPLETE);

    if (infoMachineSettings.EEPROM == 1)
    {
      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

      popupDialog(DIALOG_TYPE_SUCCESS, LABEL_MBL_SETTINGS, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, LABEL_MBL_SETTINGS, (uint8_t *) tempMsg);
    }
  }
  else  // if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(SOUND_ERROR);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
  }
}

// Show an error notification
void mblNotifyError(bool isStarted)
{
  LABELCHAR(tempMsg, LABEL_MBL);

  sprintf(&tempMsg[strlen(tempMsg)], " %s", isStarted ? textSelect(LABEL_ON) : textSelect(LABEL_OFF));

  addToast(DIALOG_TYPE_ERROR, tempMsg);
}

void mblDraw(COORDINATE *val)
{
  char tempstr[24], tempstr2[24], tempstr3[24];

  if (mblPoint == 0)
  {
    sprintf(tempstr, "%-15s", textSelect(LABEL_OFF));
    sprintf(tempstr2, "%s", "");  // temp string

    GUI_SetColor(infoSettings.reminder_color);
  }
  else
  {
    sprintf(tempstr, "P:%-4d", mblPoint);
    sprintf(tempstr2, "X:%.3f Y:%.3f", val->axis[X_AXIS], val->axis[Y_AXIS]);  // temp string

    GUI_SetColor(infoSettings.status_color);
  }

  sprintf(tempstr3, "%-19s", tempstr2);              // X, Y
  sprintf(tempstr2, "  %.3f  ", val->axis[Z_AXIS]);  // Z

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
  GUI_SetColor(infoSettings.status_color);
  GUI_DispString(exhibitRect.x0, exhibitRect.y1 - BYTE_HEIGHT, (uint8_t *) tempstr3);

  GUI_SetColor(infoSettings.font_color);
  setFontSize(FONT_SIZE_LARGE);
  GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "mm");
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr2);
  setFontSize(FONT_SIZE_NORMAL);
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
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
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
  COORDINATE now, curValue;
  float unit;

  coordinateGetAllActual(&now);

  INVERT_Z_AXIS_ICONS(&mblItems);
  mblItems.items[KEY_ICON_4] = itemMoveLen[curUnit_index];

  if (mblRunning)
  {
    mblItems.items[KEY_ICON_6].icon = ICON_PAGE_DOWN;
    mblItems.items[KEY_ICON_6].label.index = LABEL_NEXT;
  }

  menuDrawPage(&mblItems);
  mblDraw(&now);

  while (MENU_IS(menuMBL))
  {
    unit = moveLenSteps[curUnit_index];
    coordinateGetAllActual(&curValue);
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      // decrease Z height
      case KEY_ICON_0:
      case KEY_DECREASE:
        if (!mblRunning)
          mblNotifyError(false);
        else
          probeHeightMove(-unit);
        break;

      case KEY_INFOBOX:
        if (mblRunning)
          mblNotifyError(true);
        else
          OPEN_MENU(menuUnifiedHeat);
        break;

      // increase Z height
      case KEY_ICON_3:
      case KEY_INCREASE:
        if (!mblRunning)
          mblNotifyError(false);
        else
          probeHeightMove(unit);
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
          probeHeightMove(-curValue.axis[Z_AXIS]);
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
        }
        else
        {
          storeCmd("G29 S2\n");  // save Z height and move to next mesh point

          #ifdef MBL_START_Z
            probeHeightStart(infoSettings.level_z_pos, false);  // raise nozzle
          #endif

          ++mblPoint;
        }
        break;

      case KEY_ICON_7:
        if (mblRunning)
          mblAbort();

        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (memcmp(&now, &curValue, sizeof(COORDINATE)))
    {
      coordinateGetAllActual(&now);
      mblDraw(&now);
    }

    probeHeightQueryCoord();

    loopProcess();
  }
}
