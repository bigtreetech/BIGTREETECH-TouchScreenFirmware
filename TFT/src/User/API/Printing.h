#ifndef _PRINTING_H_
#define _PRINTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"
#include "ff.h"

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
} PRINT_SUMMARY;

extern PRINT_SUMMARY infoPrintSummary;

void setExtrusionDuringPause(bool extruded);

bool isHostPrinting(void);  // condition callback for loopProcessToCondition()

void setRunoutAlarmTrue(void);
void setRunoutAlarmFalse(void);
bool getRunoutAlarm(void);

void breakAndContinue(void);
void resumeAndPurge(void);
void resumeAndContinue(void);

void setPrintExpectedTime(uint32_t expectedTime);
uint32_t getPrintExpectedTime(void);

void setPrintTime(uint32_t elapsedTime);
uint32_t getPrintTime(void);
void getPrintTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec);

void setPrintRemainingTime(int32_t remainingTime);  // used for M73 Rxx and M117 Time Left xx
void parsePrintRemainingTime(char * buffer);        // used for M117 Time Left xx
uint32_t getPrintRemainingTime();
void getPrintRemainingTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec);

void setPrintLayerNumber(uint16_t layerNumber);
uint16_t getPrintLayerNumber();

void setPrintLayerCount(uint16_t layerCount);
uint16_t getPrintLayerCount();

uint32_t getPrintSize(void);
uint32_t getPrintCur(void);

void setPrintProgress(float cur, float size);
void setPrintProgressPercentage(uint8_t percentage);  // used by M73 Pxx
bool updatePrintProgress(void);
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

void printSetUpdateWaiting(bool isWaiting);           // called in interfaceCmd.c
void updatePrintUsedFilament(void);                   // called in PrintingMenu.c
uint8_t * getPrintName(char * path);                  // called in PrintingMenu.c
void clearInfoPrint(void);                            // called in PrintingMenu.c

void printComplete(void);                             // print complete
void printRemoteStart(const char * filename);         // print started from remote onboard SD or remote host
void printStart(FIL * file, uint32_t size);           // it also sends start gcode
void printEnd(void);                                  // it also sends end gcode
void printAbort(void);                                // it also sends cancel gcode
bool printPause(bool isPause, PAUSE_TYPE pauseType);

bool isPrinting(void);
bool isPaused(void);
bool isTFTPrinting(void);
bool isRemoteHostPrinting(void);

void setPrintAbort(void);
void setPrintPause(bool updateHost, PAUSE_TYPE pauseType);
void setPrintResume(bool updateHost);

void loopPrintFromTFT(void);        // called in loopBackEnd(). It handles a print from TFT, if any
void loopPrintFromOnboardSD(void);  // called in loopBackEnd(). It handles a print from (remote) onboard SD, if any

#ifdef __cplusplus
}
#endif

#endif
