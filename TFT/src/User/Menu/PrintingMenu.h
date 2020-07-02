#ifndef _PRINTINGMENU_H_
#define _PRINTINGMENU_H_
#include "variants.h"
#include "stdbool.h"
#include "ff.h"

void menuBeforePrinting(void);
void menuPrinting(void);
void menuStopPrinting(void);

void printingDrawPage(void);
void reDrawProgress(int icon_pos);
void reValueNozzle(int icon_pos);
void reValueBed(int icon_pos);
void reDrawTime(int icon_pos);
void reDrawLayer(int icon_pos);

#endif
