#ifndef _HD44780_H
#define _HD44780_H

#include "includes.h"

void HD44780_DeConfig(void);
void HD44780_Config(CIRCULAR_QUEUE *queue);
bool HD44780_writeData(void);
bool HD44780_getData(uint8_t *data);

#endif
