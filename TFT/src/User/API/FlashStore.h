#ifndef _FLASH_STORE_H_
#define _FLASH_STORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "Settings.h"

#define PARA_SIZE (128 * 3)  // Max size of settings buffer to read/write

extern int32_t TSC_Para[7];
extern SETTINGS infoSettings;

void readStoredPara(void);
void storePara(void);
bool readIsTSCExist(void);
bool readIsRestored(void);

#ifdef __cplusplus
}
#endif

#endif
