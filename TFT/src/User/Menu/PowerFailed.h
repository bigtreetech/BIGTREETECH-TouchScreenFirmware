#ifndef _POWER_FAILED_H_
#define _POWER_FAILED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "Settings.h"
#include "ff.h"

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
void powerFailedCache(uint32_t offset);
void powerFailedClose(void);
void powerFailedDelete(void);

bool powerFailedlSeek(FIL* fp);

#ifdef __cplusplus
}
#endif

#endif
