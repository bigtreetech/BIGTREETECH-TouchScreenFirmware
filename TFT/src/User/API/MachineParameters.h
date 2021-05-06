#ifndef _MACHINE_PARAMETERS_H_
#define _MACHINE_PARAMETERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "menu.h"

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
  P_FILAMENT_DIAMETER,
  P_CURRENT,
  P_BUMPSENSITIVITY,
  P_HYBRID_THRESHOLD,
  P_STEALTH_CHOP,
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
  AXIS_INDEX_X = 0,
  AXIS_INDEX_Y,
  AXIS_INDEX_Z,
  AXIS_INDEX_E0,
  AXIS_INDEX_E1,
  AXIS_INDEX_COUNT
} AXIS_INDEX;

typedef enum
{
  STEPPER_INDEX_X = 0,
  STEPPER_INDEX_X2,
  STEPPER_INDEX_Y,
  STEPPER_INDEX_Y2,
  STEPPER_INDEX_Z,
  STEPPER_INDEX_Z2,
  STEPPER_INDEX_E0,
  STEPPER_INDEX_E1,
  STEPPER_INDEX_COUNT
} STEPPER_INDEX;

typedef enum
{
  VAL_TYPE_INT,
  VAL_TYPE_NEG_INT,
  VAL_TYPE_FLOAT,
  VAL_TYPE_NEG_FLOAT,
} VAL_TYPE;

typedef struct
{
  float StepsPerMM[AXIS_INDEX_COUNT];
  float MaxFeedRate[AXIS_INDEX_COUNT];
  float MaxAcceleration[AXIS_INDEX_COUNT];
  float Acceleration[3];
  float Jerk[AXIS_INDEX_COUNT - 1];
  float JunctionDeviation[1];
  float HomeOffset[AXIS_INDEX_COUNT - 2];
  float FwRetract[4];
  float FwRecover[4];
  float AutoRetract[1];
  float HotendOffset[AXIS_INDEX_COUNT - 2];
  float ABLState[2];
  float ProbeOffset[AXIS_INDEX_COUNT - 2];
  float LinAdvance[2];
  float FilamentSetting[3];
  float Current[STEPPER_INDEX_COUNT];
  float BumpSensitivity[3];
  float HybridThreshold[STEPPER_INDEX_COUNT];
  float StealthChop[STEPPER_INDEX_COUNT];
  float MblOffset[1];
} PARAMETERS;

extern PARAMETERS infoParameters;

extern char * const axisDisplayID[AXIS_INDEX_COUNT];
extern char * const stepperDisplayID[STEPPER_INDEX_COUNT];
extern char * const ablStateDisplayID[];
extern char * const linAdvDisplayID[];
extern char * const filamentDiaDisplayID[];
extern char * const stealthChopDisplayID[];

extern const LABEL accelDisplayID[];
extern const LABEL junctionDeviationDisplayID[];
extern const LABEL retractDisplayID[];
extern const LABEL recoverDisplayID[];
extern const LABEL autoRetractDisplayID[];

// Get enable element count for the parameter
uint8_t getEnabledElementCount(PARAMETER_NAME name);

// Get element index out of total enabled enabled element
// If no element is enabled, total element count is returned
uint8_t getEnabledElement(PARAMETER_NAME name, uint8_t index);

// Get total enabled parameters
uint8_t getEnabledParameterCount(void);

// Get PARAMETER_NAME of selected index out of total enabled parameters
// If no parameter is enabled, total parameter count is returned
PARAMETER_NAME getEnabledParameter(uint8_t index);

float getParameter(PARAMETER_NAME name, uint8_t index);
void setParameter(PARAMETER_NAME name, uint8_t index, float val);

// Get total elements in a parameter
uint8_t getElementCount(PARAMETER_NAME para);

// Get type of value a parameter element holds
VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index);

// Send parameter cmd (Parameter value gets updated after the cmd passes through the cmd cache)
void sendParameterCmd(PARAMETER_NAME para_index, uint8_t stepper_index, float Value);

// Save parameter setting to eeprom
void saveEepromSettings(void);

// Restore settings from eeprom
void restoreEepromSettings(void);

// Reset settings and eeprom to default values
void resetEepromSettings(void);

#ifdef __cplusplus
}
#endif

#endif
