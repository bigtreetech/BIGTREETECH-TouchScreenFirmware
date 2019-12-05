#ifndef _PRINTING_H_
#define _PRINTING_H_
#include "variants.h"
#include "stdbool.h"
#include "ff.h"

typedef struct 
{	
  FIL     file;

  u32     time; // Printed time in sec
  u32     size; // Gcode file total size
  u32     cur; // Gcode has printed file size
  u8      progress;
  bool    printing; // 1 means printing, 0 means idle
  bool    pause; //1 means paused
}PRINTING;

void exitPrinting(void);
void endPrinting(void);
void completePrinting(void);
void abortPrinting(void);

bool setPrintPause(bool is_pause);

bool isPrinting(void);	
bool isPause(void);
void setPrintingTime(u32 RTtime);

void setPrintSize(u32 size);
void setPrintCur(u32 cur);

u8   getPrintProgress(void);
u32  getPrintTime(void);

void printSetUpdateWaiting(bool isWaiting);

void startGcodeExecute(void);
void endGcodeExecute(void);

void getGcodeFromFile(void);

void menuBeforePrinting(void);
void menuPrinting(void);
void menuStopPrinting(void);
void menuShutDown(void);

void printingDrawPage(void);
void reDrawProgress(u8 progress);
void reValueNozzle(void);
void reValueBed(void);
void reDrawTime(void);

void loopCheckPrinting(void);

#endif
