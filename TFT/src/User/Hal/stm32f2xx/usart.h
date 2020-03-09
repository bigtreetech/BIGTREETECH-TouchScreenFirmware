#ifndef _USART_H_
#define _USART_H_

#include "stm32f2xx_conf.h"

#define _USART1    0
#define _USART2    1
#define _USART3    2
#define _UART4     3
#define _UART5     4
#define _USART6    5
#define _USART_CNT 6

void USART_Config(uint8_t port, uint32_t baud, uint16_t usart_it);
void USART_DeConfig(uint8_t port);
void USART_Puts(uint8_t port, uint8_t *str);
void USART_Write(uint8_t port, uint8_t d);

#endif
