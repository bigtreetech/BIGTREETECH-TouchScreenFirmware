#include "TuneExtruder.h"
#include "includes.h"

#define ITEM_TUNE_EXTRUDER_LEN_NUM 4

static uint8_t degreeSteps_index = 1;
static uint8_t curExtStep_index = 0;
static uint8_t c_heater = NOZZLE0;

// Show/draw temperature in heat menu
void showExtrudeTemperature(uint8_t index)
{
  char tempstr[20];

  sprintf(tempstr, "%-15s", heatDisplayID[index]);
  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);

  sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(index), heatGetTargetTemp(index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

static inline void turnHeaterOff(void)
{
  heatSetTargetTemp(c_heater, 0);
  infoMenu.cur--;
}

static inline void returnToTuning(void)
{
  infoMenu.cur--;
}

void showNewESteps(const float measured_length, const float old_esteps, float * new_esteps)
{
  char tempstr[20];

  // First we calculate the new E-step value:
  *new_esteps = (100 * old_esteps) / (100 - (measured_length - 20));

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, textSelect(LABEL_TUNE_EXT_MEASURED));

  sprintf(tempstr, "  %0.2fmm  ", measured_length);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);

  sprintf(tempstr, (char*)textSelect(LABEL_TUNE_EXT_OLD_ESTEP), old_esteps);
  GUI_DispString(exhibitRect.x0, exhibitRect.y1 - BYTE_HEIGHT, (uint8_t *)tempstr);

  sprintf(tempstr, (char*)textSelect(LABEL_TUNE_EXT_NEW_ESTEP), *new_esteps);
  GUI_DispString(exhibitRect.x0,  exhibitRect.y1, (uint8_t *)tempstr);
}

static inline void extrudeFilament(void)
{
  storeCmd("G28\n");                              // Home extruder
  mustStoreScript("G90\nG0 F3000 X0 Y0 Z100\n");  // present extruder
  mustStoreScript("M83\nG1 F50 E100\nM82\n");     // extrude
  infoMenu.menu[++infoMenu.cur] = menuNewExtruderESteps;
}
// end Esteps part

void menuTuneExtruder(void)
{
  MENUITEMS tuneExtruderItems = {
    // title
    LABEL_TUNE_EXT_TEMP,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_NOZZLE,                  LABEL_NOZZLE},
      {ICON_5_DEGREE,                LABEL_5_DEGREE},
      {ICON_LOAD,                    LABEL_TUNE_EXT_EXTRUDE_100},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  int16_t lastCurrent = heatGetCurrentTemp(c_heater);
  int16_t lastTarget = heatGetTargetTemp(c_heater);
  int16_t actCurrent;
  int16_t actTarget;

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  menuDrawPage(&tuneExtruderItems);
  showExtrudeTemperature(c_heater);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuTuneExtruder)
  {
    actCurrent = heatGetCurrentTemp(c_heater);
    actTarget = heatGetTargetTemp(c_heater);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetTargetTemp(c_heater, actTarget - degreeSteps[degreeSteps_index]);
        break;

      case KEY_INFOBOX:
        {
          char titlestr[30];
          sprintf(titlestr, "Min:0 | Max:%i", infoSettings.max_temp[c_heater]);

          int16_t val = numPadInt((uint8_t *) titlestr, actTarget, 0, false);
          val = NOBEYOND(0, val, infoSettings.max_temp[c_heater]);

          if (val != actTarget)
            heatSetTargetTemp(c_heater, val);

          menuDrawPage(&tuneExtruderItems);
          showExtrudeTemperature(c_heater);
        }
        break;

      case KEY_ICON_3:
        heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index]);
        break;

      case KEY_ICON_4:
        do
        {
          c_heater = (c_heater + 1) % MAX_HOTEND_COUNT;
        }
        while (!heaterIsValid(c_heater));

        showExtrudeTemperature(c_heater);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        tuneExtruderItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

        menuDrawItem(&tuneExtruderItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        {
          char tempMsg[120];

          if (heatGetTargetTemp(c_heater) < infoSettings.min_ext_temp)
          {
            LABELCHAR(tempStr, LABEL_TUNE_EXT_TEMPLOW);

            sprintf(tempMsg, tempStr, infoSettings.min_ext_temp);
            popupReminder(DIALOG_TYPE_ALERT, tuneExtruderItems.title.index, (uint8_t *) tempMsg);
          }
          else if (heatGetCurrentTemp(c_heater) < heatGetTargetTemp(c_heater) - 1)
          {
            popupReminder(DIALOG_TYPE_ALERT, tuneExtruderItems.title.index, LABEL_TUNE_EXT_DESIREDVAL);
          }
          else
          {
            LABELCHAR(tempStr, LABEL_TUNE_EXT_MARK120MM);

            sprintf(tempMsg, tempStr, textSelect(LABEL_EXTRUDE));
            setDialogText(tuneExtruderItems.title.index, (uint8_t *) tempMsg, LABEL_EXTRUDE, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, extrudeFilament, NULL, NULL);
          }
        }
        break;

      case KEY_ICON_7:
        if (heatGetTargetTemp(c_heater) > 0)
        {
          setDialogText(tuneExtruderItems.title.index, LABEL_TUNE_EXT_HEATOFF, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, turnHeaterOff, returnToTuning, NULL);
        }
        else
        {
          infoMenu.cur--;
        }
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index] * encoderPosition);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      showExtrudeTemperature(c_heater);
    }

    loopProcess();
  }

  // Set slow update time if not waiting for target temperature
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

