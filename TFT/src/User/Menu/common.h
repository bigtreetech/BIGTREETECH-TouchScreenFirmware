#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "menu.h"
#include "Heat.h"

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

extern const ITEM itemTool[MAX_HEATER_COUNT];
extern const ITEM itemDegreeSteps[ITEM_DEGREE_NUM];
extern const uint8_t degreeSteps[ITEM_DEGREE_NUM];

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

#ifdef __cplusplus
}
#endif

#endif
