#include "MachineParameters.h"
#include "includes.h"

const uint8_t parameter_element_count[PARAMETERS_COUNT] = {5, 5, 5, 3, 4, 1, 3, 4, 4, 1, 3, 2, 3, 2, 5, 3, 5, 1};

const char * const parameter_Cmd[PARAMETERS_COUNT][STEPPER_COUNT] = {
    {"M92 X%.2f\n",     "M92 Y%.2f\n",      "M92 Z%.2f\n",      "M92 T0 E%.2f\n",   "M92 T1 E%.2f\n"},     //Steps/mm
    {"M203 X%.0f\n",    "M203 Y%.0f\n",     "M203 Z%.0f\n",     "M203 T0 E%.0f\n",  "M203 T1 E%.0f\n"},   //MaxFeedrate
    {"M201 X%.0f\n",    "M201 Y%.0f\n",     "M201 Z%.0f\n",     "M201 T0 E%.0f\n",  "M201 T1 E%.0f\n"},   //MaxAcceleration
    {"M204 P%.0f\n",    "M204 R%.0f\n",     "M204 T%.0f\n",     NULL,               NULL},                //Acceleration
    {"M205 X%.0f\n",    "M205 Y%.0f\n",     "M205 Z%.2f\n",     "M205 E%.2f\n",     NULL},                //Jerk
    {"M205 J%.3f\n",    NULL,               NULL,               NULL,               NULL},                //Junction Deviation
    {"M206 X%.2f\n",    "M206 Y%.2f\n",     "M206 Z%.2f\n",     NULL,               NULL},                //Home offset
    {"M207 S%.2f\n",    "M207 W%.2f\n",     "M207 F%.2f\n",     "M207 Z%.2f\n",     NULL},                //FW retract
    {"M208 S%.2f\n",    "M208 W%.2f\n",     "M208 F%.2f\n",     "M208 R%.2f\n",     NULL},                //FW retract recover
    {"M209 S%.0f\n",    NULL,               NULL,               NULL,               NULL},                //Set auto FW retract
    {"M218 T1 X%.2f",   "M218 T1 Y%.2f\n",  "M218 T1 Z%.2f\n",  NULL,               NULL},                //Hotend Offset
    {"M420 S%.0f\n",    "M420 Z%.2f\n",     NULL,               NULL,               NULL},                //ABL State + Z Fade
    {"M851 X%.2f\n",    "M851 Y%.2f\n",     "M851 Z%.2f\n",     NULL,               NULL},                //Probe offset
    {"M900 T0 K%.2f\n", "M900 T1 K%.2f\n",  NULL,               NULL,               NULL},                //Linear Advance
    {"M906 X%.0f\n",    "M906 Y%.0f\n",     "M906 Z%.0f\n",     "M906 T0 E%.0f\n",  "M906 T1 E%.0f\n"},   //Current
    {"M914 X%.0f\n",    "M914 Y%.0f\n",     "M914 Z%.0f\n",     NULL,               NULL},                //bump Sensitivity
    {"M913 X%.0f\n",    "M913 Y%.0f\n",     "M913 Z%.0f\n",     "M913 T0 E%.0f\n",  "M913 T1 E%.0f\n"},   //TMC Hybrid Threshold Speed
    {"G29 S4 Z%.2f\n",  NULL,               NULL,               NULL,               NULL},                //MBL offset
};

