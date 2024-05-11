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
  P_FILAMENT_DIAMETER,
  P_MAX_ACCELERATION,
  P_MAX_FEED_RATE,
  P_ACCELERATION,
  P_JERK,
  P_JUNCTION_DEVIATION,
  P_HOME_OFFSET,
  P_FWRETRACT,
  P_FWRECOVER,
  P_AUTO_RETRACT,
  P_HOTEND_OFFSET,
  P_HOTEND_PID,
  P_BED_PID,
  P_ABL_STATE,
  P_STEALTH_CHOP,
  P_INPUT_SHAPING,
  P_DELTA_CONFIGURATION,
  P_DELTA_TOWER_ANGLE,
  P_DELTA_DIAGONAL_ROD,
  P_DELTA_ENDSTOP,
  P_PROBE_OFFSET,
  P_LIN_ADV,
  P_CURRENT,
  P_HYBRID_THRESHOLD,
  P_BUMPSENSITIVITY,
  P_MBL_OFFSET,
  // keep below items always at the end
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
  AXIS_INDEX_E2,
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
  STEPPER_INDEX_Z3,
  STEPPER_INDEX_Z4,
  STEPPER_INDEX_E0,
  STEPPER_INDEX_E1,
  STEPPER_INDEX_E2,
  STEPPER_INDEX_COUNT
} STEPPER_INDEX;

typedef enum
{
  VAL_TYPE_INT,
  VAL_TYPE_NEG_INT,
  VAL_TYPE_FLOAT,
  VAL_TYPE_NEG_FLOAT,
} VAL_TYPE;

// param attributes multi purpose hard coded labels
extern char * const axisDisplayID[AXIS_INDEX_COUNT];
extern char * const stepperDisplayID[STEPPER_INDEX_COUNT];

// param attributes hard coded labels
extern char * const filamentDiaDisplayID[];
extern char * const autoRetractDisplayID[];
extern char * const hotendPidDisplayID[];
extern char * const bedPidDisplayID[];
extern char * const ablStateDisplayID[];
extern char * const stealthChopDisplayID[];
extern char * const inputShapingDisplayID[];
extern char * const deltaConfigurationDisplayID[];
extern char * const deltaTowerAngleDisplayID[];
extern char * const deltaDiagonalRodDisplayID[];
extern char * const deltaEndstopDisplayID[];
extern char * const linAdvDisplayID[];

// param attributes configurable labels
extern const LABEL accelDisplayID[];
extern const LABEL junctionDeviationDisplayID[];
extern const LABEL retractDisplayID[];
extern const LABEL recoverDisplayID[];

void infoParametersRefreshBackup(void);
bool infoParametersHasChange(void);

// get enable element count for the parameter
uint8_t getEnabledElementCount(PARAMETER_NAME name);

// get element index out of total enabled enabled element.
// If no element is enabled, total element count is returned
uint8_t getEnabledElement(PARAMETER_NAME name, uint8_t index);

// get total enabled parameters
uint8_t getEnabledParameterCount(void);

// get PARAMETER_NAME of selected index out of total enabled parameters.
// If no parameter is enabled, total parameter count is returned
PARAMETER_NAME getEnabledParameter(uint8_t index);

float getParameter(PARAMETER_NAME name, uint8_t index);
void setParameter(PARAMETER_NAME name, uint8_t index, float val);

// get total elements in a parameter
uint8_t getElementCount(PARAMETER_NAME para);

// get type of value a parameter element holds
VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index);

// send parameter cmd (parameter value gets updated after the cmd passes through the cmd cache)
void sendParameterCmd(PARAMETER_NAME para_index, uint8_t stepper_index, float Value);

// save parameter setting to eeprom
void saveEepromSettings(void);

// restore settings from eeprom
void restoreEepromSettings(void);

// reset settings and eeprom to default values
void resetEepromSettings(void);

#ifdef __cplusplus
}
#endif

#endif
