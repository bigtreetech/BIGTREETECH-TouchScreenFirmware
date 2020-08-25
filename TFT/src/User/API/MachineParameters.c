#include "MachineParameters.h"
#include "includes.h"

PARAMETERS infoParameters;

const u8 parameter_element_count[PARAMETERS_COUNT] = {5, 5, 5, 5, 3, 3, 3, 4, 4, 1, 2, 2, 3};

const char *const parameter_Cmd[PARAMETERS_COUNT][STEPPER_COUNT] = {
  {"M92 X%.2f\n",    "M92 Y%.2f\n",  "M92 Z%.2f\n",  "M92 T0 E%.2f\n",  "M92 T1 E%.2f\nM503 S0\n"}, //Steps/mm
  {"M906 X%.0f\n",  "M906 Y%.0f\n", "M906 Z%.0f\n", "M906 T0 E%.0f\n", "M906 T1 E%.0f\nM503 S0\n"}, //Current
  {"M203 X%.0f\n",  "M203 Y%.0f\n", "M203 Z%.0f\n", "M203 T0 E%.0f\n", "M203 T1 E%.0f\nM503 S0\n"}, //MaxFeedrate
  {"M201 X%.0f\n",  "M201 Y%.0f\n", "M201 Z%.0f\n", "M201 T0 E%.0f\n", "M201 T1 E%.0f\nM503 S0\n"}, //MaxAcceleration
  {"M204 P%.0f\n",  "M204 R%.0f\n", "M204 T%.0f\n",              NULL,                       NULL}, //Acceleration
  {"M851 X%.2f\n",  "M851 Y%.2f\n", "M851 Z%.2f\n",              NULL,                       NULL}, //Probe offset
  {"M914 X%.0f\n",  "M914 Y%.0f\n", "M914 Z%.0f\n",              NULL,                       NULL}, //bump Sensitivity
  {"M207 S%.2f\n",  "M207 W%.2f\n", "M207 F%.2f\n",    "M207 Z%.2f\n",                       NULL}, //FW retract
  {"M208 S%.2f\n",  "M208 W%.2f\n", "M208 F%.2f\n",    "M208 R%.2f\n",                       NULL}, //FW retract recover
  {"M209 S%.0f\nM503 S0\n", NULL,             NULL,               NULL,                      NULL}, //Set auto FW retract
  {"M900 T0 K%.2f\n", "M900 T1 K%.2f\nM503 S0\n", NULL,          NULL,                       NULL}, //Linear Advance
  {"M420 S%.0f\n", "M420 Z%.2f\n",            NULL,               NULL,                      NULL}, //ABL State + Z Fade
  {"M218 T1 X%.2f\nM503 S0\n", "M218 T1 Y%.2f\nM503 S0\n", "M218 T1 Z%.2f\nM503 S0\n", NULL, NULL}, //Offset Tools
};

const VAL_TYPE parameter_val_type[PARAMETERS_COUNT][STEPPER_COUNT] = {
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_FLOAT,       VAL_TYPE_FLOAT,       VAL_TYPE_FLOAT},  //Steps/mm
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT,         VAL_TYPE_INT},    //Current
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT,         VAL_TYPE_INT},    //MaxFeedrate
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT,         VAL_TYPE_INT},    //MaxAcceleration
  {VAL_TYPE_INT,        VAL_TYPE_INT,       VAL_TYPE_INT,         VAL_TYPE_INT},                          //Acceleration
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                          //Probe offset
  {VAL_TYPE_NEG_INT,    VAL_TYPE_NEG_INT,   VAL_TYPE_NEG_INT},                                            //bump Sensitivity
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_INT,         VAL_TYPE_FLOAT},                        //FW retract
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT,     VAL_TYPE_INT,         VAL_TYPE_INT},                          //FW retract recover
  {VAL_TYPE_INT},                                                                                         //Set auto FW retract
  {VAL_TYPE_FLOAT,      VAL_TYPE_FLOAT},                                                                  //Linear Advance                                                                            //Linear Advance
  {VAL_TYPE_INT,        VAL_TYPE_FLOAT},                                                                  //ABL State + Z Fade
  {VAL_TYPE_NEG_FLOAT,  VAL_TYPE_NEG_FLOAT, VAL_TYPE_NEG_FLOAT},                                          //Offset Tools                                                                                          //Set auto FW retract
};

//Extra teppers current gcode command
const char *const dualStepperParameter_cmd[2][AXIS_NUM] = {
  {"M906 I1 X%.0f\n", "M906 I1 Y%.0f\n", "M906 I1 Z%.0f\n"},  //Current
  {"M914 I1 X%.0f\n", "M914 I1 Y%.0f\n", "M914 I1 Z%.0f\n"}   //bump Sensitivity
};


