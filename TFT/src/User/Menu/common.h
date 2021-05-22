#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Settings.h"
#include "menu.h"

#define ITEM_DEGREE_NUM         3
#define ITEM_SPEED_NUM          3
#define ITEM_PERCENT_STEPS_NUM  3
#define ITEM_MOVE_LEN_NUM       5
#define ITEM_FINE_MOVE_LEN_NUM  3
#define ITEM_EXT_LEN_NUM        5
#define ITEM_TOGGLE_NUM         2

typedef enum
{
  VALUE_NONE = 0,
  VALUE_BYTE,
  VALUE_INT,
  VALUE_FLOAT,
  VALUE_STRING,
} VALUE_TYPE;

extern SCROLL scrollLine;

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

// Check if next screen update is due
bool nextScreenUpdate(uint32_t duration);

#ifdef FRIENDLY_Z_OFFSET_LANGUAGE
  void invertZAxisIcons(MENUITEMS * menuItems);

  #define  INVERT_Z_AXIS_ICONS(menuItemsPtr) invertZAxisIcons(menuItemsPtr)
#else
  #define  INVERT_Z_AXIS_ICONS(menuItemsPtr)
#endif

void drawBorder(const GUI_RECT *rect, uint16_t color, uint16_t edgeDistance);

void drawBackground(const GUI_RECT *rect, uint16_t bgColor, uint16_t edgeDistance);

void drawStandardValue(const GUI_RECT *rect, VALUE_TYPE valType, const void *val, uint16_t font,
                       uint16_t color, uint16_t bgColor, uint16_t edgeDistance, bool clearBgColor);

bool warmupTemperature(uint8_t toolIndex, void (* callback)(void));

void cooldownTemperature(void);

// Show/draw temperature in a standard menu
void temperatureReDraw(uint8_t toolIndex, int16_t * temp, bool skipHeader);

// Show/draw fan in a standard menu
void fanReDraw(uint8_t fanIndex, bool skipHeader);

// Show/draw extruder in a standard menu
void extruderReDraw(uint8_t extruderIndex, float extrusion, bool skipHeader);

// Show/draw percentage in a standard menu
void percentageReDraw(uint8_t itemIndex, bool skipHeader);

// Edit temperature in a standard menu
int32_t editIntValue(int32_t minValue, int32_t maxValue, int32_t resetValue, int32_t value);

// Edit a float value in a standard menu
float editFloatValue(float minValue, float maxValue, float resetValue, float value);

#ifdef __cplusplus
}
#endif

#endif
