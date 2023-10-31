#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include "variants.h"  // for uint32_t etc...
#include "uart.h"

// comment out this line to use TX interrupt based serial writing instead of TX DMA based serial writing
#define TX_DMA_WRITE

typedef struct
{
  char * cache;
  uint32_t cacheSize;
  volatile uint32_t wIndex;  // writing index
  volatile uint32_t rIndex;  // reading index
  volatile uint32_t flag;    // custom flag (for custom usage by the application)
} DMA_CIRCULAR_BUFFER;

// config for USART DMA channels
typedef struct
{
  USART_TypeDef * uart;  // uint32_t
  uint32_t dma_rcc;
  DMA_TypeDef * dma_controller;
  DMA_Channel_TypeDef * dma_channelRX;
#ifdef TX_DMA_WRITE
  DMA_Channel_TypeDef * dma_channelTX;
#endif
} SERIAL_CFG;

extern DMA_CIRCULAR_BUFFER dmaL1DataRX[_UART_CNT];
extern const SERIAL_CFG Serial[_UART_CNT];

void Serial_Config(uint8_t port, uint32_t cacheSizeRX, uint32_t cacheSizeTX, uint32_t baudrate);
void Serial_DeConfig(uint8_t port);

// retrieve the next reading index in the RX message queue of the provided physical serial port:
//   - port: physical serial port
//
//   - return value: next reading index
static inline uint32_t Serial_GetReadingIndexRX(uint8_t port)
{
  return dmaL1DataRX[port].rIndex;
}

// retrieve the next writing index in the RX message queue of the provided physical serial port
// based on Interrupt/DMA status while writing serial data in the background:
//   - port: physical serial port
//
//   - return value: next writing index
static inline uint32_t Serial_GetWritingIndexRX(uint8_t port)
{
  return dmaL1DataRX[port].cacheSize - Serial[port].dma_channelRX->CNDTR;
}

// send a zero terminated message to UART port
//   - port: index of serial port
//   - msg: message to send
void Serial_Put(uint8_t port, const char * msg);

#endif
