#ifndef _PRINTING_H_
#define _PRINTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "variants.h"  // for RAPID_SERIAL_COMM
#include "main.h"      // for HOST_STATUS

#ifdef RAPID_SERIAL_COMM
  #define RAPID_SERIAL_LOOP() loopBackEnd()
  #define RAPID_PRINTING_COMM() if (isPrinting() == true && infoSettings.serial_always_on != 1) {loopBackEnd();}
#else
  #define RAPID_SERIAL_LOOP()
  #define RAPID_PRINTING_COMM()
#endif

#define SUMMARY_NAME_LEN 26  // max character length to copy from name buffer

typedef enum
{
  PROG_FILE = 0,  // file execution progress ()
  PROG_RRF,       // progress from RRF ("fraction_printed")
  PROG_TIME,      // time based progress (elapsed/total)
  PROG_SLICER,    // progress from slicer (M73)
} PROG_FROM;

typedef enum
{
  PAUSE_NORMAL = 0,
  PAUSE_M0,
  PAUSE_EXTERNAL,
} PAUSE_TYPE;

typedef struct
{
  // data
  char name[SUMMARY_NAME_LEN + 1];
  uint32_t time;
  float length;
  float weight;
  float cost;
  bool hasFilamentData;
} PRINT_SUMMARY;

extern PRINT_SUMMARY infoPrintSummary;

void setExtrusionDuringPause(bool extruded);

void setRunoutAlarmTrue(void);
void setRunoutAlarmFalse(void);
bool getRunoutAlarm(void);

void breakAndContinue(void);
void resumeAndPurge(void);
void resumeAndContinue(void);

//
// commented because NOT externally invoked
//
//void abortAndTerminate(void);
//void loopBreakToCondition(CONDITION_CALLBACK condCallback);

void setPrintExpectedTime(uint32_t expectedTime);
uint32_t getPrintExpectedTime(void);

void updatePrintTime(uint32_t osTime);
uint32_t getPrintTime(void);

void setPrintRemainingTime(int32_t remainingTime);  // used for M73 Rxx and M117 Time Left xx
void parsePrintRemainingTime(char * buffer);        // used for M117 Time Left xx
uint32_t getPrintRemainingTime();

void setPrintLayerNumber(uint16_t layerNumber);
uint16_t getPrintLayerNumber();

void setPrintLayerCount(uint16_t layerCount);
uint16_t getPrintLayerCount();

void setPrintProgressSource(PROG_FROM progressSource);
PROG_FROM getPrintProgressSource(void);

//
// used for print based on gcode file
//
uint32_t getPrintDataSize(void);
uint32_t getPrintDataCur(void);
void setPrintProgressData(float cur, float size);

//
// used for print based on M73 Pxx or RRF
//
void setPrintProgressPercentage(uint8_t percentage);

uint8_t updatePrintProgress(void);
uint8_t getPrintProgress(void);

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

void setPrintUpdateWaiting(bool isWaiting);  // called in interfaceCmd.c
void updatePrintUsedFilament(void);          // called in PrintingMenu.c
void clearInfoPrint(void);                   // called in PrintingMenu.c

//
// commented because NOT externally invoked
//
//void completePrint(void);                           // complete a print (finalize stats etc.)

// start print originated and/or hosted (handled) by remote host
// (e.g. print started from remote onboard media or hosted by remote host)
bool startPrintFromRemoteHost(const char * filename);

// start print originated and/or hosted (handled) by TFT
// (e.g. print started from onboard media or hosted by TFT)
bool startPrint(void);                                // it also sends start gcode

void endPrint(void);                                  // it also sends end gcode
void abortPrint(void);                                // it also sends cancel gcode
bool pausePrint(bool isPause, PAUSE_TYPE pauseType);

bool isPrinting(void);                // return "true" in case a print is ongoing
bool isPaused(void);                  // return "true" in case a print is paused
bool isAborted(void);                 // return "true" in case a print is aborted/canceled
bool isPrintingFromTFT(void);         // return "true" in case a print hosted (handled) by TFT is ongoing
bool isPrintingFromOnboard(void);     // return "true" in case a print hosted (handled) by onboard (Marlin) is ongoing
bool isPrintingFromRemoteHost(void);  // return "true" in case a print hosted (handled) by remote host is ongoing

//
// used for print hosted (handled) by onboard (Marlin) or remote host
// (e.g. print started from (remote) onboard media or hosted by remote host)
//
void setPrintAbort(void);
void setPrintPause(HOST_STATUS hostStatus, PAUSE_TYPE pauseType);
void setPrintResume(HOST_STATUS hostStatus);

void loopPrintFromTFT(void);      // called in loopBackEnd(). It handles a print from TFT media, if any
void loopPrintFromOnboard(void);  // called in loopBackEnd(). It handles a print from (remote) onboard media, if any

#ifdef __cplusplus
}
#endif

#endif
