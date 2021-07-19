#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  X_AXIS = 0,
  Y_AXIS,
  Z_AXIS,
  E_AXIS,
  TOTAL_AXIS
} AXIS;

typedef struct
{
  float axis[TOTAL_AXIS];
  uint32_t feedrate;
} COORDINATE;

typedef struct
{
  float coordinate;
  uint32_t feedrate;
  bool relative;
  bool backedUp;
} E_AXIS_BACKUP;

extern const char axis_id[TOTAL_AXIS];
E_AXIS_BACKUP eAxisBackup;

bool coorGetRelative(void);
void coorSetRelative(bool mode);
bool eGetRelative(void);
void eSetRelative(bool mode);
bool coordinateIsKnown(void);
void coordinateSetKnown(bool known);
float coordinateGetAxisTarget(AXIS axis);
void coordinateSetAxisTarget(AXIS axis, float position);
uint32_t coordinateGetFeedRate(void);
void coordinateSetFeedRate(uint32_t feedrate);
void coordinateGetAll(COORDINATE *tmp);
float coordinateGetExtruderActual(void);
void coordinateSetExtruderActualSteps(float steps);
float coordinateGetAxisActual(AXIS axis);
void coordinateSetAxisActual(AXIS axis, float position);
void coordinateQuerySetWait(bool wait);
void coordinateQuery(uint8_t delay);

#ifdef __cplusplus
}
#endif

#endif
