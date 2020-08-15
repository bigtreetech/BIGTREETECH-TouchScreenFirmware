#include "Pid.h"
#include "Temperature.h"

//#define ENABLE_PID_STATUS_UPDATE_NOTIFICATION

const ITEM itemPidTool[] = {
  // icon                           label
  {ICON_NOZZLE,                     LABEL_NOZZLE},
  {ICON_NOZZLE,                     LABEL_NOZZLE},
  {ICON_NOZZLE,                     LABEL_NOZZLE},
  {ICON_NOZZLE,                     LABEL_NOZZLE},
  {ICON_NOZZLE,                     LABEL_NOZZLE},
  {ICON_NOZZLE,                     LABEL_NOZZLE},
  {ICON_BED,                        LABEL_BED},
  {ICON_CHAMBER,                    LABEL_CHAMBER},         // that will never be displayed because no PID is provided for chamber
};

#define ITEM_PID_DEGREE_NUM 3

const ITEM itemPidDegree[ITEM_PID_DEGREE_NUM] = {
  // icon                           label
  {ICON_1_DEGREE,                   LABEL_1_DEGREE},
  {ICON_5_DEGREE,                   LABEL_5_DEGREE},
  {ICON_10_DEGREE,                  LABEL_10_DEGREE},
};

const u8  pidDegree[ITEM_PID_DEGREE_NUM] = {1, 5, 10};
static u8 curDegree = 1;

const char* const pidDisplayID[] = HEAT_DISPLAY_ID;
const char*       pidCmd[] = PID_CMD;

HEATER pidHeater = {{}, 0};
u32 pidTimeout = 0;
u8 pidCounter = 0;
bool pidSucceeded = false;
bool pidRunning = false;
bool pidInitialized = false;

void pidUpdateStatus(bool succeeded)
{
  if (pidCounter > 0)
    pidCounter--;

  if (!succeeded)                      // if one PID process fails, the overall PID process must be marked as failed so no save to EEPROM will be allowed
    pidSucceeded = false;

  if (pidCounter > 0)                  // if all the PID processes were still not terminated, simply provide a notification
  {
#ifdef ENABLE_PID_STATUS_UPDATE_NOTIFICATION
    if (succeeded)
      addToast((char*)textSelect(LABEL_PROCESS_COMPLETED));
    else
      addToast((char*)textSelect(LABEL_PROCESS_ABORTED));
#endif
  }
  else                                 // if all the PID processes terminated, provide the final dialog
  {
    pidRunning = false;

    if (pidSucceeded)                  // if all the PID processes successfully terminated, allow to save to EEPROM
    {
      char tmpBuf[120];

      sprintf(tmpBuf, "%s\n %s", textSelect(LABEL_PROCESS_COMPLETED), textSelect(LABEL_EEPROM_SAVE_INFO));

      BUZZER_PLAY(sound_success);

      showDialog(DIALOG_TYPE_SUCCESS, textSelect(LABEL_PID_TITLE), (u8*) tmpBuf,
        textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), saveEepromSettings, NULL, NULL);
    }
    else                               // if at least a PID process failed, provide an error dialog
    {
      BUZZER_PLAY(sound_error);

      popupReminder(DIALOG_TYPE_ERROR, textSelect(LABEL_PID_TITLE), textSelect(LABEL_PROCESS_ABORTED));
    }
  }
}

void pidCheckTimeout()
{
  if (pidRunning)
  {
    if (OS_GetTimeMs() > pidTimeout)
    {
      pidRunning = false;

//      u8 pidCounter = 0;             // we voluntary don't reset (commented out the code) also pidCounter and pidSucceeded to let the
//      pidSucceeded = false;          // pidUpdateStatus function allow to handle status updates eventually arriving after the timeout

      char tmpBuf[120];

      sprintf(tmpBuf, "%s\n %s", textSelect(LABEL_TIMEOUT_REACHED), textSelect(LABEL_PROCESS_ABORTED));

      BUZZER_PLAY(sound_error);

      popupReminder(DIALOG_TYPE_ERROR, textSelect(LABEL_PID_TITLE), (u8*) tmpBuf);
    }
  }
}

void pidUpdateCounter()
{
  pidCounter = 0;

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)                               // hotends + bed + chamber
  {
    if (pidHeater.T[i].target > 0)
      pidCounter++;
  }
}

void menuPidWait(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  const MENUITEMS pidWaitItems = {
    // title
    LABEL_PID_TITLE,
    // icon                         label
    {{ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},}
  };

  bool isPressed, isReleased;
  isPressed = isReleased = false;

  menuDrawPage(&pidWaitItems);

  GUI_DispString(20, BYTE_HEIGHT * 5, textSelect(LABEL_PID_START_INFO_2));
  GUI_DispStringInRectEOL(20, BYTE_HEIGHT * 7, LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_PID_START_INFO_3));

  while (infoMenu.menu[infoMenu.cur] == menuPidWait)
  {
    if (!isPressed)                    // if touch screen is not yet pressed
    {
      if (isPress())                   // if touch screen is now pressed
        isPressed = true;
    }
    else if (!isPress())               // if touch screen is now released
    {
      isReleased = true;
    }

    if (isReleased)
      infoMenu.cur--;

    pidCheckTimeout();

    loopProcess();
  }
}

