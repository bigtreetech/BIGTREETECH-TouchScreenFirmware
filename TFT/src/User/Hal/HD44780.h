#ifndef _HD44780_H
#define _HD44780_H

#include "variants.h"
#define HD44780_data_MAX (1024 * 5)

typedef struct
{
  u8  *data;
  u16 rIndex;
  u16 wIndex;
}HD44780_QUEUE;

extern HD44780_QUEUE HD44780_queue;

void HD44780_DeConfig(void);
void HD44780_Config(void);

#endif