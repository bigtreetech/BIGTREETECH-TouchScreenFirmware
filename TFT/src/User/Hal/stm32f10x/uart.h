#ifndef _UART_H_
#define _UART_H_

#include <stdbool.h>
#include <stdint.h>

#define _USART1    0
#define _USART2    1
#define _USART3    2
#define _UART4     3
#define _UART5     4  // UART5 don't support DMA
#define _UART_CNT  5

void UART_Config(uint8_t port, uint32_t baud, uint16_t usart_it, bool idle_interrupt);
void UART_DeConfig(uint8_t port);
void UART_Puts(uint8_t port, uint8_t *str);
void UART_Write(uint8_t port, uint8_t d);

#endif
