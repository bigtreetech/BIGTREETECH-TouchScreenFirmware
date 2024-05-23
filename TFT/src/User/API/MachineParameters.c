#include "MachineParameters.h"
#include "includes.h"

typedef struct {
  const char * const suffix;
  const VAL_TYPE valType;
} suffix_t;

typedef struct {
  const char * const prefix;
  const suffix_t * const suffix;
  const uint8_t cnt;
  uint16_t enabled;  // parameterElementCount must be less than 16
} parameter_member_t;

// Steps/mm (X, Y, Z, E0, E1, E2)
static const suffix_t M92_suffix[]= {
  {"X%.4f\n",    VAL_TYPE_FLOAT}, {"Y%.4f\n",    VAL_TYPE_FLOAT}, {"Z%.2f\n", VAL_TYPE_FLOAT},
  {"T0 E%.2f\n", VAL_TYPE_FLOAT}, {"T1 E%.2f\n", VAL_TYPE_FLOAT}, {"T2 E%.2f\n", VAL_TYPE_FLOAT},
};
// Filament Diameter (Enable, E0, E1, E2)
static const suffix_t M200_suffix[]= {
  {"S%.0f\n",       VAL_TYPE_INT},   {"S1 T0 D%.2f\n", VAL_TYPE_FLOAT}, {"S1 T1 D%.2f\n", VAL_TYPE_FLOAT},
  {"S1 T2 D%.2f\n", VAL_TYPE_FLOAT},
};
// MaxAcceleration (X, Y, Z, E0, E1, E2)
static const suffix_t M201_suffix[]= {
  {"X%.0f\n",    VAL_TYPE_INT}, {"Y%.0f\n",    VAL_TYPE_INT}, {"Z%.0f\n",    VAL_TYPE_INT},
  {"T0 E%.0f\n", VAL_TYPE_INT}, {"T1 E%.0f\n", VAL_TYPE_INT}, {"T2 E%.0f\n", VAL_TYPE_INT},
};
// MaxFeedrate (X, Y, Z, E0, E1, E2)
static const suffix_t M203_suffix[]= {
  {"X%.0f\n",    VAL_TYPE_INT}, {"Y%.0f\n",    VAL_TYPE_INT}, {"Z%.0f\n",    VAL_TYPE_INT},
  {"T0 E%.0f\n", VAL_TYPE_INT}, {"T1 E%.0f\n", VAL_TYPE_INT}, {"T2 E%.0f\n", VAL_TYPE_INT},
};
// Acceleration (Print, Retract, Travel)
static const suffix_t M204_suffix[]= {
  {"P%.0f\n", VAL_TYPE_INT}, {"R%.0f\n", VAL_TYPE_INT}, {"T%.0f\n", VAL_TYPE_INT},
};
// Jerk (X, Y, Z, E)
static const suffix_t M205_jerk_suffix[]= {
  {"X%.0f\n", VAL_TYPE_INT},   {"Y%.0f\n", VAL_TYPE_INT}, {"Z%.2f\n", VAL_TYPE_FLOAT},
  {"E%.2f\n", VAL_TYPE_FLOAT},
};
// Junction Deviation (X, Y, Z, E)
static const suffix_t M205_deviation_suffix[]= {
  {"J%.3f\n", VAL_TYPE_FLOAT},
};
// Home offset (X, Y, Z)
static const suffix_t M206_suffix[]= {
  {"X%.2f\n", VAL_TYPE_NEG_FLOAT}, {"Y%.2f\n", VAL_TYPE_NEG_FLOAT}, {"Z%.2f\n", VAL_TYPE_NEG_FLOAT},
};
// FW retract (Length, Swap Length, Feedrate, Z lift height)
static const suffix_t M207_suffix[]= {
  {"S%.2f\n", VAL_TYPE_FLOAT}, {"W%.2f\n", VAL_TYPE_FLOAT}, {"F%.2f\n", VAL_TYPE_INT},
  {"Z%.2f\n", VAL_TYPE_FLOAT},
};
// FW retract recover (Additional length, Additional Swap Length, Feedrate, Swap feedrate)
static const suffix_t M208_suffix[]= {
  {"S%.2f\n", VAL_TYPE_FLOAT}, {"W%.2f\n", VAL_TYPE_FLOAT}, {"F%.2f\n", VAL_TYPE_INT},
  {"R%.2f\n", VAL_TYPE_INT},
};
// Set auto FW retract
static const suffix_t M209_suffix[]= {
  {"S%.0f\n", VAL_TYPE_INT}
};
// Hotend Offset (X, Y, Z)
static const suffix_t M218_suffix[]= {
  {"T1 X%.2f\n", VAL_TYPE_NEG_FLOAT}, {"T1 Y%.2f\n", VAL_TYPE_NEG_FLOAT}, {"T1 Z%.2f\n", VAL_TYPE_NEG_FLOAT},
};
// Hotend PID
static const suffix_t M301_suffix[]= {
  {"P%.4f\n", VAL_TYPE_FLOAT}, {"I%.4f\n", VAL_TYPE_FLOAT}, {"D%.4f\n", VAL_TYPE_FLOAT},
};
// Bed PID
static const suffix_t M304_suffix[]= {
  {"P%.4f\n", VAL_TYPE_FLOAT}, {"I%.4f\n", VAL_TYPE_FLOAT}, {"D%.4f\n", VAL_TYPE_FLOAT},
};
// ABL State + Z Fade
static const suffix_t M420_suffix[]= {
  {"S%.0f\n", VAL_TYPE_INT}, {"Z%.2f\n", VAL_TYPE_FLOAT},
};
// TMC StealthChop (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1, E2)
static const suffix_t M569_suffix[]= {
  {"S%.0f I0 X\n", VAL_TYPE_INT}, {"S%.0f I1 X\n", VAL_TYPE_INT}, {"S%.0f I0 Y\n", VAL_TYPE_INT},
  {"S%.0f I1 Y\n", VAL_TYPE_INT}, {"S%.0f I0 Z\n", VAL_TYPE_INT}, {"S%.0f I1 Z\n", VAL_TYPE_INT},
  {"S%.0f I2 Z\n", VAL_TYPE_INT}, {"S%.0f I3 Z\n", VAL_TYPE_INT}, {"S%.0f T0 E\n", VAL_TYPE_INT},
  {"S%.0f T1 E\n", VAL_TYPE_INT}, {"S%.0f T2 E\n", VAL_TYPE_INT},
};
// Input Shape (X damping frequency, X damping factor, Y damping frequency, Y damping factor)
static const suffix_t M593_suffix[]= {
  {"X F%.3f\n", VAL_TYPE_FLOAT}, {"X D%.3f\n", VAL_TYPE_FLOAT}, {"Y F%.3f\n", VAL_TYPE_FLOAT},
  {"Y D%.3f\n", VAL_TYPE_FLOAT},
};
// Delta Configuration (Height, Segment per sec, Radius, Diagonal Rod)
static const suffix_t M665_hsrl_suffix[]= {
  {"H%.4f\n", VAL_TYPE_FLOAT}, {"S%.4f\n", VAL_TYPE_FLOAT}, {"R%.4f\n", VAL_TYPE_FLOAT},
  {"L%.4f\n", VAL_TYPE_FLOAT},
};
// Delta Tower Angle (Tx, Ty, Tz)
static const suffix_t M665_xyz_suffix[]= {
  {"X%.4f\n", VAL_TYPE_NEG_FLOAT}, {"Y%.4f\n", VAL_TYPE_NEG_FLOAT}, {"Z%.4f\n", VAL_TYPE_NEG_FLOAT},
};
// Delta Diagonal Rod Trim (Dx, Dy, Dz)
static const suffix_t M665_abc_suffix[]= {
  {"A%.4f\n", VAL_TYPE_NEG_FLOAT}, {"B%.4f\n", VAL_TYPE_NEG_FLOAT}, {"C%.4f\n", VAL_TYPE_NEG_FLOAT},
};
// Delta Endstop Adjustments (Ex, Ey, Ez)
static const suffix_t M666_suffix[]= {
  {"X%.4f\n", VAL_TYPE_NEG_FLOAT}, {"Y%.4f\n", VAL_TYPE_NEG_FLOAT}, {"Z%.4f\n", VAL_TYPE_NEG_FLOAT},
};
// Probe offset (X, Y, Z)
static const suffix_t M851_suffix[]= {
  {"X%.2f\n", VAL_TYPE_NEG_FLOAT}, {"Y%.2f\n", VAL_TYPE_NEG_FLOAT}, {"Z%.2f\n", VAL_TYPE_NEG_FLOAT},
};
// Linear Advance (E0, E1, E2)
static const suffix_t M900_suffix[]= {
  {"T0 K%.3f\n", VAL_TYPE_FLOAT}, {"T1 K%.3f\n", VAL_TYPE_FLOAT}, {"T2 K%.3f\n", VAL_TYPE_FLOAT},
};
// Stepper Motor Current (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1, E2)
static const suffix_t M906_suffix[]= {
  {"I0 X%.0f\n", VAL_TYPE_INT}, {"I1 X%.0f\n", VAL_TYPE_INT}, {"I0 Y%.0f\n", VAL_TYPE_INT},
  {"I1 Y%.0f\n", VAL_TYPE_INT}, {"I0 Z%.0f\n", VAL_TYPE_INT}, {"I1 Z%.0f\n", VAL_TYPE_INT},
  {"I2 Z%.0f\n", VAL_TYPE_INT}, {"I3 Z%.0f\n", VAL_TYPE_INT}, {"T0 E%.0f\n", VAL_TYPE_INT},
  {"T1 E%.0f\n", VAL_TYPE_INT}, {"T2 E%.0f\n", VAL_TYPE_INT},
};
// TMC Hybrid Threshold Speed (X, X2, Y, Y2, Z, Z2, Z3, Z4, E0, E1, E2)
static const suffix_t M913_suffix[]= {
  {"I1 X%.0f\n", VAL_TYPE_INT}, {"I2 X%.0f\n", VAL_TYPE_INT}, {"I1 Y%.0f\n", VAL_TYPE_INT},
  {"I2 Y%.0f\n", VAL_TYPE_INT}, {"I1 Z%.0f\n", VAL_TYPE_INT}, {"I2 Z%.0f\n", VAL_TYPE_INT},
  {"I3 Z%.0f\n", VAL_TYPE_INT}, {"I4 Z%.0f\n", VAL_TYPE_INT}, {"T0 E%.0f\n", VAL_TYPE_INT},
  {"T1 E%.0f\n", VAL_TYPE_INT}, {"T2 E%.0f\n", VAL_TYPE_INT},
};
// TMC Bump Sensitivity (X, X2, Y, Y2, Z, Z2, Z3, Z4)
static const suffix_t M914_suffix[]= {
  {"I1 X%.0f\n", VAL_TYPE_NEG_INT}, {"I2 X%.0f\n", VAL_TYPE_NEG_INT}, {"I1 Y%.0f\n", VAL_TYPE_NEG_INT},
  {"I2 Y%.0f\n", VAL_TYPE_NEG_INT}, {"I1 Z%.0f\n", VAL_TYPE_NEG_INT}, {"I2 Z%.0f\n", VAL_TYPE_NEG_INT},
  {"I3 Z%.0f\n", VAL_TYPE_NEG_INT}, {"I4 Z%.0f\n", VAL_TYPE_NEG_INT},
};
// MBL offset
static const suffix_t G29_suffix[]= {
  {"S4 Z%.2f\nG29 S0\n", VAL_TYPE_NEG_FLOAT},
};

