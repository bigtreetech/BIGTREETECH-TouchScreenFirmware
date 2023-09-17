#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include "variants.h"  // for uint32_t etc...
#include "uart.h"

typedef volatile struct  // precautionally declared as volatile due to access from interrupt handler and main thread
{
  char *cache;
  volatile uint16_t wIndex;  // writing index
  volatile uint16_t rIndex;  // reading index
  uint16_t flag;    // custom flag (for custom usage by the application)
  uint16_t cacheSize;
} DMA_CIRCULAR_BUFFER;

extern DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT];  // DMA RX buffer
extern char * dmaL1DataTX[_UART_CNT];               // DMA TX buffer

void Serial_Config(uint8_t port, uint16_t cacheSizeRX, uint16_t cacheSizeTX, uint32_t baudrate);
void Serial_DeConfig(uint8_t port);
void Serial_Put(uint8_t port, const char *s);

#endif
