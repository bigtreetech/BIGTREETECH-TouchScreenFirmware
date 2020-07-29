#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_
#include "menu.h"
#include "MachineParameters.h"


void menuParameterSettings(void);

// Global Info
#define GLOBALICON_WIDTH   (BYTE_WIDTH * 2)
#define GLOBALICON_HEIGHT   GLOBALICON_WIDTH
#define GLOBALICON_INTERVAL 2

void loopTemperatureStatus(void);
void drawTemperatureStatus(void);

#endif
