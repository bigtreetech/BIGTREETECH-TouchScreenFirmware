#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

#include "stdbool.h"
#include "variants.h"
#include "Settings.h"
#include "includes.h"

extern bool wasRestored;

bool readStoredPara(void);
void storePara(void);

#endif
