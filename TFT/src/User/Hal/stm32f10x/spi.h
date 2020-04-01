#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f10x.h"


#define _SPI1     0
#define _SPI2     1
#define _SPI3     2
#define _SPI_CNT  3


void SPI_GPIO_Init(uint8_t port);
void SPI_Config(uint8_t port);
void SPI_DeConfig(uint8_t port);
void SPI_Protocol_Init(uint8_t port, uint8_t baudrate);
uint16_t SPI_Read_Write(uint8_t port, uint16_t d);

#endif
