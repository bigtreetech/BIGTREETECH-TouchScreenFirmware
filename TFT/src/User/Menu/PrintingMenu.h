#ifndef _PRINTINGMENU_H_
#define _PRINTINGMENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"
#include "ff.h"

void menuBeforePrinting(void);
void menuPrinting(void);
void printInfoPopup(void);
extern bool filDataSeen;

typedef struct
{
  /* data */
  char name[30];
  uint32_t time;
  float length;
  float weight;
  float cost;
} FILAMENTDATA;

extern FILAMENTDATA filData;

#ifdef __cplusplus
}
#endif

#endif
