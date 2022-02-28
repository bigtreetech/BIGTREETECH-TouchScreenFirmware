#ifndef _PRINTING_MENU_H_
#define _PRINTING_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void menuBeforePrinting(void);  // start print of file selected by TFT's GUI
void initMenuPrinting(void);    // initialize things before opening menuPrinting
void menuPrinting(void);
void printSummaryPopup(void);

extern bool hasFilamentData;

#ifdef __cplusplus
}
#endif

#endif