parameter_member_t parameter_list[] = {
  // Steps/mm
  {"M92", M92_suffix, COUNT(M92_suffix)},
  // Filament Diameter
  {"M200", M200_suffix, COUNT(M200_suffix)},
  // MaxAcceleration
  {"M201", M201_suffix, COUNT(M201_suffix)},
  // MaxFeedrate
  {"M203", M203_suffix, COUNT(M203_suffix)},
  // Acceleration
  {"M204", M204_suffix, COUNT(M204_suffix)},
  // Jerk
  {"M205", M205_jerk_suffix, COUNT(M205_jerk_suffix)},
  // Junction Deviation
  {"M205", M205_deviation_suffix, COUNT(M205_deviation_suffix)},
  // Home offset
  {"M206", M206_suffix, COUNT(M206_suffix)},
  // FW retract
  {"M207", M207_suffix, COUNT(M207_suffix)},
  // FW retract recover
  {"M208", M208_suffix, COUNT(M208_suffix)},
  // Set auto FW retract
  {"M209", M209_suffix, COUNT(M209_suffix)},
  // Hotend Offset
  {"M218", M218_suffix, COUNT(M218_suffix)},
  // Hotend PID
  {"M301", M301_suffix, COUNT(M301_suffix)},
  // Bed PID
  {"M304", M304_suffix, COUNT(M304_suffix)},
  // ABL State & Z Fade
  {"M420", M420_suffix, COUNT(M420_suffix)},
  // TMC StealthChop
  {"M569", M569_suffix, COUNT(M569_suffix)},
  // Input Shape
  {"M593", M593_suffix, COUNT(M593_suffix)},
  // Delta Configuration
  {"M665", M665_hsrl_suffix, COUNT(M665_hsrl_suffix)},
  // Delta Tower Angle
  {"M665", M665_xyz_suffix, COUNT(M665_xyz_suffix)},
  // Delta Diagonal Rod Trim
  {"M665", M665_abc_suffix, COUNT(M665_abc_suffix)},
  // Delta Endstop Adjustments
  {"M666", M666_suffix, COUNT(M666_suffix)},
  // Probe offset
  {"M851", M851_suffix, COUNT(M851_suffix)},
  // Linear Advance
  {"M900", M900_suffix, COUNT(M900_suffix)},
  // Stepper Motor Current
  {"M906", M906_suffix, COUNT(M906_suffix)},
  // TMC Hybrid Threshold Speed
  {"M913", M913_suffix, COUNT(M913_suffix)},
  // TMC Bump Sensitivity
  {"M914", M914_suffix, COUNT(M914_suffix)},
  // MBL offset
  {"G29", G29_suffix, COUNT(G29_suffix)},
};

