#ifndef _PRINTING_MENU_H_
#define _PRINTING_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

// start print originated and/or hosted (handled) by remote host
// (e.g. print started from remote onboard media or hosted by remote host) and open Printing menu
void startPrintingFromRemoteHost(const char * filename);

// start print originated and/or hosted (handled) by TFT
// (e.g. print started from onboard media or hosted by TFT) and open Printing menu
void startPrinting(void);

void printSummaryPopup(void);
void menuPrinting(void);

#ifdef __cplusplus
}
#endif

#endif
