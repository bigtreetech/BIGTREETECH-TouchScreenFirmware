#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include "variants.h"
#include "stdbool.h"


typedef enum
{
  X_AXIS=0,
  Y_AXIS,
  Z_AXIS,
  E_AXIS,
  TOTAL_AXIS
}AXIS;


typedef struct
{
  float axis[TOTAL_AXIS];
  u32   feedrate;
}COORDINATE;

extern const char axis_id[TOTAL_AXIS];

bool  coorGetRelative(void);
void  coorSetRelative(bool mode);
bool  eGetRelative(void);
void  eSetRelative(bool mode);
bool  coordinateIsKnown(void);
void  coordinateSetKnown(bool known);
float coordinateGetAxisTarget(AXIS axis);
void  coordinateSetAxisTarget(AXIS axis, float position);
u32   coordinateGetFeedRate(void);
void  coordinateSetFeedRate(u32 feedrate);
void  coordinateGetAll(COORDINATE *tmp);
float coordinateGetAxisActual(AXIS axis);
void coordinateSetAxisActual(AXIS axis, float position);
void  coordinateSetAxisActualSteps(AXIS axis, int steps);
void coordinateQuerySetWait(bool wait);
void coordinateQuery(void);

#endif
