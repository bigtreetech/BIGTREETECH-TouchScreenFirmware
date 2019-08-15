#ifndef _PRINTING_H_
#define _PRINTING_H_
#include "variants.h"
#include "stdbool.h"
#include "ff.h"

typedef struct 
{	
  FIL     file;

  u32     time;           //锟斤拷印时锟斤拷  锟斤拷位锟斤拷锟斤拷
  u32     size;           //锟斤拷锟斤拷印锟侥硷拷锟斤拷锟斤拷锟街斤拷锟斤拷
  u32     cur;            //锟窖讹拷取锟斤拷锟斤拷锟街斤拷锟斤拷
  u8      progress;
  bool    printing;      //锟斤拷印锟斤拷志  锟斤拷锟叫★拷准锟斤拷锟叫★拷锟斤拷印锟叫★拷锟斤拷印锟斤拷锟�
  bool    pause;          //1锟斤拷锟斤拷停  0锟斤拷未锟斤拷停

//  u32     lastUpdate;    // Last OS_time of update
}PRINTING;

void exitPrinting(void);
void endPrinting(void);
void completePrinting(void);
void haltPrinting(void);

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

void printingDrawPage(void);
void reDrawProgress(u8 progress);
void reValueNozzle(void);
void reValueBed(void);
void reDrawTime(void);

void loopCheckPrinting(void);

#endif