typedef struct {
  float StepsPerMM[COUNT(M92_suffix)];
  float FilamentSetting[COUNT(M200_suffix)];
  float MaxAcceleration[COUNT(M201_suffix)];
  float MaxFeedRate[COUNT(M203_suffix)];
  float Acceleration[COUNT(M204_suffix)];
  float Jerk[COUNT(M205_jerk_suffix)];
  float JunctionDeviation[COUNT(M205_deviation_suffix)];
  float HomeOffset[COUNT(M206_suffix)];
  float FwRetract[COUNT(M207_suffix)];
  float FwRecover[COUNT(M208_suffix)];
  float AutoRetract[COUNT(M209_suffix)];
  float HotendOffset[COUNT(M218_suffix)];
  float HotendPid[COUNT(M301_suffix)];
  float BedPid[COUNT(M304_suffix)];
  float ABLState[COUNT(M420_suffix)];
  float StealthChop[COUNT(M569_suffix)];
  float InputShaping[COUNT(M593_suffix)];
  float DeltaConfiguration[COUNT(M665_hsrl_suffix)];
  float DeltaTowerAngle[COUNT(M665_xyz_suffix)];
  float DeltaDiagonalRod[COUNT(M665_abc_suffix)];
  float DeltaEndstop[COUNT(M666_suffix)];
  float ProbeOffset[COUNT(M851_suffix)];
  float LinAdvance[COUNT(M900_suffix)];
  float Current[COUNT(M906_suffix)];
  float HybridThreshold[COUNT(M913_suffix)];
  float BumpSensitivity[COUNT(M914_suffix)];
  float MblOffset[COUNT(G29_suffix)];
} PARAMETERS;

