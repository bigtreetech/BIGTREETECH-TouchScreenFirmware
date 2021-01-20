#ifndef _PRINTINGMENU_H_
#define _PRINTINGMENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"
#include "ff.h"

void menuBeforePrinting(void);
void menuPrinting(void);
void printInfoPopup(void);

extern bool hasFilamentLength;

#ifdef __cplusplus
}
#endif

#endif
