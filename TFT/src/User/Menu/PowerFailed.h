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
  float    axis[TOTAL_AXIS];
  uint32_t feedrate;
  uint16_t speed,flow;
  uint16_t target[MAX_HEATER_COUNT];
  uint16_t fan[MAX_FAN_COUNT];
  uint8_t  tool;
  uint32_t offset;
  bool     relative;
  bool     relative_e;
  bool     pause;
} BREAK_POINT;


void menuPowerOff(void);

void powerFailedSetDriverSource(char *src);
bool powerFailedCreate(char *path) ;
void powerFailedCache(u32 offset);
void powerFailedClose(void);
void powerFailedDelete(void);

bool powerFailedlSeek(FIL* fp);

#endif
