#include "Pid.h"
#include "includes.h"

//#define ENABLE_PID_STATUS_UPDATE_NOTIFICATION

static const char * const pidCmdMarlin[] = PID_CMD_MARLIN;
static const char * const pidCmdRRF[]    = PID_CMD_RRF;

static int16_t pidHeaterTarget[MAX_HEATER_PID_COUNT] = {0};
static uint8_t curTool_index = NOZZLE0;
static uint8_t degreeSteps_index = 1;
static uint32_t pidTimeout = 0;
static PID_STATUS pidStatus = PID_IDLE;

// called by parseAck() to notify PID process status
void pidUpdateStatus(PID_STATUS status)
{
  if (pidStatus != PID_RUNNING)  // update only if PID tuning is running and initiated from GUI
    return;

  pidStatus = status;
}

static uint8_t checkFirstValidPID(void)
{
  uint8_t tool = 0;

  while (tool < MAX_HEATER_PID_COUNT)
  {
    if (pidHeaterTarget[tool] == 0)
      tool++;
    else
      break;
  }

  return tool;
}

static void pidRun(void)
{
  uint8_t tool = checkFirstValidPID();

  if (tool < MAX_HEATER_PID_COUNT)
  {
    mustStoreCmd("%s S%d\n", (infoMachineSettings.firmwareType != FW_REPRAPFW) ? pidCmdMarlin[tool] : pidCmdRRF[tool], (int)pidHeaterTarget[tool]);  // start PID autotune

    pidStatus = PID_RUNNING;
  }
}

static inline void pidStart(void)
{
  pidTimeout = OS_GetTimeMs() + PID_PROCESS_TIMEOUT;  // set timeout for overall PID process

  LED_SetEventColor(&ledRed, false);  // set (neopixel) LED light to RED
  LCD_SET_KNOB_LED_IDLE(false);       // set infoSettings.knob_led_idle temporary to OFF

  pidRun();
}

