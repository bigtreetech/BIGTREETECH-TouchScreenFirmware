#ifndef _POWERFAILED_H_
#define _POWERFAILED_H_

#include "variants.h"
#include "Heat.h"
#include "coordinate.h"
#include "ff.h"
#include "Configuration.h"

#define BREAK_POINT_FILE "Printing.sys"

typedef struct
{
  float axis[TOTAL_AXIS];
  u32   feedrate;
  u16   speed,flow;
  u16   target[MAX_HEATER_COUNT];
  u16   fan[MAX_FAN_COUNT];
  TOOL  nozzle;
  u32   offset;
  bool  relative;
  bool  relative_e;
} BREAK_POINT;


void menuPowerOff(void);

void powerFailedSetDriverSource(char *src);
bool powerFailedCreate(char *path) ;
void powerFailedCache(u32 offset);
void powerFailedClose(void);
void powerFailedDelete(void);

bool powerFailedlSeek(FIL* fp);

#endif
