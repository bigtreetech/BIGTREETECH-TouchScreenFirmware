#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_
#include "menu.h"
#include "MachineParameters.h"


void menuParameterSettings(void);

// Global Info
#define INFO_STR_WIDTH    (BYTE_WIDTH * 7) // 123/123
#define GLOBALICON_WIDTH  (BYTE_WIDTH * 2)
#define GLOBALICON_HEIGHT GLOBALICON_WIDTH
#define ICON_BED_X        (LCD_WIDTH - INFO_STR_WIDTH - GLOBALICON_WIDTH - 2)
#define VALUE_BED_X       ICON_BED_X + GLOBALICON_WIDTH + 2

#define ICON_NOZZLE_X   (ICON_BED_X - INFO_STR_WIDTH - GLOBALICON_WIDTH - 2 - BYTE_WIDTH/2)
#define VALUE_NOZZLE_X  ICON_NOZZLE_X + GLOBALICON_WIDTH + 2

void show_GlobalInfo(void);
void temp_Change(void);
void drawGlobalInfo(void);

#endif
