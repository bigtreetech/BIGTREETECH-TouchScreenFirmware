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

bool powerFailedInitRestore(void);  // initialize print restore, if any, if not already initialized (one shot flag)

/**
 * load PLR info.
 * If print restore flag was enabled (e.g. by powerFailedSetRestore function called in PrintRestore.c),
 * try to load PLR info from file in order to restore the print from the failed point (setting the offset
 * on print file to the backed up layer).
 * It finally disables print restore flag (one shot flag) for the next print.
 * The flag must always be explicitly re-enabled (e.g by powerFailedSetRestore function)
 */
bool powerFailedLoad(FIL * print_fp);

bool powerFailedExist(void);
void powerFailedCreate(char * path);
void powerFailedCache(uint32_t offset);
void powerFailedDelete(void);

#ifdef __cplusplus
}
#endif

#endif