PARAMETERS infoParametersBackup;
PARAMETERS infoParameters;
static uint32_t parametersEnabled = 0;

// param attributes multi purpose hard coded labels
char * const axisDisplayID[AXIS_INDEX_COUNT] = AXIS_DISPLAY_ID;
char * const stepperDisplayID[STEPPER_INDEX_COUNT] = STEPPER_DISPLAY_ID;

#define ONOFF_DISPLAY_ID "1=ON 0=OFF"

// param attributes hard coded labels
char * const filamentDiaDisplayID[] = {"S " ONOFF_DISPLAY_ID, "T0 Ø Filament", "T1 Ø Filament", "T2 Ø Filament"};
char * const autoRetractDisplayID[] = {"S " ONOFF_DISPLAY_ID};
char * const hotendPidDisplayID[] = {"Kp", "Ki", "Kd"};
char * const bedPidDisplayID[] = {"Kp", "Ki", "Kd"};
char * const ablStateDisplayID[] = {"S " ONOFF_DISPLAY_ID, "Z fade height"};
char * const stealthChopDisplayID[] = {"X " ONOFF_DISPLAY_ID, "X2 " ONOFF_DISPLAY_ID, "Y " ONOFF_DISPLAY_ID, "Y2 " ONOFF_DISPLAY_ID,
                                       "Z " ONOFF_DISPLAY_ID, "Z2 " ONOFF_DISPLAY_ID, "Z3 " ONOFF_DISPLAY_ID, "Z4 " ONOFF_DISPLAY_ID,
                                       "E0 "ONOFF_DISPLAY_ID, "E1 "ONOFF_DISPLAY_ID, "E2 "ONOFF_DISPLAY_ID};
