#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "stm32f10x.h"
#include "parseACK.h"
#include "usart.h"

#define DMA_TRANS_LEN  ACK_MAX_SIZE
extern char dma_mem_buf[_USART_CNT][DMA_TRANS_LEN];

void Serial_DMAReEnable(uint8_t port);
    
void Serial_Config(u32 baud);
void Serial_DeConfig(void);
void Serial_Puts(uint8_t port, char *s);

#endif 




