void menuNewExtruderESteps(void)
{

  // Extruder steps are not correct. Ask user for the amount that's extruded
  // Automaticaly calculate new steps/mm when changing the measured distance
  // When pressing save to eeprom the new steps will be saved.
  MENUITEMS newExtruderESteps = {
    // title
    LABEL_TUNE_EXT_ADJ_ESTEPS,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_EEPROM_SAVE,             LABEL_SAVE},
      {ICON_1_MM,                    LABEL_1_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  float measured_length;
  float now = measured_length = 20.00f;
  float old_esteps, new_esteps;  // get the value of the E-steps

  mustStoreCmd("M503 S0\n");

  old_esteps = getParameter(P_STEPS_PER_MM, E_AXIS);  // get the value of the E-steps
  newExtruderESteps.items[KEY_ICON_5] = itemMoveLen[curExtStep_index];

  menuDrawPage(&newExtruderESteps);
  showNewESteps(measured_length, old_esteps, &new_esteps);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuNewExtruderESteps)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        measured_length -= moveLenSteps[curExtStep_index];
        break;

      case KEY_ICON_3:
        measured_length += moveLenSteps[curExtStep_index];
        break;

      case KEY_ICON_4:
      {
        LABELCHAR(tempStr, LABEL_TUNE_EXT_ESTEPS_SAVED);
        char tempMsg[120];

        storeCmd("M92 T0 E%0.2f\n", new_esteps);
        sprintf(tempMsg, tempStr, new_esteps);
        popupReminder(DIALOG_TYPE_QUESTION, newExtruderESteps.title.index, (uint8_t *) tempMsg);
        break;
      }

      case KEY_ICON_5:
        curExtStep_index = (curExtStep_index + 1) % ITEM_TUNE_EXTRUDER_LEN_NUM;
        newExtruderESteps.items[key_num] = itemMoveLen[curExtStep_index];

        menuDrawItem(&newExtruderESteps.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        measured_length = 0.0f;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            measured_length += moveLenSteps[curExtStep_index] * encoderPosition;
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != measured_length)
    {
      now = measured_length;
      showNewESteps(measured_length, old_esteps, &new_esteps);
    }

    loopProcess();
  }
}
