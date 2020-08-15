#ifndef _TUNE_EXTRUDER_H
#define _TUNE_EXTRUDER_H

#include "stdint.h"
#include "stdbool.h"

#include "Configuration.h"
#include "Settings.h"
#include "Temperature.h"

void updateExtruderESteps(void);

void menuTuneExtruder(void);
void menuNewExtruderESteps(void);

#endif