const VAL_TYPE parameter_val_type[PARAMETERS_COUNT][STEPPER_COUNT] = {
  {VAL_TYPE_FLOAT,        VAL_TYPE_FLOAT,       VAL_TYPE_FLOAT,        VAL_TYPE_FLOAT,    VAL_TYPE_FLOAT},  //Steps/mm
  {VAL_TYPE_INT,          VAL_TYPE_INT,         VAL_TYPE_INT,          VAL_TYPE_INT,      VAL_TYPE_INT},    //MaxFeedrate
  {VAL_TYPE_INT,          VAL_TYPE_INT,         VAL_TYPE_INT,          VAL_TYPE_INT,      VAL_TYPE_INT},    //MaxAcceleration
  {VAL_TYPE_INT,          VAL_TYPE_INT,         VAL_TYPE_INT},                                              //Acceleration
  {VAL_TYPE_INT,          VAL_TYPE_INT,         VAL_TYPE_FLOAT,        VAL_TYPE_FLOAT},                     //Jerk
  {VAL_TYPE_FLOAT},                                                                                         //Junction Deviation
  {VAL_TYPE_NEG_FLOAT,    VAL_TYPE_NEG_FLOAT,   VAL_TYPE_NEG_FLOAT},                                        //Home offset
  {VAL_TYPE_FLOAT,        VAL_TYPE_FLOAT,       VAL_TYPE_INT,          VAL_TYPE_FLOAT},                     //FW retract
  {VAL_TYPE_FLOAT,        VAL_TYPE_FLOAT,       VAL_TYPE_INT,          VAL_TYPE_INT},                       //FW retract recover
  {VAL_TYPE_INT},                                                                                           //Set auto FW retract
  {VAL_TYPE_NEG_FLOAT,    VAL_TYPE_NEG_FLOAT,   VAL_TYPE_NEG_FLOAT},                                        //Hotend Offset
  {VAL_TYPE_INT,          VAL_TYPE_FLOAT},                                                                  //ABL State + Z Fade
  {VAL_TYPE_NEG_FLOAT,    VAL_TYPE_NEG_FLOAT,   VAL_TYPE_NEG_FLOAT},                                        //Probe offset
  {VAL_TYPE_FLOAT,        VAL_TYPE_FLOAT},                                                                  //Linear Advance
  {VAL_TYPE_INT,          VAL_TYPE_INT,         VAL_TYPE_INT,          VAL_TYPE_INT,      VAL_TYPE_INT},    //Current
  {VAL_TYPE_NEG_INT,      VAL_TYPE_NEG_INT,     VAL_TYPE_NEG_INT},                                          //bump Sensitivity
  {VAL_TYPE_INT,          VAL_TYPE_INT,         VAL_TYPE_INT,          VAL_TYPE_INT,      VAL_TYPE_INT},    //TMC Hybrid Threshold Speed
  {VAL_TYPE_NEG_FLOAT},                                                                                     //MBL offset
};

//Extra steppers current gcode command
const char *const dualStepperParameter_cmd[3][AXIS_NUM] = {
  {"M906 I1 X%.0f\n", "M906 I1 Y%.0f\n", "M906 I1 Z%.0f\n"},  //Current
  {"M914 I1 X%.0f\n", "M914 I1 Y%.0f\n", "M914 I1 Z%.0f\n"},  //bump Sensitivity
  {"M913 I1 X%.0f\n", "M913 I1 Y%.0f\n", "M913 I1 Z%.0f\n"},  //TMC Hybrid Threshold Speed
};

PARAMETERS infoParameters;
uint32_t parametersEnabled = 0;
uint8_t dualsteppers = 0;

char * const axisDisplayID[STEPPER_COUNT] = AXIS_DISPLAY_ID;

const LABEL accel_disp_ID[] = {LABEL_PRINT_ACCELERATION, LABEL_RETRACT_ACCELERATION, LABEL_TRAVEL_ACCELERATION};
const LABEL retract_disp_ID[] = {LABEL_RETRACT_LENGTH, LABEL_RETRACT_SWAP_LENGTH, LABEL_RETRACT_FEEDRATE, LABEL_RETRACT_Z_LIFT};
const LABEL recover_disp_ID[] = {LABEL_RECOVER_LENGTH, LABEL_SWAP_RECOVER_LENGTH, LABEL_RECOVER_FEEDRATE, LABEL_SWAP_RECOVER_FEEDRATE};
const LABEL retract_auto_ID[] = {LABEL_RETRACT_AUTO};
const LABEL junction_deviation_disp_ID[] = {LABEL_JUNCTION_DEVIATION};

