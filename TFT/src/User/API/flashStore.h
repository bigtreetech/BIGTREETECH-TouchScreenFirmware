#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

#include "stdbool.h"
#include "variants.h"
#include "Settings.h"

extern bool wasRestored;

bool readStoredPara(void);
void storePara(void);

#endif
