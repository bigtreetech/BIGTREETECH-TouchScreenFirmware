#include "TuneExtruder.h"
#include "includes.h"

#define ITEM_TUNE_EXTRUDER_LEN_NUM 4

#define EXTRUDE_LEN 100.0f // in MM

static uint8_t tool_index = NOZZLE0;
static uint8_t degreeSteps_index = 1;
static uint8_t extStep_index = 0;
static bool loadRequested = false;

// set the hotend to the minimum extrusion temperature if user selected "OK"
void extrudeMinTemp_OK(void)
{
  heatSetTargetTemp(tool_index, infoSettings.min_ext_temp);
}

static inline void turnHeaterOff(void)
{
  heatSetTargetTemp(tool_index, 0);
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
  *new_esteps = (EXTRUDE_LEN * old_esteps) / (EXTRUDE_LEN - (measured_length - 20));

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
  // Home extruder
  mustStoreScript("G28\nG90\n");
  // Raise Z axis to pause height
  mustStoreCmd("G0 Z%.3f F%d\n", coordinateGetAxisActual(Z_AXIS) + infoSettings.pause_z_raise,
                  infoSettings.pause_feedrate[FEEDRATE_Z]);
  // Move to pause location
  mustStoreCmd("G0 X%.3f Y%.3f F%d\n", infoSettings.pause_pos[X_AXIS], infoSettings.pause_pos[Y_AXIS],
                  infoSettings.pause_feedrate[FEEDRATE_XY]);
  // extrude 100MM
  mustStoreScript("M83\nG1 F50 E%.2f\nM82\n", EXTRUDE_LEN);

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
  int16_t lastCurrent = heatGetCurrentTemp(tool_index);
  int16_t lastTarget = heatGetTargetTemp(tool_index);
  int16_t actCurrent;
  int16_t actTarget;

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  tuneExtruderItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  menuDrawPage(&tuneExtruderItems);
  temperatureReDraw(tool_index, NULL, false);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuTuneExtruder)
  {
    actCurrent = heatGetCurrentTemp(tool_index);
    actTarget = heatGetTargetTemp(tool_index);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetTargetTemp(tool_index, actTarget - degreeSteps[degreeSteps_index]);
        break;

      case KEY_INFOBOX:
      {
        int16_t val = editIntValue(0, infoSettings.max_temp[tool_index], 0, actTarget);

        if (val != actTarget)
          heatSetTargetTemp(tool_index, val);

        menuDrawPage(&tuneExtruderItems);
        temperatureReDraw(tool_index, NULL, false);
        break;
      }

      case KEY_ICON_3:
        heatSetTargetTemp(tool_index, actTarget + degreeSteps[degreeSteps_index]);
        break;

      case KEY_ICON_4:
        tool_index = (tool_index + 1) % infoSettings.hotend_count;

        temperatureReDraw(tool_index, NULL, false);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        tuneExtruderItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

        menuDrawItem(&tuneExtruderItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        loadRequested = true;
        break;

      case KEY_ICON_7:
        if (heatGetTargetTemp(tool_index) > 0)
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
            heatSetTargetTemp(tool_index, actTarget + degreeSteps[degreeSteps_index] * encoderPosition);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (loadRequested == true)
    {
      switch (warmupNozzle(tool_index, extrudeMinTemp_OK))
      {
        case COLD:
          loadRequested = false;
          break;

        case SETTLING:
          break;

        case HEATED:
          {
            char tempMsg[120];

            LABELCHAR(tempStr, LABEL_TUNE_EXT_MARK120MM);

            sprintf(tempMsg, tempStr, textSelect(LABEL_EXTRUDE));
            setDialogText(tuneExtruderItems.title.index, (uint8_t *) tempMsg, LABEL_EXTRUDE, LABEL_CANCEL);
            showDialog(DIALOG_TYPE_QUESTION, extrudeFilament, NULL, NULL);
          }
          loadRequested = false;
          break;
      }
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      temperatureReDraw(tool_index, NULL, true);
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

  newExtruderESteps.items[KEY_ICON_5] = itemMoveLen[extStep_index];

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
        measured_length -= moveLenSteps[extStep_index];
        break;

      case KEY_ICON_3:
        measured_length += moveLenSteps[extStep_index];
        break;

      case KEY_ICON_4:
      {
        char tempMsg[120];
        LABELCHAR(tempStr, LABEL_TUNE_EXT_ESTEPS_SAVED);

        sendParameterCmd(P_STEPS_PER_MM, AXIS_INDEX_E0, new_esteps);
        sprintf(tempMsg, tempStr, new_esteps);
        popupReminder(DIALOG_TYPE_QUESTION, newExtruderESteps.title.index, (uint8_t *) tempMsg);
        break;
      }

      case KEY_ICON_5:
        extStep_index = (extStep_index + 1) % ITEM_TUNE_EXTRUDER_LEN_NUM;
        newExtruderESteps.items[key_num] = itemMoveLen[extStep_index];

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
            measured_length += moveLenSteps[extStep_index] * encoderPosition;
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
