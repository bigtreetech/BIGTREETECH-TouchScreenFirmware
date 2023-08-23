#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include "variants.h"  // for uint32_t etc...
#include "uart.h"

typedef volatile struct  // precautionally declared as volatile due to access from interrupt handler and main thread
{
  char *cache;
  uint16_t wIndex;  // writing index
  uint16_t rIndex;  // reading index
  uint16_t flag;    // custom flag (for custom usage by the application)
  uint16_t cacheSize;
} DMA_CIRCULAR_BUFFER;

extern DMA_CIRCULAR_BUFFER dmaL1Data[_UART_CNT];

void Serial_Config(uint8_t port, uint16_t cacheSize, uint32_t baudrate);
void Serial_DeConfig(uint8_t port);
void Serial_Put(uint8_t port, const char *s);
void Serial_PutChar(uint8_t port, const char ch);

#endif
