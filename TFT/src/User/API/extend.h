#ifndef _EXTEND_H_
#define _EXTEND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

enum
{
  FILAMENT_RUNOUT_OFF = 0,
  FILAMENT_RUNOUT_ON,
  FILAMENT_SMART_RUNOUT_ON,
};

void PS_ON_Init(void);
void PS_ON_On(void);
void PS_ON_Off(void);
void positionSetUpdateWaiting(bool isWaiting);
void FIL_Runout_Init(void);
void FIL_SFS_SetAlive(uint8_t alive);
void loopBackEndFILRunoutDetect(void);
void loopFrontEndFILRunoutDetect(void);

#ifdef __cplusplus
}
#endif

#endif
