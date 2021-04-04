#ifndef _PRINTING_H_
#define _PRINTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"
#include "ff.h"

#ifndef M27_WATCH_OTHER_SOURCES
  #define M27_WATCH_OTHER_SOURCES false
#endif

#ifndef M27_REFRESH
  #define M27_REFRESH 3
#endif

#ifdef RAPID_SERIAL_COMM
  #define RAPID_SERIAL_LOOP() loopBackEnd()
  #define RAPID_PRINTING_COMM() if (isPrinting() == true && infoSettings.serial_alwaysOn != 1) {loopBackEnd();}
#else
  #define RAPID_SERIAL_LOOP()
  #define RAPID_PRINTING_COMM()
#endif

#define SUMMARY_NAME_LEN 26  // max character length to copy from name buffer

typedef struct
{
  // data
  char name[SUMMARY_NAME_LEN + 1];
  uint32_t time;
  float length;
  float weight;
  float cost;
} PRINT_SUMMARY;

extern PRINT_SUMMARY infoPrintSummary;

void setRunoutAlarmTrue(void);
void setRunoutAlarmFalse(void);
bool getRunoutAlarm(void);

void breakAndContinue(void);
void resumeAndPurge(void);
void resumeAndContinue(void);

void setPrintTime(uint32_t RTtime);
uint32_t getPrintTime(void);
void getPrintTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec);

uint32_t getPrintSize(void);
uint32_t getPrintCur(void);

void setPrintProgress(float cur, float size);
bool updatePrintProgress(void);
uint8_t getPrintProgress(void);

void setPrintModelIcon(bool exist);
bool isPrintModelIcon(void);

void setPrintRunout(bool runout);
bool getPrintRunout(void);

//
// commented because NOT externally invoked
//
//void shutdown(void);
//void shutdownLoop(void);
//void shutdownStart(void);  // start auto shutdown after a print successfully completed
//void initPrintSummary(void);
//void preparePrintSummary(void);
//void sendPrintCodes(uint8_t index);

void printSetUpdateWaiting(bool isWaiting);       // called in interfaceCmd.c
void updatePrintUsedFilament(void);               // called in PrintingMenu.c
uint8_t * getPrintName(char * path);              // called in PrintingMenu.c
void clearInfoPrint(void);                        // called in PrintingMenu.c

void printStart(FIL * file, uint32_t size);       // it also sends start gcode
void printEnd(void);                              // it also sends end gcode

void printComplete(void);                         // print successfully completed
void printAbort(void);                            // it also sends cancel gcode
bool printPause(bool is_pause, bool is_m0pause);

bool isPrinting(void);
bool isPaused(void);

void setPrintHost(bool isPrinting);
void setPrintAbort(void);
void setPrintPause(bool updateHost);
void setPrintResume(bool updateHost);

void loopPrintFromTFT(void);   // called in loopBackEnd(). It handles a print from TFT, if any
void loopPrintFromHost(void);  // called in loopBackEnd(). It handles a print from onboard SD or remote host (e.g. USB), if any

#ifdef __cplusplus
}
#endif

#endif
