#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include "variants.h"  // for uint32_t etc...
#include "uart.h"

typedef struct
{
  char *cache;
  uint16_t wIndex;
  uint16_t rIndex;
  uint16_t cacheSize;
} DMA_CIRCULAR_BUFFER;

extern DMA_CIRCULAR_BUFFER dmaL1Data[_UART_CNT];

void Serial_Config(uint8_t port, uint16_t cacheSize, uint32_t baudrate);
void Serial_DeConfig(uint8_t port);
void Serial_Puts(uint8_t port, const char *s);
void Serial_Putchar(uint8_t port, const char ch);

#endif
