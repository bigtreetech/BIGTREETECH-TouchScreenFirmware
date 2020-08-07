#ifndef _MARLIN_MODE_H_
#define _MARLIN_MODE_H_

#include "stdint.h"
#include "Configuration.h"

// User-defined colors for Marlin mode from Configuration.h
#ifndef MARLIN_BKCOLOR
  #define MARLIN_BKCOLOR LCD_BLACK
#endif

#ifndef MARLIN_FNCOLOR
  #define MARLIN_FNCOLOR LCD_GREEN
#endif

#define QUEUE_MAX_BYTE (1024 * 5)

typedef struct
{
  uint8_t  *data;
  uint16_t index_r; // Ring buffer read position
  uint16_t index_w; // Ring buffer write position
  uint16_t count;   // Count of commands in the queue
}CIRCULAR_QUEUE;

extern CIRCULAR_QUEUE marlinQueue;

void menuMarlinMode(void);

#endif
