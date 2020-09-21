#ifndef _TUNE_EXTRUDER_H
#define _TUNE_EXTRUDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "Configuration.h"
#include "Settings.h"
#include "Temperature.h"

void menuTuneExtruder(void);
void menuNewExtruderESteps(void);

#ifdef __cplusplus
}
#endif

#endif
