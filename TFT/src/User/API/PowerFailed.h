#ifndef _POWER_FAILED_H_
#define _POWER_FAILED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ff.h"

#define BREAK_POINT_FILE "Printing.sys"

void powerFailedClear(void);
bool powerFailedExist(void);
void powerFailedSetDriverSource(char *src);
bool powerFailedCreate(char *path) ;
void powerFailedCache(uint32_t offset);
void powerFailedClose(void);
void powerFailedDelete(void);
bool powerFailedlSeek(FIL *fp);
bool powerFailedGetData(void);

#ifdef __cplusplus
}
#endif

#endif