bool dualstepper[TOTAL_AXIS] = {false,false,false,false};

char *const axisDisplayID[STEPPER_COUNT] = AXIS_DISPLAY_ID;

const LABEL accel_disp_ID[] = {LABEL_PRINT_ACCELERATION, LABEL_RETRACT_ACCELERATION, LABEL_TRAVEL_ACCELERATION};
const LABEL retract_disp_ID[] = {LABEL_RETRACT_LENGTH, LABEL_RETRACT_SWAP_LENGTH, LABEL_RETRACT_FEEDRATE, LABEL_RETRACT_Z_LIFT};
const LABEL recover_disp_ID[] = {LABEL_RECOVER_LENGTH, LABEL_SWAP_RECOVER_LENGTH, LABEL_RECOVER_FEEDRATE, LABEL_SWAP_RECOVER_FEEDRATE};
const LABEL retract_auto_ID[] = {LABEL_RETRACT_AUTO};


float getParameter(PARAMETER_NAME name, u8 index)
{
  if(index >= parameter_element_count[name]) return 0.0f;
  switch (name)
  {
  case P_STEPS_PER_MM:
    return infoParameters.StepsPerMM[index];
  case P_CURRENT:
    return infoParameters.Current[index];
  case P_MAX_FEED_RATE:
    return infoParameters.MaxFeedRate[index];
  case P_MAX_ACCELERATION:
    return infoParameters.MaxAcceleration[index];
  case P_ACCELERATION:
    return infoParameters.Acceleration[index];
  case P_PROBE_OFFSET:
    return infoParameters.ProbeOffset[index];
  case P_BUMPSENSITIVITY:
    return infoParameters.BumpSensitivity[index];
  case P_FWRETRACT:
    return infoParameters.FwRetract[index];
  case P_FWRECOVER:
    return infoParameters.FwRecover[index];
  case P_AUTO_RETRACT:
    return infoParameters.AutoRetract[index];
  case P_LIN_ADV:
    return infoParameters.LinAdvance[index];
  case P_ABL_STATE:
    return infoParameters.ABLState[index];
  case P_OFFSET_TOOL:
    return infoParameters.OffsetTool[index];
  default:
    return 0.0f;
  }
}

void setParameter(PARAMETER_NAME name, u8 index, float val)
{
  if(index >= parameter_element_count[name]) return;
    switch (name)
    {
    case P_STEPS_PER_MM:
      infoParameters.StepsPerMM[index] = val;
      break;
    case P_CURRENT:
      infoParameters.Current[index] = val;
      break;
    case P_MAX_FEED_RATE:
      infoParameters.MaxFeedRate[index]  = val;
      break;
    case P_MAX_ACCELERATION:
      infoParameters.MaxAcceleration[index] = val;
      break;
    case P_ACCELERATION:
      infoParameters.Acceleration[index] = val;
      break;
    case P_PROBE_OFFSET:
      infoParameters.ProbeOffset[index] = val;
      break;
    case P_BUMPSENSITIVITY:
      infoParameters.BumpSensitivity[index] = val;
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
    case P_LIN_ADV:
      infoParameters.LinAdvance[index] = val;
      break;
    case P_ABL_STATE:
      infoParameters.ABLState[index] = val;
      break;
    case P_OFFSET_TOOL:
      infoParameters.OffsetTool[index] = val;
      break;
    default:
      break;
    }
}

u8 getParameterElementCount(PARAMETER_NAME para)
{
  return parameter_element_count[para];
}

VAL_TYPE getParameterValType(PARAMETER_NAME para, u8 index)
{
  return parameter_val_type[para][index];
}

void setDualStepperStatus(u8 index, bool status)
{
  dualstepper[index] = status;
}

bool getDualstepperStatus(u8 index)
{
  return dualstepper[index];
}

void sendParameterCmd(PARAMETER_NAME para, u8 stepper_index, float Value)
{
  storeCmd(parameter_Cmd[para][stepper_index], Value);
  if (dualstepper[stepper_index] && stepper_index < AXIS_NUM)
    {
      if(para == P_CURRENT)
        storeCmd(dualStepperParameter_cmd[0][stepper_index], Value);
      if(para == P_BUMPSENSITIVITY)
        storeCmd(dualStepperParameter_cmd[1][stepper_index], Value);
    }
}

void saveEepromSettings(void)
{
  if(infoMachineSettings.EEPROM == 1)
    mustStoreCmd("M500\n");
}
void restoreEepromSettings(void)
{
  if(infoMachineSettings.EEPROM == 1)
    mustStoreScript("M501\nM503 S0\n");
}
void resetEepromSettings(void)
{
  if(infoMachineSettings.EEPROM == 1)
    mustStoreScript("M502\nM500\nM503 S0\n");
}
