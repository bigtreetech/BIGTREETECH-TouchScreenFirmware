//TG MODIFIED BY T.GIOIOSA
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "menu.h"
//#include "Heat.h"     //TG 8/22/21 removed for CNC
#include "Settings.h"   //TG 8/22/21 added when removing Heat.h for CNC
#define ITEM_RPM_NUM            4   //TG 2/5/21
#define ITEM_PWM_NUM            3   //TG 2/14/21
#define ITEM_DEGREE_NUM         3
#define ITEM_SPEED_NUM          3
#define ITEM_PERCENT_STEPS_NUM  3
#define ITEM_MOVE_LEN_NUM       5
#define ITEM_FINE_MOVE_LEN_NUM  3
#define ITEM_EXT_LEN_NUM        5
#define ITEM_TOGGLE_NUM         2

extern uint8_t currentTool;
extern uint8_t currentFan;
extern uint8_t currentSpeedID;
uint8_t currentSpindleSpeedID; //TG 2/24/21 new

extern const ITEM itemTool[MAX_TOOL_COUNT];
extern const ITEM itemSpindle[MAX_TOOL_COUNT];
extern const ITEM itemLaser[MAX_TOOL_COUNT];
extern const ITEM itemVacuum[MAX_TOOL_COUNT];
extern const ITEM itemDegreeSteps[ITEM_DEGREE_NUM]; //TG 1/16/20 remove after completed dev, replaced by RPM 
extern const u8 degreeSteps[ITEM_DEGREE_NUM];

extern const ITEM itemRPMSteps[ITEM_RPM_NUM];       //TG 1/16/20 new for RPM
extern const u16 RPMSteps[ITEM_RPM_NUM];

const ITEM itemPWMSteps[ITEM_PWM_NUM]; 
const uint8_t PWMSteps[ITEM_PWM_NUM];


extern const ITEM itemSpeed[ITEM_SPEED_NUM];

extern const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM];
extern const uint8_t percentSteps[ITEM_PERCENT_STEPS_NUM];

extern const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM];
extern const float moveLenSteps[ITEM_MOVE_LEN_NUM];

extern const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM];
extern const float extlenSteps[ITEM_EXT_LEN_NUM];

extern const LABEL itemToggle[ITEM_TOGGLE_NUM];
extern const uint16_t iconToggle[ITEM_TOGGLE_NUM];

bool nextScreenUpdate(uint32_t duration);

extern const bool warmupTemperature(uint8_t toolIndex, void (* callback)(void));

extern const void cooldownTemperature(void);

// Show/draw temperature in a standard menu
extern const void temperatureReDraw(uint8_t toolIndex, int16_t * temp, bool skipHeader);

// Show/draw fan in a standard menu
extern const void fanReDraw(uint8_t fanIndex, bool skipHeader);

// Show/draw extruder in a standard menu
extern const void extruderReDraw(uint8_t extruderIndex, float extrusion, bool skipHeader);

// Show/draw percentage in a standard menu
extern const void percentageReDraw(uint8_t itemIndex, bool skipHeader);

// Edit temperature in a standard menu
extern const int16_t editIntValue(int16_t minValue, int16_t maxValue, int16_t resetValue, int16_t value);

// Edit a float value in a standard menu
extern const float editFloatValue(float minValue, float maxValue, float resetValue, float value);

#ifdef __cplusplus
}
#endif

#endif
