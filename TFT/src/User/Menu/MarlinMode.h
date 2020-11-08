#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "Configuration.h"

// User-defined colors for Marlin mode from Configuration.h
#ifndef MARLIN_BKCOLOR
  #define MARLIN_BKCOLOR LCD_BLACK
#endif

#ifndef MARLIN_FNCOLOR
  #define MARLIN_FNCOLOR LCD_GREEN
#endif

void menuMarlinMode(void);

#ifdef __cplusplus
}
#endif