static inline void pidResultAction(void)
{
  if (pidStatus == PID_TIMEOUT)
  {
    memset(pidHeaterTarget, 0, sizeof(pidHeaterTarget));  // reset pidHeaterTarget[] to 0

    LABELCHAR(tempMsg, LABEL_TIMEOUT_REACHED);

    sprintf(strchr(tempMsg, '\0'), "\n %s", textSelect(LABEL_BUSY));

    BUZZER_PLAY(SOUND_NOTIFY);

    popupReminder(DIALOG_TYPE_ALERT, LABEL_PID_TITLE, (uint8_t *) tempMsg);
  }
  else if (pidStatus == PID_FAILED)
  {
    memset(pidHeaterTarget, 0, sizeof(pidHeaterTarget));  // reset pidHeaterTarget[] to 0

    BUZZER_PLAY(SOUND_ERROR);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_PID_TITLE, LABEL_PROCESS_ABORTED);
  }
  else if (pidStatus == PID_SUCCESS)
  {
    pidHeaterTarget[checkFirstValidPID()] = 0;  // reset the succesful PID tool's target temperature to 0

    if (checkFirstValidPID() == MAX_HEATER_PID_COUNT)  // no more tools left, PID autotune finished
    {
      BUZZER_PLAY(SOUND_SUCCESS);

      LABELCHAR(tempMsg, LABEL_PROCESS_COMPLETED);

      if (infoMachineSettings.EEPROM == 1)
      {
        sprintf(strchr(tempMsg, '\0'), "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

        popupDialog(DIALOG_TYPE_SUCCESS, LABEL_PID_TITLE, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
      }
      else
      {
        popupReminder(DIALOG_TYPE_SUCCESS, LABEL_PID_TITLE, (uint8_t *) tempMsg);
      }
    }
    else  // PID tuning process not finished for all tools
    {
      #ifdef ENABLE_PID_STATUS_UPDATE_NOTIFICATION
        LABELCHAR(tempMsg, LABEL_PID_TITLE);

        sprintf(strchr(tempMsg, '\0'), " %s", textSelect(LABEL_PROCESS_COMPLETED));

        BUZZER_PLAY(SOUND_NOTIFY);

        addToast(DIALOG_TYPE_INFO, tempMsg);
      #endif

      pidRun();  // autotune next tool
    }
  }

  if (checkFirstValidPID() == MAX_HEATER_PID_COUNT)  // no more tools left, PID autotune finished
  {
    pidStatus = PID_IDLE;

    LED_SetEventColor(&ledGreen, false);  // set (neopixel) LED light to GREEN
  }
}

void menuPid(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS pidItems = {
    // title
    LABEL_PID_TITLE,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_INC,                     LABEL_INC},
      {ICON_NOZZLE,                  LABEL_NOZZLE},
      {ICON_5_DEGREE,                LABEL_5_DEGREE},
      {ICON_RESUME,                  LABEL_START},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (hasMPC() && curTool_index < MAX_HOTEND_COUNT)
    curTool_index = BED;  // there's no PID for hotends if MPC enabled

  pidItems.items[KEY_ICON_4] = itemTool[curTool_index];
  pidItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  menuDrawPage(&pidItems);
  temperatureReDraw(curTool_index, &pidHeaterTarget[curTool_index], true);

  while (MENU_IS(menuPid))
  {
    if (pidStatus == PID_IDLE)
    {
      key_num = menuKeyGetValue();

      switch (key_num)
      {
        case KEY_ICON_0:
        case KEY_DECREASE:
          if (pidHeaterTarget[curTool_index] > 0)
            pidHeaterTarget[curTool_index] =
              NOBEYOND(0, pidHeaterTarget[curTool_index] - degreeSteps[degreeSteps_index], infoSettings.max_temp[curTool_index]);

          temperatureReDraw(curTool_index, &pidHeaterTarget[curTool_index], false);
          break;

        case KEY_INFOBOX:
        {
          int16_t val = editIntValue(0, infoSettings.max_temp[curTool_index], 0, pidHeaterTarget[curTool_index]);

          if (val != pidHeaterTarget[curTool_index])  // if value is different than target, change it
            pidHeaterTarget[curTool_index] = val;

          temperatureReDraw(curTool_index, &pidHeaterTarget[curTool_index], true);
          break;
        }

        case KEY_ICON_3:
        case KEY_INCREASE:
          if (pidHeaterTarget[curTool_index] < infoSettings.max_temp[curTool_index])
            pidHeaterTarget[curTool_index] =
              NOBEYOND(0, pidHeaterTarget[curTool_index] + degreeSteps[degreeSteps_index], infoSettings.max_temp[curTool_index]);

          temperatureReDraw(curTool_index, &pidHeaterTarget[curTool_index], false);
          break;

        case KEY_ICON_4:
          if (!hasMPC())
          {
            do
            {
              curTool_index = (curTool_index + 1) % MAX_HEATER_PID_COUNT;
            } while (!heaterDisplayIsValid(curTool_index));

            pidItems.items[key_num] = itemTool[curTool_index];

            menuDrawItem(&pidItems.items[key_num], key_num);
            temperatureReDraw(curTool_index, &pidHeaterTarget[curTool_index], true);
          }
          break;

        case KEY_ICON_5:
          degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
          pidItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

          menuDrawItem(&pidItems.items[key_num], key_num);
          break;

        case KEY_ICON_6:
          if (checkFirstValidPID() == MAX_HEATER_PID_COUNT)  // if no temperature was set to a value > 0
          {
            addToast(DIALOG_TYPE_ERROR, (char *) textSelect(LABEL_INVALID_VALUE));
          }
          else
          {
            popupDialog(DIALOG_TYPE_QUESTION, pidItems.title.index, LABEL_TUNE_START_INFO, LABEL_CONFIRM, LABEL_CANCEL, pidStart, NULL, NULL);
          }
          break;

        case KEY_ICON_7:
          // set (neopixel) LED light to current color or to OFF according to infoSettings.led_always_on and
          // restore infoSettings.knob_led_idle and knob LED color to their default values
          LED_SetPostEventColor();

          CLOSE_MENU();
          break;

        default:
          break;
      }
    }
    else
    {
      if (getMenuType() != MENU_TYPE_SPLASH)
        popupSplash(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);

      if (OS_GetTimeMs() >= pidTimeout)
        pidUpdateStatus(PID_TIMEOUT);

      if (pidStatus != PID_RUNNING)
        pidResultAction();
    }

    loopProcess();
  }
}
