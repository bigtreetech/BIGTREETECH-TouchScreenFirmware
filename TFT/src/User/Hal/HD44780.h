#ifndef _HD44780_H
#define _HD44780_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>         // for uint8_t etc.
#include "variants.h"       // for LCD2004_EMULATOR etc.
#include "CircularQueue.h"  // for CIRCULAR_QUEUE etc.

#ifdef LCD2004_EMULATOR
  void HD44780_DeConfig(void);
  void HD44780_Config(CIRCULAR_QUEUE * queue);
  bool HD44780_writeData(void);
  bool HD44780_getData(uint8_t * data);
#endif

#ifdef __cplusplus
}
#endif

#endif
