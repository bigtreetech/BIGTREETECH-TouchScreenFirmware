#include "MachineParameters.h"
#include "includes.h"

#define MAX_ELEMENT_COUNT 10

const uint8_t parameterElementCount[PARAMETERS_COUNT] = {
  AXIS_INDEX_COUNT,           // Steps/mm (X, Y, Z, E0, E1)
  3,                          // Filament Diameter (Enable, E0, E1)
  AXIS_INDEX_COUNT,           // MaxAcceleration (X, Y, Z, E0, E1)
  AXIS_INDEX_COUNT,           // MaxFeedrate (X, Y, Z, E0, E1)
  3,                          // Acceleration (Print, Retract, Travel)
  (AXIS_INDEX_COUNT - 1),     // Jerk (X, Y, Z, E)
  1,                          // Junction Deviation
  (AXIS_INDEX_COUNT - 2),     // Home offset (X, Y, Z)
  4,                          // FW retract (Length, Swap Length, Feedrate, Z lift height)
  4,                          // FW retract recover (Additional length, Additional Swap Length, Feedrate, Swap feedrate)
  1,                          // Set auto FW retract
  (AXIS_INDEX_COUNT - 2),     // Hotend Offset (X, Y, Z)
  3,                          // Hotend PID
  3,                          // Bed PID
  2,                          // ABL State & Z Fade
  STEPPER_INDEX_COUNT,        // TMC StealthChop (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
  4,                          // Delta Configuration
  3,                          // Delta Tower Angle
  3,                          // Delta Diagonal Rod Trim
  3,                          // Delta Endstop Adjustments
  (AXIS_INDEX_COUNT - 2),     // Probe offset (X, Y, Z)
  2,                          // Linear Advance (E0, E1)
  STEPPER_INDEX_COUNT,        // Stepper Motor Current (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
  STEPPER_INDEX_COUNT,        // TMC Hybrid Threshold Speed (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
  (STEPPER_INDEX_COUNT - 2),  // TMC Bump Sensitivity (X, X2, Y, Y2, Z, Z2, Z3, Z4)
  1                           // MBL offset
};

const char * const parameterCode[PARAMETERS_COUNT] = {
  "M92",   // Steps/mm
  "M200",  // Filament Diameter
  "M201",  // MaxAcceleration
  "M203",  // MaxFeedrate
  "M204",  // Acceleration
  "M205",  // Jerk
  "M205",  // Junction Deviation
  "M206",  // Home offset
  "M207",  // FW retract
  "M208",  // FW retract recover
  "M209",  // Set auto FW retract
  "M218",  // Hotend Offset
  "M301",  // Hotend PID
  "M304",  // Bed PID
  "M420",  // ABL State & Z Fade
  "M569",  // TMC StealthChop
  "M665",  // Delta Configuration
  "M665",  // Delta Tower Angle
  "M665",  // Delta Diagonal Rod Trim
  "M666",  // Delta Endstop Adjustments
  "M851",  // Probe offset
  "M900",  // Linear Advance
  "M906",  // Stepper Motor Current
  "M913",  // TMC Hybrid Threshold Speed
  "M914",  // TMC Bump Sensitivity
  "G29",   // MBL offset
};

