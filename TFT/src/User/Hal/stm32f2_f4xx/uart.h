#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

#define _USART1    0
#define _USART2    1
#define _USART3    2
#define _UART4     3
#define _UART5     4
#define _USART6    5
#define _UART_CNT  6

void UART_Config(uint8_t port, uint32_t baud, uint16_t usart_it);
void UART_DeConfig(uint8_t port);
void UART_Puts(uint8_t port, uint8_t *str);
void UART_Write(uint8_t port, uint8_t d);

#endif
