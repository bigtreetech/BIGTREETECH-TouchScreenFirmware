#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_
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
// Keep below items always at the end
P_RESET_SETTINGS,
P_RESTORE_SETTINGS,
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
}PARAMETERS;

extern PARAMETERS infoParameters;
extern bool dualstepper[TOTAL_AXIS];

float getParameter(PARAMETER_NAME name, int index);
void setParameter(PARAMETER_NAME name, int index, float val);

void menuParameterSettings(void);

// Global Info

#define ICON_NOZZLE_X   2*ICON_WIDTH+ICON_WIDTH/2
#define VALUE_NOZZLE_X  ICON_NOZZLE_X+BYTE_WIDTH

#define ICON_BED_X      3*ICON_WIDTH+2*BYTE_WIDTH+ICON_WIDTH/2
#define VALUE_BED_X     ICON_BED_X+BYTE_WIDTH

void show_GlobalInfo(void);
void temp_Change(void);
void drawGlobalInfo(void);

#endif
