#ifndef _HEAT_H_
#define _HEAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * index >= 0 to set the specific index
 * index == -1 to set the last used hotend index
 * index == -2 to set the last used bed index
 */
void heatSetCurrentIndex(uint8_t index);

void menuHeat(void);

#ifdef __cplusplus
}
#endif

#endif
