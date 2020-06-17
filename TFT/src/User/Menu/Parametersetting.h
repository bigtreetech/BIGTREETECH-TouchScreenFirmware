#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_
#include "menu.h"
#include "MachineParameters.h"


void menuParameterSettings(void);

// Global Info

#define ICON_NOZZLE_X   2*ICON_WIDTH+ICON_WIDTH/2
#define VALUE_NOZZLE_X  ICON_NOZZLE_X+BYTE_WIDTH

#define ICON_BED_X      3*ICON_WIDTH+2*BYTE_WIDTH+ICON_WIDTH/2
#define VALUE_BED_X     ICON_BED_X+BYTE_WIDTH

void show_GlobalInfo(void);
void temp_Change(void);
void drawGlobalInfo(void);

#endif
