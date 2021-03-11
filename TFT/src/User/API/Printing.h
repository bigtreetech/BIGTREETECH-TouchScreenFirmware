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
  FIL file;
  uint32_t time;        // Printed time in sec
  uint32_t size;        // Gcode file total size
  uint32_t cur;         // Gcode has printed file size
  uint8_t  progress;
  bool     printing;    // 1 means printing, 0 means idle
  bool     pause;       // 1 means paused
  bool     m0_pause;    // pause triggered through M0/M1 gcode
  bool     runout;      // 1: runout in printing, 0: idle
  bool     model_icon;  // 1: model preview icon exist, 0: not exist
} PRINTING;

typedef struct
{
  // data
  char name[SUMMARY_NAME_LEN + 1];
  uint32_t time;
  float length;
  float weight;
  float cost;
} PRINTSUMMARY;

extern PRINTING infoPrinting;
extern PRINTSUMMARY infoPrintSummary;

bool isPrinting(void);
bool isPause(void);
bool isM0_Pause(void);
void setPrintingTime(uint32_t RTtime);
void setPrintSize(uint32_t size);
uint32_t getPrintSize(void);
uint32_t getPrintCur(void);
bool getPrintRunout(void);
void setPrintRunout(bool runout);
void setPrintCur(uint32_t cur);

void setRunoutAlarmTrue(void);
void setRunoutAlarmFalse(void);
bool getRunoutAlarm(void);

void setPrintModelIcon(bool exist);
bool getPrintModelIcon(void);

uint8_t getPrintProgress(void);
uint32_t getPrintTime(void);
void printSetUpdateWaiting(bool isWaiting);
uint8_t *getCurGcodeName(char *path);
void sendPrintCodes(uint8_t index);

void initPrintSummary(void);
void updateFilamentUsed(void);
void preparePrintSummary(void);

bool setPrintPause(bool is_pause, bool is_m0pause);
void exitPrinting(void);
void endPrinting(void);
void abortPrinting(void);
void printingFinished(void);
void shutdown(void);
void shutdownLoop(void);
void startShutdown(void);

void getGcodeFromFile(void);
void breakAndContinue(void);
void resumeAndPurge(void);
void resumeAndContinue(void);

void loopCheckPrinting(void);

#ifdef __cplusplus
}
#endif

#endif
