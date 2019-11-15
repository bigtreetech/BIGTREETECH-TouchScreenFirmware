#ifndef _SPI_SLAVE_H_
#define _SPI_SLAVE_H_

#include "stm32f2xx_conf.h"

#define SPI_SLAVE_MAX (1024 * 5)

typedef struct
{
  u8  *data;
  u16 rIndex;
  u16 wIndex;
}SPI_QUEUE;

extern SPI_QUEUE SPISlave;

void SPI_Slave(void);
void SPI_SlaveDeInit(void);
void SPI_Slave_CS_Config(void);
#endif
