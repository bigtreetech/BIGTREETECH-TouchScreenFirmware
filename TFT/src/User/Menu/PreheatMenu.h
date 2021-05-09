#ifndef _PREHEAT_MENU_H_
#define _PREHEAT_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "menu.h"

typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  NOZZLE0_PREHEAT = 2,
} TOOLPREHEAT;

void refreshPreheatIcon(int8_t preheatnum, int8_t icon_index, const ITEM * menuitem);
void menuPreheat(void);

#ifdef __cplusplus
}
#endif

#endif
