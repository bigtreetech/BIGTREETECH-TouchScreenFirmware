#ifndef _MACHINEPARAMETERS_H_
#define _MACHINEPARAMETERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"
#include "coordinate.h"

typedef enum
{
  P_STEPS_PER_MM = 0,
  P_MAX_FEED_RATE,
  P_MAX_ACCELERATION,
  P_ACCELERATION,
  P_JERK,
  P_JUNCTION_DEVIATION,
  P_HOME_OFFSET,
  P_FWRETRACT,
  P_FWRECOVER,
  P_AUTO_RETRACT,
  P_HOTEND_OFFSET,
  P_ABL_STATE,
  P_PROBE_OFFSET,
  P_LIN_ADV,
  P_CURRENT,
  P_BUMPSENSITIVITY,
  P_HYBRID_THRESHOLD,
  P_MBL_OFFSET,
  // Keep below items always at the end
  PARAMETERS_COUNT,
} PARAMETER_NAME;

typedef enum
{
  P_SAVE_SETTINGS = 0,
  P_RESTORE_SETTINGS,
  P_RESET_SETTINGS,
  P_SETTINGS_COUNT,
} PARAMETER_SETTINGS;

typedef enum
{
  X_STEPPER = 0,
  Y_STEPPER,
  Z_STEPPER,
  E_STEPPER,
  E2_STEPPER,
  STEPPER_COUNT
} STEPPERS;

typedef enum
{
  VAL_TYPE_INT,
  VAL_TYPE_NEG_INT,
  VAL_TYPE_FLOAT,
  VAL_TYPE_NEG_FLOAT,
} VAL_TYPE;

typedef struct
{
  float StepsPerMM[STEPPER_COUNT];
  float MaxFeedRate[STEPPER_COUNT];
  float MaxAcceleration[STEPPER_COUNT];
  float Acceleration[3];
  float Jerk[4];
  float JunctionDeviation[1];
  float HomeOffset[3];
  float FwRetract[4];
  float FwRecover[4];
  float AutoRetract[1];
  float HotendOffset[3];
  float ABLState[2];
  float ProbeOffset[3];
  float LinAdvance[2];
  float Current[STEPPER_COUNT];
  float BumpSensitivity[3];
  float HybridThreshold[STEPPER_COUNT];
  float MblOffset[1];
} PARAMETERS;

extern PARAMETERS infoParameters;

extern char *const axisDisplayID[STEPPER_COUNT];
extern const LABEL accel_disp_ID[];
extern const LABEL junction_deviation_disp_ID[];
extern const LABEL retract_disp_ID[];
extern const LABEL recover_disp_ID[];
extern const LABEL retract_auto_ID[];

// Set a parameter status to enable or disable.
void setParameterStatus(PARAMETER_NAME name, bool status);

// Check status of a parameter
uint8_t getParameterStatus(PARAMETER_NAME name);

// Get total enabled parameters
uint8_t getEnabledParameterCount(void);

// Get PARAMETER_NAME of selected index out of total enabled parameters
// If no parameter is enabled, total parameter count is returned
PARAMETER_NAME getEnabledParameter(uint8_t index);

float getParameter(PARAMETER_NAME name, uint8_t index);
void setParameter(PARAMETER_NAME name, uint8_t index, float val);

// get total elements in a parameter
uint8_t getParameterElementCount(PARAMETER_NAME para);

//Get type of value a parameter element holds
VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index);

//set status of dual stepper for an axis
void setDualStepperStatus(uint8_t index, bool status);

//get status of dual stepper for an axis
bool getDualStepperStatus(uint8_t index);

//send parameter cmd (Parameter value gets updated after the cmd passes through the cmd cache)
void sendParameterCmd(PARAMETER_NAME para_index, uint8_t stepper_index, float Value);

//Save parameter setting to eeprom
void saveEepromSettings(void);

//restore settings from eeprom
void restoreEepromSettings(void);

//reset settings and eeprom to default values
void resetEepromSettings(void);

#ifdef __cplusplus
}
#endif

#endif
