#include "Pid.h"
#include "includes.h"

//#define ENABLE_PID_STATUS_UPDATE_NOTIFICATION

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

const char *const pidCmd[] = PID_CMD;
static int16_t pidHeaterTarget[MAX_HEATER_COUNT] = {0};
static uint8_t curToolIndex = NOZZLE0;
static uint8_t degreeSteps_index = 1;
u32 pidTimeout = 0;
uint8_t pidCounter = 0;
bool pidSucceeded = false;
bool pidRunning = false;
bool pidInitialized = false;

/* called by parseAck() to notify PID process status */
void pidUpdateStatus(bool succeeded)
{
  if (pidCounter > 0)
    pidCounter--;

  if (!succeeded)  // if one PID process fails, the overall PID process must be marked as failed so no save to EEPROM will be allowed
    pidSucceeded = false;

  if (pidCounter > 0)  // if all the PID processes were still not terminated, simply provide a notification
  {
  #ifdef ENABLE_PID_STATUS_UPDATE_NOTIFICATION
      LABELCHAR(tempMsg, LABEL_PID_TITLE);

      if (succeeded)
      {
        sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_PROCESS_COMPLETED));
        BUZZER_PLAY(sound_notify);
        addToast(DIALOG_TYPE_INFO, tempMsg);
      }
      else
      {
        sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_PROCESS_ABORTED));
        BUZZER_PLAY(sound_error);
        addToast(DIALOG_TYPE_ERROR, tempMsg);
      }
  #endif
  }
  else  // if all the PID processes terminated, provide the final dialog
  {
    pidRunning = false;

    if (pidSucceeded)  // if all the PID processes successfully terminated, allow to save to EEPROM
    {
      BUZZER_PLAY(sound_success);

      LABELCHAR(tempMsg, LABEL_PROCESS_COMPLETED);

      if (infoMachineSettings.EEPROM == 1)
      {
        sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

        setDialogText(LABEL_PID_TITLE, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
      }
      else
      {
        popupReminder(DIALOG_TYPE_SUCCESS, LABEL_PID_TITLE, (uint8_t *) tempMsg);
      }
    }
    else  // if at least a PID process failed, provide an error dialog
    {
      BUZZER_PLAY(sound_error);

      popupReminder(DIALOG_TYPE_ERROR, LABEL_PID_TITLE, LABEL_PROCESS_ABORTED);
    }
  }
}

static inline void pidCheckTimeout(void)
{
  if (pidRunning)
  {
    if (OS_GetTimeMs() > pidTimeout)
    {
      pidRunning = false;
    //  uint8_t pidCounter = 0;  // we voluntary don't reset (commented out the code) also pidCounter and pidSucceeded to let the
    //  pidSucceeded = false;  // pidUpdateStatus function allow to handle status updates eventually arriving after the timeout
      LABELCHAR(tempMsg, LABEL_TIMEOUT_REACHED);

      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_PROCESS_ABORTED));
      BUZZER_PLAY(sound_error);
      popupReminder(DIALOG_TYPE_ERROR, LABEL_PID_TITLE, (uint8_t *) tempMsg);
    }
  }
}

static inline void pidUpdateCounter(void)
{
  pidCounter = 0;

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)  // hotends + bed + chamber
  {
    if (pidHeaterTarget[i] > 0)
      pidCounter++;
  }
}

void menuPidWait(void)
{
  bool isPressed = false;
  bool isReleased = false;

  menuDrawPage(&pidWaitItems);

  GUI_DispString(20, BYTE_HEIGHT * 5, textSelect(LABEL_PID_START_INFO_2));
  GUI_DispStringInRectEOL(20, BYTE_HEIGHT * 7, LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_PID_START_INFO_3));

  while (infoMenu.menu[infoMenu.cur] == menuPidWait)
  {
    if (!isPressed)  // if touch screen is not yet pressed
    {
      if (isPress())  // if touch screen is now pressed
        isPressed = true;
    }
    else if (!isPress())  // if touch screen is now released
    {
      isReleased = true;
    }

    if (isReleased)
      infoMenu.cur--;

    pidCheckTimeout();

    loopProcess();
  }
}

