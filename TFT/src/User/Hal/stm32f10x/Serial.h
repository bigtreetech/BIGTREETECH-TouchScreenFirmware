#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "uart.h"

typedef struct
{
  char *cache;
  uint16_t wIndex;
  uint16_t rIndex;
}DMA_CIRCULAR_BUFFER;

#if ((defined(MKS_28_V1_0)) || (defined(MKS_32_V1_4) && !defined(MKS_32_V1_4_NOBL)))
  #define DMA_TRANS_LEN  4096 // for 64k RAM
#else
  #define DMA_TRANS_LEN  3072 // for 48k RAM
#endif
extern DMA_CIRCULAR_BUFFER dmaL1Data[_UART_CNT];

void Serial_Init(u32 baud);
void Serial_DeInit(void);
void Serial_Puts(uint8_t port, char *s);

#endif