void pidStart(void)
{
  pidRunning = true;
  pidSucceeded = true;

  pidUpdateCounter();                                                          // update the number of set temperatures (number of PID processes to execute)
  pidTimeout = OS_GetTimeMs() + PID_PROCESS_TIMEOUT;                           // set timeout for overall PID process

  mustStoreCmd("M42 P4 S0\nM42 P5 S255\nM42 P6 S0\n");                         // set LED light to RED
  mustStoreCmd("M106 S255\n");                                                 // set fan speed to max
  mustStoreCmd("G4 S1\n");                                                     // wait 1 sec

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)                               // hotends + bed + chamber
  {
    if (pidHeater.T[i].target > 0)
    {
      mustStoreCmd("%s S%d\n", pidCmd[i], (int) pidHeater.T[i].target);        // start PID autotune
      mustStoreCmd("G4 S1\n");                                                 // wait 1 sec
    }
  }

  mustStoreCmd("M107\n");                                                      // stop fan
  mustStoreCmd("M42 P4 S255\nM42 P5 S0\nM42 P6 S0\n");                         // set LED light to GREEN

  infoMenu.menu[++infoMenu.cur] = menuPidWait;
}

void pidTemperatureReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%s    ", pidDisplayID[pidHeater.toolIndex]);

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  }

  sprintf(tempstr, "  %d  ", pidHeater.T[pidHeater.toolIndex].target);

  setLargeFont(true);

  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);

  setLargeFont(false);
}

void menuPid(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS pidItems = {
    // title
    LABEL_PID_TITLE,
    // icon                         label
    {{ICON_DEC,                     LABEL_DEC},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_INC,                     LABEL_INC},
     {ICON_NOZZLE,                  LABEL_NOZZLE},
     {ICON_5_DEGREE,                LABEL_5_DEGREE},
     {ICON_RESUME,                  LABEL_START},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!pidInitialized)                 // in order to keep the current set values even when reloading the menu
  {                                    // we initialize the data structure only the first time the menu is loaded
    for (uint8_t i = NOZZLE0; i < MAX_HEATER_COUNT; i++)
    {
      pidHeater.T[i].target = 0;
    }

    pidInitialized = true;
  }

  pidItems.items[KEY_ICON_4] = itemPidTool[pidHeater.toolIndex];
  pidItems.items[KEY_ICON_5] = itemPidDegree[curDegree];

  menuDrawPage(&pidItems);
  pidTemperatureReDraw(false);

  while (infoMenu.menu[infoMenu.cur] == menuPid)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (pidHeater.T[pidHeater.toolIndex].target > 0)
          pidHeater.T[pidHeater.toolIndex].target =
            limitValue(0, pidHeater.T[pidHeater.toolIndex].target - pidDegree[curDegree], infoSettings.max_temp[pidHeater.toolIndex]);

        pidTemperatureReDraw(true);
        break;

      case KEY_ICON_3:
        if (pidHeater.T[pidHeater.toolIndex].target < infoSettings.max_temp[pidHeater.toolIndex])
          pidHeater.T[pidHeater.toolIndex].target =
            limitValue(0, pidHeater.T[pidHeater.toolIndex].target + pidDegree[curDegree], infoSettings.max_temp[pidHeater.toolIndex]);

        pidTemperatureReDraw(true);
        break;

      case KEY_ICON_4:
        do
        {
          pidHeater.toolIndex = (pidHeater.toolIndex + 1) % MAX_HEATER_COUNT;
        }
        while (!heaterIsValid(pidHeater.toolIndex) || pidHeater.toolIndex == CHAMBER);

        pidItems.items[key_num] = itemPidTool[pidHeater.toolIndex];

        menuDrawItem(&pidItems.items[key_num], key_num);
        pidTemperatureReDraw(false);
        break;

      case KEY_ICON_5:
        curDegree = (curDegree + 1) % ITEM_PID_DEGREE_NUM;

        pidItems.items[key_num] = itemPidDegree[curDegree];

        menuDrawItem(&pidItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        if (pidRunning)
        {
          addToast(DIALOG_TYPE_ERROR, (char*)textSelect(LABEL_PROCESS_RUNNING));
        }
        else
        {
          pidUpdateCounter();

          if (pidCounter == 0)         // if no temperature was set to a value > 0
          {
            addToast(DIALOG_TYPE_ERROR, (char*)textSelect(LABEL_INVALID_VALUE));
          }
          else
          {
            showDialog(DIALOG_TYPE_QUESTION, textSelect(pidItems.title.index), textSelect(LABEL_PID_START_INFO),
              textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL), pidStart, NULL, NULL);
          }
        }
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    pidCheckTimeout();

    loopProcess();
  }
}