static inline void pidStart(void)
{
  pidRunning = true;
  pidSucceeded = true;

  pidUpdateCounter();  // update the number of set temperatures (number of PID processes to execute)
  pidTimeout = OS_GetTimeMs() + PID_PROCESS_TIMEOUT;  // set timeout for overall PID process

  mustStoreCmd("M150 R255 U0 B0\n");  // set LED light to RED
  mustStoreCmd("M106 S255\n");        // set fan speed to max
  mustStoreCmd("G4 S1\n");            // wait 1 sec

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)  // hotends + bed + chamber
  {
    if (pidHeaterTarget[i] > 0)
    {
      mustStoreCmd("%s S%d\n", pidCmd[i], (int)pidHeaterTarget[i]);  // start PID autotune
      mustStoreCmd("G4 S1\n");                                          // wait 1 sec
    }
  }

  mustStoreCmd("M107\n");             // stop fan
  mustStoreCmd("M150 R0 U255 B0\n");  // set LED light to GREEN

  infoMenu.menu[++infoMenu.cur] = menuPidWait;
}

void pidTemperatureReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%s    ", heatDisplayID[curToolIndex]);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
  }

  sprintf(tempstr, "  %d  ", pidHeaterTarget[curToolIndex]);
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
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

  if (!pidInitialized)  // in order to keep the current set values even when reloading the menu
  {                     // we initialize the data structure only the first time the menu is loaded
    for (uint8_t i = NOZZLE0; i < MAX_HEATER_COUNT; i++)
    {
      pidHeaterTarget[i] = 0;
    }

    pidInitialized = true;
  }

  pidItems.items[KEY_ICON_4] = itemTool[curToolIndex];
  pidItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  menuDrawPage(&pidItems);
  pidTemperatureReDraw(false);

  while (infoMenu.menu[infoMenu.cur] == menuPid)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (pidHeaterTarget[curToolIndex] > 0)
          pidHeaterTarget[curToolIndex] =
              NOBEYOND(0, pidHeaterTarget[curToolIndex] - degreeSteps[degreeSteps_index],
                       infoSettings.max_temp[curToolIndex]);

        pidTemperatureReDraw(true);
        break;

      case KEY_INFOBOX:
      {
        char titlestr[30];
        sprintf(titlestr, "Min:0 | Max:%i", infoSettings.max_temp[curToolIndex]);

        int32_t val = numPadInt((uint8_t *) titlestr, pidHeaterTarget[curToolIndex], 0, false);
        val = NOBEYOND(0, val, infoSettings.max_temp[curToolIndex]);

        if (val != pidHeaterTarget[curToolIndex])  // if value is different than target change it
          pidHeaterTarget[curToolIndex] = val;

        menuDrawPage(&pidItems);
        pidTemperatureReDraw(true);
        break;
      }

      case KEY_ICON_3:
        if (pidHeaterTarget[curToolIndex] < infoSettings.max_temp[curToolIndex])
          pidHeaterTarget[curToolIndex] =
              NOBEYOND(0, pidHeaterTarget[curToolIndex] + degreeSteps[degreeSteps_index],
                       infoSettings.max_temp[curToolIndex]);

        pidTemperatureReDraw(true);
        break;

      case KEY_ICON_4:
        do
        {
          curToolIndex = (curToolIndex + 1) % MAX_HEATER_COUNT;
        } while (!heaterIsValid(curToolIndex) || curToolIndex == CHAMBER);

        pidItems.items[key_num] = itemTool[curToolIndex];

        menuDrawItem(&pidItems.items[key_num], key_num);
        pidTemperatureReDraw(false);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        pidItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

        menuDrawItem(&pidItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        if (pidRunning)
        {
          addToast(DIALOG_TYPE_ERROR, (char *) textSelect(LABEL_PROCESS_RUNNING));
        }
        else
        {
          pidUpdateCounter();

          if (pidCounter == 0)                             // if no temperature was set to a value > 0
          {
            addToast(DIALOG_TYPE_ERROR, (char *) textSelect(LABEL_INVALID_VALUE));
          }
          else
          {
            setDialogText(pidItems.title.index, LABEL_PID_START_INFO, LABEL_CONFIRM, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, pidStart, NULL, NULL);
          }
        }
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (encoderPosition > 0)
            {
              if (pidHeaterTarget[curToolIndex] < infoSettings.max_temp[curToolIndex])
                pidHeaterTarget[curToolIndex] =
                    NOBEYOND(0, pidHeaterTarget[curToolIndex] + degreeSteps[degreeSteps_index],
                             infoSettings.max_temp[curToolIndex]);
            }
            else  // if < 0
            {
              if (pidHeaterTarget[curToolIndex] > 0)
                pidHeaterTarget[curToolIndex] =
                    NOBEYOND(0, pidHeaterTarget[curToolIndex] - degreeSteps[degreeSteps_index],
                             infoSettings.max_temp[curToolIndex]);
            }

            pidTemperatureReDraw(true);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    pidCheckTimeout();

    loopProcess();
  }
}
