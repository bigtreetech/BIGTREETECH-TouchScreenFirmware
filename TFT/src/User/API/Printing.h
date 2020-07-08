#ifndef _PRINTING_H_
#define _PRINTING_H_
#include "variants.h"
#include "stdbool.h"
#include "ff.h"


#ifndef M27_WATCH_OTHER_SOURCES
#define M27_WATCH_OTHER_SOURCES    false
#endif

#ifndef M27_REFRESH
#define M27_REFRESH   3
#endif

#ifdef RAPID_SERIAL_COMM
#define rapid_serial_loop()  loopBackEnd()
#else
#define rapid_serial_loop()
#endif


typedef struct
{
  FIL     file;

  u32     time; // Printed time in sec
  u32     size; // Gcode file total size
  u32     cur; // Gcode has printed file size
  u8      progress;
  bool    printing; // 1 means printing, 0 means idle
  bool    pause; // 1 means paused
  bool    m0_pause; // pause triggered through M0/M1 gcode
  bool    runout; // 1: runout in printing, 0: idle
}PRINTING;

extern PRINTING infoPrinting;

void setPrintfinishAction(void (*_printfinish)());
bool isPrinting(void);
bool isPause(void);
bool isM0_Pause(void);
void breakAndContinue(void);
void setPrintingTime(u32 RTtime);

void exitPrinting(void);
void endPrinting(void);
void completePrinting(void);
void abortPrinting(void);
u8 *getCurGcodeName(char *path);
void sendPrintCodes(uint8_t index);

void setM0Pause(bool m0_pause);
bool setPrintPause(bool is_pause, bool is_m0pause);

void setPrintSize(u32 size);
void setPrintCur(u32 cur);
u32 getPrintSize(void);
u32 getPrintCur(void);
bool getPrintRunout(void);
void setPrintRunout(bool runout);

u8   getPrintProgress(void);
u32  getPrintTime(void);

void printSetUpdateWaiting(bool isWaiting);

void getGcodeFromFile(void);

void shutdown(void);
void shutdownLoop(void);
void startShutdown(void);

void printingFinished(void);
void loopCheckPrinting(void);




#endif
