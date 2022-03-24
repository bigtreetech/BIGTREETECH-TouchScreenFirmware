#ifndef _POWER_FAILED_H_
#define _POWER_FAILED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ff.h"

// called in PrintRestore.c
void powerFailedSetRestore(bool allowed);  // set to "true" to enable print restore flag (one shot flag)
bool powerFailedGetRestore(void);          // get the value of current print restore flag

// called in Printing.c

/**
 * initialize PLR info.
 * If print restore flag was enabled (e.g. by powerFailedSetRestore function called in PrintRestore.c),
 * try to load PLR info from file in order to restore the print from the failed point.
 * It finally disables print restore flag (one shot flag) for the next print.
 * The flag must always be explicitly re-enabled (e.g by powerFailedSetRestore function)
 */
bool powerFailedInitData(void);

bool powerFailedExist(void);
bool powerFailedCreate(char *path) ;
void powerFailedCache(uint32_t offset);
void powerFailedClose(void);
void powerFailedDelete(void);
bool powerFailedlSeek(FIL *fp);

#ifdef __cplusplus
}
#endif

#endif
