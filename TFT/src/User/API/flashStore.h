//TG MODIFIED*****
#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "variants.h"
#include "Settings.h"

// should this be 768 as in prior versions?
#define PARA_SIZE 512  //TG 8/30/21 increased from 256 Max size of settings buffer to read/write

void readStoredPara(void);
void storePara(void);
bool readIsTSCExist(void);
bool readIsRestored(void);

#ifdef __cplusplus
}
#endif

#endif
