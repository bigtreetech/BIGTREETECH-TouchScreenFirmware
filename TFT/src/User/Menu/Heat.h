#ifndef _HEAT_H_
#define _HEAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "Configuration.h"
#include "Settings.h"
#include "menu.h"
#include "Temperature.h"



void menuHeat(void);
void heatSetCurrentIndex(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
