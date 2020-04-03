#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

#include "stdbool.h"
#include "variants.h"
#include "Settings.h"

extern bool wasRestored;
#define PARA_SIZE 512  //bytes

bool readStoredPara(void);
void storePara(void);

#endif
