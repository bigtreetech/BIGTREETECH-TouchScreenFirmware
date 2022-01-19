//TG MODIFIED BY T.GIOIOSA

#ifndef _SPINDLECONTROL_H_
#define _SPINDLECONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "includes.h"

#define SPINDLE_TYPE_F        0  // Default cooling fan
#define SPINDLE_TYPE_CTRL_S   1  // Controller FAN on Stepper/Bed On  (Check - Marlin M710)
#define SPINDLE_TYPE_CTRL_I   2  // Controller FAN on Idle            (Check - Marlin M710)
#define SPINDLE_TYPE_UNKNOWN  8  // Unknown / Not defined!

extern char* spindleID[MAX_SPINDLE_COUNT];
extern char* spindleDisplayID[MAX_SPINDLE_COUNT];
extern char* spindleCmd[MAX_SPINDLE_COUNT];

extern uint8_t spindleType[MAX_SPINDLE_COUNT];
uint16_t lastSetSpindleSpeed[MAX_SPINDLE_COUNT];

typedef enum  {
  MRPM = 0,
  MPCT,
  MPWM,
  CUTTER_PWR_SIZE
} MTYPE;

uint16_t convertSpeed_Marlin_2_LCD(uint8_t s_index, uint16_t input);
uint16_t convertSpeed_LCD_2_Marlin(uint8_t s_index, uint16_t input);
uint16_t getSpeedUnits_Marlin_2_LCD(MTYPE marlin_units, MTYPE lcd_units, uint8_t s_index);
uint16_t getLCD_Units_StepValue(uint8_t spIndex,uint8_t stepsIndex);
uint16_t checkPowerLimits(uint8_t s_index, uint16_t newpower);

bool    spindleIsType(uint8_t i, uint8_t type);
uint8_t spindleGetTypID(uint8_t startIndex, uint8_t type);
void    spindleSetSpeed(uint8_t i, uint16_t speed);
uint16_t spindleGetSetSpeed(uint8_t i);
//void    spindleSetPercent(uint8_t i, uint8_t percent);
//uint8_t spindleGetSetPercent(uint8_t i);
void    spindleSetCurSpeed(uint8_t i, uint16_t speed);
uint16_t spindleGetCurSpeed(uint8_t i);
//void    spindleSetCurPercent(uint8_t i, uint8_t percent);
//uint8_t spindleGetCurPercent(uint8_t i);
void    spindleBuildList(void);
void    spindleControlInit(void);
void    loopSpindle(void);
bool    SpindleChanged(uint8_t i);
void    spindleSpeedQuery(void);
void    spindleQuerySetWait(bool wait);

#ifdef __cplusplus
}
#endif

#endif
