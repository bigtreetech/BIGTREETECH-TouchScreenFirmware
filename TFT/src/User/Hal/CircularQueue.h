#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_

#include "includes.h"

#define CIRCULAR_QUEUE_SIZE (1024 * 5)

typedef struct
{
  uint8_t  data[CIRCULAR_QUEUE_SIZE];                      // Data buffer
  uint16_t index_r;                                        // Ring buffer read position
  uint16_t index_w;                                        // Ring buffer write position
  uint16_t count;                                          // Count of commands in the queue
} CIRCULAR_QUEUE;

#endif
