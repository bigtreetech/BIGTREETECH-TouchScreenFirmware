#ifndef _PRINTING_H_
#define _PRINTING_H_
#include "variants.h"
#include "stdbool.h"
#include "ff.h"

#if LCD_WIDTH == 480 && LCD_HEIGHT == 320

  #define PICON_LG_WIDTH     154
  #define PICON_SM_WIDTH     121
  #define PICON_HEIGHT       50
  #define PICON_SPACE_X      ((LCD_WIDTH - PICON_LG_WIDTH*2 - PICON_SM_WIDTH - SPACE_X )/2)
  #define PICON_SPACE_Y      (ICON_HEIGHT + SPACE_Y - PICON_HEIGHT*2)/2
  #define PICON_TITLE_X      52
  #define PICON_TITLE_Y      1
  #define PICON_VAL_X        52
  #define PICON_VAL_Y        (PICON_HEIGHT/2)
  #define PICON_VAL_LG_EX    150
  #define PICON_VAL_SM_EX    117

#elif LCD_WIDTH == 320 && LCD_HEIGHT == 240

  #define PICON_LG_WIDTH     106
  #define PICON_SM_WIDTH     87
  #define PICON_HEIGHT       36
  #define PICON_SPACE_X      ((LCD_WIDTH - PICON_LG_WIDTH*2 - PICON_SM_WIDTH - SPACE_X )/2)
  #define PICON_SPACE_Y      (ICON_HEIGHT + SPACE_Y - PICON_HEIGHT*2)/2
  #define PICON_TITLE_X      37
  #define PICON_TITLE_Y      1
  #define PICON_VAL_X        (PICON_HEIGHT/2)
  #define PICON_VAL_Y        25
  #define PICON_VAL_LG_EX    104
  #define PICON_VAL_SM_EX    85

#endif

typedef struct 
{
  FIL     file;

  u32     time; // Printed time in sec
  u32     size; // Gcode file total size
  u32     cur; // Gcode has printed file size
  u8      progress;
  bool    printing; // 1 means printing, 0 means idle
  bool    pause; //1 means paused
  bool    m0_pause; //pause triggered through M0/M1 gcode
}PRINTING;

void exitPrinting(void);
void endPrinting(void);
void completePrinting(void);
void abortPrinting(void);

void setM0Pause(bool m0_pause);
bool setPrintPause(bool is_pause,bool is_m0pause);

bool isPrinting(void);
bool isPause(void);
bool isM0_Pause(void);
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
void reDrawProgress(int icon_pos);
void reValueNozzle(int icon_pos);
void reValueBed(int icon_pos);
void reDrawTime(int icon_pos);
void reDrawLayer(int icon_pos);

void loopCheckPrinting(void);

#endif