char * const inputShapingDisplayID[] = {"X -> F:", "X -> D:", "Y -> F:", "Y -> D:"};
char * const deltaConfigurationDisplayID[] = {"Height", "Segment/sec.", "Radius", "Diagonal Rod"};
char * const deltaTowerAngleDisplayID[] = {"Tx", "Ty", "Tz"};
char * const deltaDiagonalRodDisplayID[] = {"Dx", "Dy", "Dz"};
char * const deltaEndstopDisplayID[] = {"Ex", "Ey", "Ez"};
char * const linAdvDisplayID[] = {"K-Factor E0", "K-Factor E1", "K-Factor E2"};

// param attributes configurable labels
const LABEL accelDisplayID[] = {LABEL_PRINT_ACCELERATION, LABEL_RETRACT_ACCELERATION, LABEL_TRAVEL_ACCELERATION};
const LABEL junctionDeviationDisplayID[] = {LABEL_JUNCTION_DEVIATION};
const LABEL retractDisplayID[] = {LABEL_RETRACT_LENGTH, LABEL_RETRACT_SWAP_LENGTH, LABEL_RETRACT_FEEDRATE, LABEL_RETRACT_Z_LIFT};
const LABEL recoverDisplayID[] = {LABEL_RECOVER_LENGTH, LABEL_SWAP_RECOVER_LENGTH, LABEL_RECOVER_FEEDRATE, LABEL_SWAP_RECOVER_FEEDRATE};

void infoParametersRefreshBackup(void)
{
  memcpy(&infoParametersBackup, &infoParameters, sizeof(PARAMETERS));
}

bool infoParametersHasChange(void)
{
  return memcmp(&infoParametersBackup, &infoParameters, sizeof(PARAMETERS)) ? true : false;
}

static inline void setElementStatus(PARAMETER_NAME name, uint8_t element, bool status)
{
  SET_BIT_VALUE(parameter_list[name].enabled, element, status);
}

static inline uint8_t getElementStatus(PARAMETER_NAME name, uint8_t element)
{
  return GET_BIT(parameter_list[name].enabled, element);
}

uint8_t getEnabledElementCount(PARAMETER_NAME name)
{
  uint8_t count = 0;
  parameter_member_t * p = &parameter_list[name];

  for (uint8_t i = 0; i < p->cnt; i++)
  {
    count += GET_BIT(p->enabled, i);
  }

  return count;
}

uint8_t getEnabledElement(PARAMETER_NAME name, uint8_t index)
{
  uint8_t count = 0;
  uint8_t state = 0;
  parameter_member_t * p = &parameter_list[name];

  for (uint8_t i = 0; i < p->cnt; i++)
  {
    state = GET_BIT(p->enabled, i);
    count += state;

    if (state && count == (index + 1))
      return i;
  }

  return p->cnt;
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

  for (uint8_t i = 0; i < PARAMETERS_COUNT; i++)
  {
    count += GET_BIT(parametersEnabled, i);
  }

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
  if (index >= parameter_list[name].cnt || !getParameterStatus(name) || !getElementStatus(name, index))
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

    case P_INPUT_SHAPING:
      return infoParameters.InputShaping[index];

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
  if (index >= parameter_list[name].cnt)
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

    case P_INPUT_SHAPING:
      infoParameters.InputShaping[index] = val;
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
  return parameter_list[para].cnt;
}

VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index)
{
  return parameter_list[para].suffix[index].valType;
}

void sendParameterCmd(PARAMETER_NAME name, uint8_t elementIndex, float Value)
{
  char tempCmd[30];
  snprintf(tempCmd, sizeof(tempCmd),"%s %s", parameter_list[name].prefix, parameter_list[name].suffix[elementIndex].suffix);
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
