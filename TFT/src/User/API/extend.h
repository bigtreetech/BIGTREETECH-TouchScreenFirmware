#ifndef _EXTEND_H_
#define _EXTEND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

enum
{
  FILAMENT_SENSOR_NORMAL,
  FILAMENT_SENSOR_SMART,
};

void PS_ON_Init(void);
void PS_ON_On(void);
void PS_ON_Off(void);
void FIL_Runout_Init(void);
void FIL_PosE_SetUpdate(bool isUpdated);
void FIL_SFS_SetAlive(bool isAlive);
void loopBackEndFILRunoutDetect(void);
void loopFrontEndFILRunoutDetect(void);

#ifdef __cplusplus
}
#endif

#endif
