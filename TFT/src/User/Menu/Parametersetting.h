#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_
#include "menu.h"
#include "coordinate.h"

#define SKEYHEIGHT      (LCD_HEIGHT-ICON_START_Y)/4
#define SKEYWIDTH       LCD_WIDTH/4

#define ICON_NOZZLE_X   2*ICON_WIDTH+ICON_WIDTH/2
#define VALUE_NOZZLE_X  ICON_NOZZLE_X+BYTE_WIDTH

#define ICON_BED_X      3*ICON_WIDTH+2*BYTE_WIDTH+ICON_WIDTH/2
#define VALUE_BED_X     ICON_BED_X+BYTE_WIDTH

#define KEY_NUM 16
#define BUFLONG 6
typedef enum
{
  SKEY_0 = 0,
  SKEY_1,
  SKEY_2,
  SKEY_3,
  SKEY_4,
  SKEY_5,
  SKEY_6,
  SKEY_7,
  SKEY_8,
  SKEY_9,
  SKEY_10,
  SKEY_11,
  SKEY_IDLE = IDLE_TOUCH,
}SKEY_VALUES;

typedef enum
{
  NUM_KEY_1 = 0,
  NUM_KEY_2,
  NUM_KEY_3,
  NUM_KEY_OK,
  NUM_KEY_4,
  NUM_KEY_5,
  NUM_KEY_6,
  NUM_KEY_DEL,
  NUM_KEY_7,
  NUM_KEY_8,
  NUM_KEY_9,
  NUM_KEY_EXIT,
  NUM_KEY_DEC,
  NUM_KEY_0,
  NUM_KEY_MINUS,
  NUM_KEY_15,

  NUM_KEY_IDLE = IDLE_TOUCH,
}NUM_KEY_VALUES;

typedef enum
{
P_STEPS_PER_MM = 0,
P_CURRENT,
P_MAX_FEED_RATE,
P_MAX_ACCELERATION,
P_ACCELERATION,
P_PROBE_OFFSET,
P_BUMPSENSITIVITY,
PARAMETERS_COUNT
}PARAMETER_NAME;

typedef enum
{
X_STEPPER = 0,
Y_STEPPER,
Z_STEPPER,
E_STEPPER,
E2_STEPPER,
STEPPER_COUNT
}STEPPERS;

typedef struct
{
float StepsPerMM[STEPPER_COUNT];
float Current[STEPPER_COUNT];
float MaxFeedRate[STEPPER_COUNT];
float MaxAcceleration[STEPPER_COUNT];
float Acceleration[3];
float ProbeOffset[3];
float BumpSensitivity[3];
}PARAMETERS;

#define PS_PAGE_COUNT  (PARAMETERS_COUNT+LISTITEM_PER_PAGE-1)/LISTITEM_PER_PAGE

extern PARAMETERS infoParameters;
extern bool dualstepper[TOTAL_AXIS];

float getParameter(PARAMETER_NAME name, int index);
void setParameter(PARAMETER_NAME name, int index, float val);

void menuParameterSettings(void);
float num_pad(float old_val, bool negative_val);

void show_GlobalInfo(void);
void temp_Change(void);
void drawGlobalInfo(void);
#endif
