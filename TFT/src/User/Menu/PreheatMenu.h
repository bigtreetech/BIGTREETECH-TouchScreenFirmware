#ifndef _PREHEATMENU_H_
#define _PREHEATMENU_H_
//TG MODIFIED BY T.GIOIOSA
#ifdef __cplusplus
extern "C" {
#endif

#include "Configuration.h"

typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  TOOL0_PREHEAT = 2,
}TOOLPREHEAT;

void menuPreheat(void);

#ifdef __cplusplus
}
#endif

#endif
