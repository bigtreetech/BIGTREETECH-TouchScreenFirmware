#include "TuneExtruder.h"
#include "includes.h"

#define ITEM_TUNE_EXTRUDER_LEN_NUM 4

#define EXTRUDE_LEN 100.0f   // in mm
#define REMAINING_LEN 20.0f  // in mm

static uint8_t tool_index = NOZZLE0;
static uint8_t degreeSteps_index = 1;
static uint8_t extStep_index = 0;
static bool loadRequested = false;

void showNewESteps(const float measured_length, const float old_esteps, float * new_esteps)
{
  char tempstr[20];

  // First we calculate the new E-step value:
  *new_esteps = (EXTRUDE_LEN * old_esteps) / (EXTRUDE_LEN - (measured_length - REMAINING_LEN));

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
  // check and adopt current E-steps
  mustStoreCmd("M92\n");
  infoParameters.StepsPerMM[E_AXIS] = 0;  // reset E-steps value

  while (infoParameters.StepsPerMM[E_AXIS] == 0)  // wait until E-steps is updated
  {
    loopProcess();
  }

  // Home extruder and set absolute positioning
  mustStoreScript("G28\nG90\n");

  // Raise Z axis to pause height
  #if DELTA_PROBE_TYPE != 0
    mustStoreCmd("G0 Z200 F%d\n", infoSettings.pause_feedrate[FEEDRATE_Z]);
  #else
    mustStoreCmd("G0 Z%.3f F%d\n", coordinateGetAxisActual(Z_AXIS) + infoSettings.pause_z_raise,
                 infoSettings.pause_feedrate[FEEDRATE_Z]);
  #endif
  // Move to pause location
  mustStoreCmd("G0 X%.3f Y%.3f F%d\n", infoSettings.pause_pos[X_AXIS], infoSettings.pause_pos[Y_AXIS],
               infoSettings.pause_feedrate[FEEDRATE_XY]);
  // extrude 100MM
  mustStoreScript("M83\nG1 F100 E%.2f\nM82\n", EXTRUDE_LEN);

  OPEN_MENU(menuNewExtruderESteps);
}

void menuTuneExtruder(void)
{
  MENUITEMS tuneExtruderItems = {
    // title
    LABEL_TUNE_EXT_TEMP,
    // icon                          label
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
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
  temperatureReDraw(tool_index, NULL, true);

  while (MENU_IS(menuTuneExtruder))
  {
    actCurrent = heatGetCurrentTemp(tool_index);
    actTarget = heatGetTargetTemp(tool_index);
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        heatSetTargetTemp(tool_index, actTarget - degreeSteps[degreeSteps_index], FROM_GUI);
        break;

      case KEY_INFOBOX:
      {
        int16_t val = editIntValue(0, infoSettings.max_temp[tool_index], 0, actTarget);

        if (val != actTarget)
          heatSetTargetTemp(tool_index, val, FROM_GUI);

        temperatureReDraw(tool_index, NULL, true);
        break;
      }

      case KEY_ICON_3:
      case KEY_INCREASE:
        heatSetTargetTemp(tool_index, actTarget + degreeSteps[degreeSteps_index], FROM_GUI);
        break;

      case KEY_ICON_4:
        tool_index = (tool_index + 1) % infoSettings.ext_count;

        temperatureReDraw(tool_index, NULL, true);
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
        COOLDOWN_TEMPERATURE();
        CLOSE_MENU();
        break;

      default:
        break;
    }

    if (loadRequested == true)
    {
      if (tool_index != heatGetCurrentTool())
        mustStoreCmd("%s\n", tool_change[tool_index]);

      switch (warmupNozzle())
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
            popupDialog(DIALOG_TYPE_QUESTION, tuneExtruderItems.title.index, (uint8_t *) tempMsg, LABEL_EXTRUDE, LABEL_CANCEL, extrudeFilament, NULL, NULL);
          }
          loadRequested = false;
          break;
      }
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      temperatureReDraw(tool_index, NULL, false);
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
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_INC,                     LABEL_INC},
      {ICON_EEPROM_SAVE,             LABEL_SAVE},
      {ICON_1_MM,                    LABEL_1_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;
  float measured_length;
  float now = measured_length = REMAINING_LEN;
  float old_esteps, new_esteps;  // get the value of the E-steps

  old_esteps = getParameter(P_STEPS_PER_MM, E_AXIS);  // get the value of the E-steps

  newExtruderESteps.items[KEY_ICON_5] = itemMoveLen[extStep_index];

  menuDrawPage(&newExtruderESteps);
  showNewESteps(measured_length, old_esteps, &new_esteps);

  while (MENU_IS(menuNewExtruderESteps))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_DECREASE:
        measured_length -= moveLenSteps[extStep_index];
        break;

      case KEY_ICON_3:
      case KEY_INCREASE:
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
        CLOSE_MENU();
        break;

      default:
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
