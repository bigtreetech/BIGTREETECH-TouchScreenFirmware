#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "CircularQueue.h"
#include <stdbool.h>

void HD44780_DeConfig(void);
void HD44780_Config(CIRCULAR_QUEUE *queue);
bool HD44780_writeData(void);
bool HD44780_getData(uint8_t *data);

#ifdef __cplusplus
}
#endif
