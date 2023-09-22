#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include "variants.h"  // for uint32_t etc...
#include "uart.h"

typedef struct
{
  char *cache;
  volatile uint16_t wIndex;  // writing index. Precautionally declared as volatile due to access from interrupt handler and main thread
  volatile uint16_t rIndex;  // reading index. Precautionally declared as volatile due to access from interrupt handler and main thread
  uint16_t flag;             // custom flag (for custom usage by the application)
  uint16_t cacheSize;
} DMA_CIRCULAR_BUFFER;

extern DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT];

void Serial_Config(uint8_t port, uint16_t cacheSizeRX, uint16_t cacheSizeTX, uint32_t baudrate);
void Serial_DeConfig(uint8_t port);

// retrieve the next reading index in the message queue of the provided serial port:
//   - port: index of serial port
//
//   - return value: next reading index
uint16_t Serial_GetReadingIndex(uint8_t port);

// retrieve the next writing index in the message queue of the provided serial port
// based on Interrupt/DMA status while writing serial data in the background
//   - port: index of serial port
//
//   - return value: next writing index
uint16_t Serial_GetWritingIndex(uint8_t port);

// send character to a uart port. TX interrupt based serial writing is always used even if TX DMA based serial writing is enabled
void Serial_PutChar(uint8_t port, const char ch);

// send a zero terminated message to uart port. TX interrupt based serial writing is used if TX DMA based serial writing is disabled
void Serial_Put(uint8_t port, const char * msg);

#endif
