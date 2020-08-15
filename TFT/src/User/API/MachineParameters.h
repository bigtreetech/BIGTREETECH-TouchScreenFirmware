#ifndef _MACHINEPARAMETERS_H_
#define _MACHINEPARAMETERS_H_
#include "menu.h"
#include "coordinate.h"

typedef enum
{
P_STEPS_PER_MM = 0,
P_CURRENT,
P_MAX_FEED_RATE,
P_MAX_ACCELERATION,
P_ACCELERATION,
P_PROBE_OFFSET,
P_BUMPSENSITIVITY,
P_FWRETRACT,
P_FWRECOVER,
P_LIN_ADV,
P_ABL_STATE,
P_AUTO_RETRACT,
// Keep below items always at the end
P_SAVE_SETTINGS,
P_RESTORE_SETTINGS,
P_RESET_SETTINGS,
P_ITEMSCOUNT
}PARAMETER_NAME;

#define PARAMETERS_COUNT P_RESET_SETTINGS

typedef enum
{
X_STEPPER = 0,
Y_STEPPER,
Z_STEPPER,
E_STEPPER,
E2_STEPPER,
STEPPER_COUNT
}STEPPERS;

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
float Current[STEPPER_COUNT];
float MaxFeedRate[STEPPER_COUNT];
float MaxAcceleration[STEPPER_COUNT];
float Acceleration[3];
float ProbeOffset[3];
float BumpSensitivity[3];
float FwRetract[4];
float FwRecover[4];
float LinAdvance[1];
float ABLState[2];
float AutoRetract[1];
}PARAMETERS;

extern PARAMETERS infoParameters;

extern char *const axisDisplayID[STEPPER_COUNT];
extern const LABEL accel_disp_ID[];
extern const LABEL retract_disp_ID[];
extern const LABEL recover_disp_ID[];
extern const LABEL retract_auto_ID[];

float getParameter(PARAMETER_NAME name, uint8_t index);
void setParameter(PARAMETER_NAME name, uint8_t index, float val);

// get total elements in a parameter
uint8_t getParameterElementCount(PARAMETER_NAME para);

//Get type of value a parameter element holds
VAL_TYPE getParameterValType(PARAMETER_NAME para, u8 index);

//set status of dual stepper for an axis
void setDualStepperStatus(u8 index, bool status);

//get status of dual stepper for an axis
bool getDualstepperStatus(u8 index);

//send parameter cmd (Parameter value gets updated after the cmd passes through the cmd cache)
void sendParameterCmd(PARAMETER_NAME para_index, u8 stepper_index, float Value);

//Save parameter setting to eeprom
void saveEepromSettings(void);

//restore settings from eeprom
void restoreEepromSettings(void);

//reset settings and eeprom to default values
void resetEepromSettings(void);

#endif
