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
#define ITEM_EXT_LEN_NUM        5
#define ITEM_MOVE_LEN_NUM       5


extern const ITEM itemTool[MAX_HEATER_COUNT];
extern const ITEM itemDegreeSteps[ITEM_DEGREE_NUM];
extern const u8 degreeSteps[ITEM_DEGREE_NUM];

extern const ITEM itemSpeed[ITEM_SPEED_NUM];

extern const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM];
extern const  u8 percentSteps[ITEM_PERCENT_STEPS_NUM];

extern const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM];
extern const  float moveLenSteps[ITEM_MOVE_LEN_NUM];

extern const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM];
extern const float extlenSteps[ITEM_EXT_LEN_NUM];

#ifdef __cplusplus
}
#endif

#endif
