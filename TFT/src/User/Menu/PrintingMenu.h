#ifndef _PRINTING_MENU_H_
#define _PRINTING_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void menuBeforePrinting(void);
void menuPrinting(void);
void printInfoPopup(void);

extern bool hasFilamentData;

#ifdef __cplusplus
}
#endif

#endif
