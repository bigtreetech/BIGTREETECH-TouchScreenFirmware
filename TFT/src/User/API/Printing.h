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
  FIL        file;
  uint32_t   size;                // Gcode file total size
  uint32_t   cur;                 // Gcode file printed size
  uint32_t   expectedTime;        // expected print duration in sec
  uint32_t   time;                // current elapsed time in sec
  uint32_t   remainingTime;       // current remaining time in sec (if set with M73 or M117)
  uint16_t   layerNumber;
  uint16_t   layerCount;
  uint8_t    prevProgress;
  uint8_t    progress;
  bool       progressFromSlicer;  // 1: progress controlled by Slicer (if set with M73)
  bool       runout;              // 1: runout in printing, 0: idle
  bool       printing;            // 1: means printing, 0: means idle
  bool       pause;               // 1: means paused
  PAUSE_TYPE pauseType;           // pause type trigged by different sources and gcodes like M0 & M600
} PRINTING;

typedef struct
{
  // data
  char name[SUMMARY_NAME_LEN + 1];
  uint32_t time;
  float length;
  float weight;
  float cost;
} PRINT_SUMMARY;

extern PRINTING infoPrinting;
extern PRINT_SUMMARY infoPrintSummary;

bool isHostPrinting(void);  // condition callback for loopProcessToCondition()

void setRunoutAlarmTrue(void);
void setRunoutAlarmFalse(void);
bool getRunoutAlarm(void);

void breakAndContinue(void);
void resumeAndPurge(void);
void resumeAndContinue(void);

void setPrintTime(uint32_t elapsedTime);
uint32_t getPrintTime(void);
void getPrintTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec);

void setPrintRemainingTime(int32_t remainingTime);  // used for M73 Rxx and M117 Time Left xx
void parsePrintRemainingTime(char * buffer);        // used for M117 Time Left xx
void getPrintRemainingTimeDetail(uint8_t * hour, uint8_t * min, uint8_t * sec);

void setPrintProgressPercentage(uint8_t percentage);  // used by M73 Pxx
bool updatePrintProgress(void);

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
bool printPause(bool isPause, PAUSE_TYPE pauseType);

bool isPrinting(void);
bool isPaused(void);

void setPrintHost(bool isPrinting);
void setPrintAbort(void);
void setPrintPause(bool updateHost, PAUSE_TYPE pauseType);
void setPrintResume(bool updateHost);

void loopPrintFromTFT(void);   // called in loopBackEnd(). It handles a print from TFT, if any
void loopPrintFromHost(void);  // called in loopBackEnd(). It handles a print from onboard SD or remote host (e.g. USB), if any

#ifdef __cplusplus
}
#endif

#endif