void setParameterStatus(PARAMETER_NAME name, bool status)
{
  parametersEnabled = (status == 1) ? (1 << name) | parametersEnabled : parametersEnabled & (~(1 << name));
}

uint8_t getParameterStatus(PARAMETER_NAME name)
{
  return (parametersEnabled >> name)  & 1;
}

uint8_t getEnabledParameterCount(void)
{
  uint8_t count = 0;
  for(uint8_t i = 0; i < PARAMETERS_COUNT; i++)
  {
    count += (parametersEnabled >> i)  & 1;
  }
  return count;
}

PARAMETER_NAME getEnabledParameter(uint8_t index)
{
  uint8_t count = 0;
  uint8_t state = 0;

  for(uint8_t i = 0; i < PARAMETERS_COUNT; i++)
  {
    state = (parametersEnabled >> i)  & 1;
    count += state;
    if (state && count == (index + 1))
     return i;
  }
  return PARAMETERS_COUNT;
}



float getParameter(PARAMETER_NAME name, uint8_t index)
{
  if (index >= parameter_element_count[name] || !getParameterStatus(name))
    return 0.0f;

  switch (name)
  {
    case P_STEPS_PER_MM:
      return infoParameters.StepsPerMM[index];

    case P_MAX_FEED_RATE:
      return infoParameters.MaxFeedRate[index];

    case P_MAX_ACCELERATION:
      return infoParameters.MaxAcceleration[index];

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

    case P_ABL_STATE:
      return infoParameters.ABLState[index];

    case P_PROBE_OFFSET:
      return infoParameters.ProbeOffset[index];

    case P_LIN_ADV:
      return infoParameters.LinAdvance[index];

    case P_CURRENT:
      return infoParameters.Current[index];

    case P_BUMPSENSITIVITY:
      return infoParameters.BumpSensitivity[index];

    case P_HYBRID_THRESHOLD:
      return infoParameters.HybridThreshold[index];

    case P_MBL_OFFSET:
      return infoParameters.MblOffset[index];

    default:
      return 0.0f;
  }
}

void setParameter(PARAMETER_NAME name, uint8_t index, float val)
{
  if (index >= parameter_element_count[name])
    return;

  setParameterStatus(name, true);

  switch (name)
  {
    case P_STEPS_PER_MM:
      infoParameters.StepsPerMM[index] = val;
      break;

    case P_MAX_FEED_RATE:
      infoParameters.MaxFeedRate[index] = val;
      break;

    case P_MAX_ACCELERATION:
      infoParameters.MaxAcceleration[index] = val;
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

    case P_ABL_STATE:
      infoParameters.ABLState[index] = val;
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

    case P_BUMPSENSITIVITY:
      infoParameters.BumpSensitivity[index] = val;
      break;

    case P_HYBRID_THRESHOLD:
      infoParameters.HybridThreshold[index] = val;
      break;

    case P_MBL_OFFSET:
      infoParameters.MblOffset[index] = val;
      break;

    default:
      break;
  }
}

uint8_t getParameterElementCount(PARAMETER_NAME para)
{
  return parameter_element_count[para];
}

VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index)
{
  return parameter_val_type[para][index];
}

void setDualStepperStatus(uint8_t index, bool status)
{
  dualsteppers = (status == 0) ? (1 << index) | parametersEnabled : parametersEnabled & (~(1 << index));
}

bool getDualStepperStatus(uint8_t index)
{
  return (dualsteppers >> index) & 1;
}

void sendParameterCmd(PARAMETER_NAME para, uint8_t stepper_index, float Value)
{
  storeCmd(parameter_Cmd[para][stepper_index], Value);

  if (getDualStepperStatus(stepper_index) && stepper_index < AXIS_NUM)
  {
    if (para == P_CURRENT)
      storeCmd(dualStepperParameter_cmd[0][stepper_index], Value);

    if (para == P_BUMPSENSITIVITY)
      storeCmd(dualStepperParameter_cmd[1][stepper_index], Value);

    if (para == P_HYBRID_THRESHOLD)
      storeCmd(dualStepperParameter_cmd[2][stepper_index], Value);
  }
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
