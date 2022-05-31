#ifndef _SPI_SLAVE_H_
#define _SPI_SLAVE_H_

#include "CircularQueue.h"
#include <stdbool.h>

void SPI_Slave(CIRCULAR_QUEUE *queue);
void SPI_SlaveDeInit(void);
bool SPI_SlaveGetData(uint8_t *data);

#endif