const char * const parameterCmd[PARAMETERS_COUNT][MAX_ELEMENT_COUNT] = {
  {"X%.2f\n",            "Y%.2f\n",       "Z%.2f\n",       "T0 E%.2f\n",   "T1 E%.2f\n",   NULL,           NULL,           NULL,           NULL,           NULL},           // Steps/mm (X, Y, Z, E0, E1)
  {"S%.0f\n",            "S1 T0 D%.2f\n", "S1 T1 D%.2f\n", NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Filament Diameter (Enable, E0, E1)
  {"X%.0f\n",            "Y%.0f\n",       "Z%.0f\n",       "T0 E%.0f\n",   "T1 E%.0f\n",   NULL,           NULL,           NULL,           NULL,           NULL},           // MaxAcceleration (X, Y, Z, E0, E1)
  {"X%.0f\n",            "Y%.0f\n",       "Z%.0f\n",       "T0 E%.0f\n",   "T1 E%.0f\n",   NULL,           NULL,           NULL,           NULL,           NULL},           // MaxFeedrate (X, Y, Z, E0, E1)
  {"P%.0f\n",            "R%.0f\n",       "T%.0f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Acceleration (Print, Retract, Travel)
  {"X%.0f\n",            "Y%.0f\n",       "Z%.2f\n",       "E%.2f\n",      NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Jerk (X, Y, Z, E)
  {"J%.3f\n",            NULL,            NULL,            NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Junction Deviation
  {"X%.2f\n",            "Y%.2f\n",       "Z%.2f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Home offset (X, Y, Z)
  {"S%.2f\n",            "W%.2f\n",       "F%.2f\n",       "Z%.2f\n",      NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // FW retract (Length, Swap Length, Feedrate, Z lift height)
  {"S%.2f\n",            "W%.2f\n",       "F%.2f\n",       "R%.2f\n",      NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // FW retract recover (Additional length, Additional Swap Length, Feedrate, Swap feedrate)
  {"S%.0f\n",            NULL,            NULL,            NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Set auto FW retract
  {"T1 X%.2f\n",         "T1 Y%.2f\n",    "T1 Z%.2f\n",    NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Hotend Offset (X, Y, Z)
  {"P%.4f\n",            "I%.4f\n",       "D%.4f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Hotend PID
  {"P%.4f\n",            "I%.4f\n",       "D%.4f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Bed PID
  {"S%.0f\n",            "Z%.2f\n",       NULL,            NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // ABL State & Z Fade
  {"S%.0f I0 X\n",       "S%.0f I1 X\n",  "S%.0f I0 Y\n",  "S%.0f I1 Y\n", "S%.0f I0 Z\n", "S%.0f I1 Z\n", "S%.0f I2 Z\n", "S%.0f I3 Z\n", "S%.0f T0 E\n", "S%.0f T1 E\n"}, // TMC StealthChop (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
  {"H%.4f\n",            "S%.4f\n",       "R%.4f\n",       "L%.4f\n",      NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Delta Configuration (Height, Segment per sec, Radius, Diagonal Rod)
  {"X%.4f\n",            "Y%.4f\n",       "Z%.4f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Delta Tower Angle (Tx, Ty, Tz)
  {"A%.4f\n",            "B%.4f\n",       "C%.4f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Delta Diagonal Rod Trim (Dx, Dy, Dz)
  {"X%.4f\n",            "Y%.4f\n",       "Z%.4f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Delta Endstop Adjustments (Ex, Ey, Ez)
  {"X%.2f\n",            "Y%.2f\n",       "Z%.2f\n",       NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Probe offset (X, Y, Z)
  {"T0 K%.2f\n",         "T1 K%.2f\n",    NULL,            NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // Linear Advance (E0, E1)
  {"I0 X%.0f\n",         "I1 X%.0f\n",    "I0 Y%.0f\n",    "I1 Y%.0f\n",   "I0 Z%.0f\n",   "I1 Z%.0f\n",   "I2 Z%.0f\n",   "I3 Z%.0f\n"    "T0 E%.0f\n",   "T1 E%.0f\n"},   // Stepper Motor Current (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
  {"I1 X%.0f\n",         "I2 X%.0f\n",    "I1 Y%.0f\n",    "I2 Y%.0f\n",   "I1 Z%.0f\n",   "I2 Z%.0f\n",   "I3 Z%.0f\n",   "I4 Z%.0f\n"    "T0 E%.0f\n",   "T1 E%.0f\n"},   // TMC Hybrid Threshold Speed (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
  {"I1 X%.0f\n",         "I2 X%.0f\n",    "I1 Y%.0f\n",    "I2 Y%.0f\n",   "I1 Z%.0f\n",   "I2 Z%.0f\n",   "I3 Z%.0f\n",   "I4 Z%.0f\n",   NULL,           NULL},           // TMC Bump Sensitivity (X, X2, Y, Y2, Z, Z2, Z3, Z4)
  {"S4 Z%.2f\nG29 S0\n", NULL,            NULL,            NULL,           NULL,           NULL,           NULL,           NULL,           NULL,           NULL},           // MBL offset
};

const VAL_TYPE parameterValType[PARAMETERS_COUNT][MAX_ELEMENT_COUNT] = {
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,   VAL_TYPE_FLOAT},  // Steps/mm (X, Y, Z, E0, E1)
  {VAL_TYPE_INT,        VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT},                                         // Filament Diameter (Enable, E0, E1)
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT},    // MaxAcceleration (X, Y, Z, E0, E1)
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT},    // MaxFeedrate (X, Y, Z, E0, E1)
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT},                                           // Acceleration (Print, Retract, Travel)
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT},                    // Jerk (X, Y, Z, E)
  {VAL_TYPE_FLOAT},                                                                                  // Junction Deviation
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                     // Home offset (X, Y, Z)
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_INT,        VAL_TYPE_FLOAT},                    // FW retract (Length, Swap Length, Feedrate, Z lift height)
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_INT,        VAL_TYPE_INT},                      // FW retract recover (Additional length, Additional Swap Length, Feedrate, Swap feedrate)
  {VAL_TYPE_INT},                                                                                    // Set auto FW retract
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                     // Hotend Offset (X, Y, Z)
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT},                                         // Hotend PID
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT},                                         // Bed PID
  {VAL_TYPE_INT,        VAL_TYPE_FLOAT},                                                             // ABL State + Z Fade
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT,     // TMC StealthChop (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
   VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT},
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT},                    // Delta Configuration (Height, Segment per sec, Radius, Diagonal Rod)
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                     // Delta Tower Angle (Tx, Ty, Tz)
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                     // Delta Diagonal Rod Trim (Dx, Dy, Dz)
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                     // Delta Endstop Adjustments (Ex, Ey, Ez)
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                     // Probe offset (X, Y, Z)
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT},                                                             // Linear Advance (E0, E1)
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT,     // Stepper Motor Current (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
   VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT,},
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT,     // TMC Hybrid Threshold Speed (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1)
   VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,        VAL_TYPE_INT,     VAL_TYPE_INT,},
  {VAL_TYPE_NEG_INT,    VAL_TYPE_NEG_INT,   VAL_TYPE_NEG_INT,    VAL_TYPE_NEG_INT, VAL_TYPE_NEG_INT, // TMC Bump Sensitivity (X, X2, Y, Y2, Z, Z2, Z3, Z4)
   VAL_TYPE_NEG_INT,    VAL_TYPE_NEG_INT,   VAL_TYPE_NEG_INT,},
  {VAL_TYPE_NEG_FLOAT},                                                                              // MBL offset
};

PARAMETERS infoParameters;
uint32_t parametersEnabled = 0;
uint16_t elementEnabled[PARAMETERS_COUNT];  // parameterElementCount must be less than 16

#define ONOFF_DISPLAY_ID "1=ON 0=OFF"

// param attributes multi purpose hard coded labels
char * const axisDisplayID[AXIS_INDEX_COUNT] = AXIS_DISPLAY_ID;
char * const stepperDisplayID[STEPPER_INDEX_COUNT] = STEPPER_DISPLAY_ID;

// param attributes hard coded labels
char * const filamentDiaDisplayID[] = {"S " ONOFF_DISPLAY_ID, "T0 Ø Filament", "T1 Ø Filament"};
char * const autoRetractDisplayID[] = {"S " ONOFF_DISPLAY_ID};
char * const hotendPidDisplayID[] = {"Kp", "Ki", "Kd"};
char * const bedPidDisplayID[] = {"Kp", "Ki", "Kd"};
char * const ablStateDisplayID[] = {"S " ONOFF_DISPLAY_ID, "Z fade height"};
char * const stealthChopDisplayID[] = {"X " ONOFF_DISPLAY_ID, "X2 " ONOFF_DISPLAY_ID, "Y "ONOFF_DISPLAY_ID, "Y2 "ONOFF_DISPLAY_ID,
                                       "Z " ONOFF_DISPLAY_ID, "Z2 " ONOFF_DISPLAY_ID, "Z3 " ONOFF_DISPLAY_ID, "Z4 " ONOFF_DISPLAY_ID,
                                       "E0 "ONOFF_DISPLAY_ID, "E1 "ONOFF_DISPLAY_ID};
char * const deltaConfigurationDisplayID[] = {"Height", "Segment/sec.", "Radius", "Diagonal Rod"};
char * const deltaTowerAngleDisplayID[] = {"Tx", "Ty", "Tz"};
char * const deltaDiagonalRodDisplayID[] = {"Dx", "Dy", "Dz"};
char * const deltaEndstopDisplayID[] = {"Ex", "Ey", "Ez"};
char * const linAdvDisplayID[] = {"K-Factor E0", "K-Factor E1"};

// param attributes configurable labels
const LABEL accelDisplayID[] = {LABEL_PRINT_ACCELERATION, LABEL_RETRACT_ACCELERATION, LABEL_TRAVEL_ACCELERATION};
const LABEL junctionDeviationDisplayID[] = {LABEL_JUNCTION_DEVIATION};
const LABEL retractDisplayID[] = {LABEL_RETRACT_LENGTH, LABEL_RETRACT_SWAP_LENGTH, LABEL_RETRACT_FEEDRATE, LABEL_RETRACT_Z_LIFT};
const LABEL recoverDisplayID[] = {LABEL_RECOVER_LENGTH, LABEL_SWAP_RECOVER_LENGTH, LABEL_RECOVER_FEEDRATE, LABEL_SWAP_RECOVER_FEEDRATE};

static inline void setElementStatus(PARAMETER_NAME name, uint8_t element, bool status)
{
  SET_BIT_VALUE(elementEnabled[name], element, status);
}

static inline uint8_t getElementStatus(PARAMETER_NAME name, uint8_t element)
{
  return GET_BIT(elementEnabled[name], element);
}

uint8_t getEnabledElementCount(PARAMETER_NAME name)
{
  uint8_t count = 0;
  for (uint8_t i = 0; i < parameterElementCount[name]; i++) { count += GET_BIT(elementEnabled[name], i); }
  return count;
}

uint8_t getEnabledElement(PARAMETER_NAME name, uint8_t index)
{
  uint8_t count = 0;
  uint8_t state = 0;

  for (uint8_t i = 0; i < parameterElementCount[name]; i++)
  {
    state = GET_BIT(elementEnabled[name], i);
    count += state;

    if (state && count == (index + 1))
      return i;
  }
  return parameterElementCount[name];
}

static inline void setParameterStatus(PARAMETER_NAME name, bool status)
{
  SET_BIT_VALUE(parametersEnabled, name, status);
}

static inline uint8_t getParameterStatus(PARAMETER_NAME name)
{
  return GET_BIT(parametersEnabled, name);
}

uint8_t getEnabledParameterCount(void)
{
  uint8_t count = 0;
  for (uint8_t i = 0; i < PARAMETERS_COUNT; i++) { count += GET_BIT(parametersEnabled, i); }
  return count;
}

PARAMETER_NAME getEnabledParameter(uint8_t index)
{
  uint8_t count = 0;
  uint8_t state = 0;

  for (uint8_t i = 0; i < PARAMETERS_COUNT; i++)
  {
    state = GET_BIT(parametersEnabled, i);
    count += state;

    if (state && count == (index + 1))
      return i;
  }
  return PARAMETERS_COUNT;
}

float getParameter(PARAMETER_NAME name, uint8_t index)
{
  if (index >= parameterElementCount[name] || !getParameterStatus(name) || !getElementStatus(name, index))
    return 0.0f;

  switch (name)
  {
    case P_STEPS_PER_MM:
      return infoParameters.StepsPerMM[index];

    case P_FILAMENT_DIAMETER:
      return infoParameters.FilamentSetting[index];

    case P_MAX_ACCELERATION:
      return infoParameters.MaxAcceleration[index];

    case P_MAX_FEED_RATE:
      return infoParameters.MaxFeedRate[index];

    case P_ACCELERATION:
      return infoParameters.Acceleration[index];

    case P_JERK:
      return infoParameters.Jerk[index];

    case P_JUNCTION_DEVIATION:
      return infoParameters.JunctionDeviation[index];

    case P_HOME_OFFSET:
      return infoParameters.HomeOffset[index];

    case P_FWRETRACT:
      return infoParameters.FwRetract[index];

    case P_FWRECOVER:
      return infoParameters.FwRecover[index];

    case P_AUTO_RETRACT:
      return infoParameters.AutoRetract[index];

    case P_HOTEND_OFFSET:
      return infoParameters.HotendOffset[index];

    case P_HOTEND_PID:
      return infoParameters.HotendPid[index];

    case P_BED_PID:
      return infoParameters.BedPid[index];

    case P_ABL_STATE:
      return infoParameters.ABLState[index];

    case P_STEALTH_CHOP:
      return infoParameters.StealthChop[index];

    case P_DELTA_CONFIGURATION:
      return infoParameters.DeltaConfiguration[index];

    case P_DELTA_TOWER_ANGLE:
      return infoParameters.DeltaTowerAngle[index];

    case P_DELTA_DIAGONAL_ROD:
      return infoParameters.DeltaDiagonalRod[index];

    case P_DELTA_ENDSTOP:
      return infoParameters.DeltaEndstop[index];

    case P_PROBE_OFFSET:
      return infoParameters.ProbeOffset[index];

    case P_LIN_ADV:
      return infoParameters.LinAdvance[index];

    case P_CURRENT:
      return infoParameters.Current[index];

    case P_HYBRID_THRESHOLD:
      return infoParameters.HybridThreshold[index];

    case P_BUMPSENSITIVITY:
      return infoParameters.BumpSensitivity[index];

    case P_MBL_OFFSET:
      return infoParameters.MblOffset[index];

    default:
      return 0.0f;
  }
}

void setParameter(PARAMETER_NAME name, uint8_t index, float val)
{
  if (index >= parameterElementCount[name])
    return;

  setParameterStatus(name, true);
  setElementStatus(name, index, true);

  switch (name)
  {
    case P_STEPS_PER_MM:
      infoParameters.StepsPerMM[index] = val;
      break;

    case P_FILAMENT_DIAMETER:
      infoParameters.FilamentSetting[index] = val;
      break;

    case P_MAX_ACCELERATION:
      infoParameters.MaxAcceleration[index] = val;
      break;

    case P_MAX_FEED_RATE:
      infoParameters.MaxFeedRate[index] = val;
      break;

    case P_ACCELERATION:
      infoParameters.Acceleration[index] = val;
      break;

    case P_JERK:
      infoParameters.Jerk[index] = val;
      break;

    case P_JUNCTION_DEVIATION:
      infoParameters.JunctionDeviation[index] = val;
      break;

    case P_HOME_OFFSET:
      infoParameters.HomeOffset[index] = val;
      break;

    case P_FWRETRACT:
      infoParameters.FwRetract[index] = val;
      break;

    case P_FWRECOVER:
      infoParameters.FwRecover[index] = val;
      break;

    case P_AUTO_RETRACT:
      infoParameters.AutoRetract[index] = val;
      break;

    case P_HOTEND_OFFSET:
      infoParameters.HotendOffset[index] = val;
      break;

    case P_HOTEND_PID:
      infoParameters.HotendPid[index] = val;
      break;

    case P_BED_PID:
      infoParameters.BedPid[index] = val;
      break;

    case P_ABL_STATE:
      infoParameters.ABLState[index] = val;
      break;

    case P_STEALTH_CHOP:
      infoParameters.StealthChop[index] = val;
      break;

    case P_DELTA_CONFIGURATION:
      infoParameters.DeltaConfiguration[index] = val;
      break;

    case P_DELTA_TOWER_ANGLE:
      infoParameters.DeltaTowerAngle[index] = val;
      break;

    case P_DELTA_DIAGONAL_ROD:
      infoParameters.DeltaDiagonalRod[index] = val;
      break;

    case P_DELTA_ENDSTOP:
      infoParameters.DeltaEndstop[index] = val;
      break;

    case P_PROBE_OFFSET:
      infoParameters.ProbeOffset[index] = val;
      break;

    case P_LIN_ADV:
      infoParameters.LinAdvance[index] = val;
      break;

    case P_CURRENT:
      infoParameters.Current[index] = val;
      break;

    case P_HYBRID_THRESHOLD:
      infoParameters.HybridThreshold[index] = val;
      break;

    case P_BUMPSENSITIVITY:
      infoParameters.BumpSensitivity[index] = val;
      break;

    case P_MBL_OFFSET:
      infoParameters.MblOffset[index] = val;
      break;

    default:
      break;
  }
}

uint8_t getElementCount(PARAMETER_NAME para)
{
  return parameterElementCount[para];
}

VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index)
{
  return parameterValType[para][index];
}

void sendParameterCmd(PARAMETER_NAME name, uint8_t elementIndex, float Value)
{
  char tempCmd[30];
  sprintf(tempCmd, "%s %s", parameterCode[name], parameterCmd[name][elementIndex]);
  mustStoreScript(tempCmd, Value);  // mustStoreScript() used because parameterCmd[] can have more than one command
}

void saveEepromSettings(void)
{
  if (infoMachineSettings.EEPROM == 1)
    mustStoreCmd("M500\n");
}

void restoreEepromSettings(void)
{
  if (infoMachineSettings.EEPROM == 1)
    mustStoreScript("M501\nM503 S0\n");
}

void resetEepromSettings(void)
{
  if (infoMachineSettings.EEPROM == 1)
    mustStoreScript("M502\nM500\nM503 S0\n");
}
