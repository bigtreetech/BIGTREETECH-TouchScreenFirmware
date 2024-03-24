#include "uart.h"
#include "variants.h"  // for USART1_TX_PIN etc.
#include "GPIO_Init.h"

// USART1 default pins config
#ifndef USART1_TX_PIN
  #define USART1_TX_PIN PA9
#endif
#ifndef USART1_RX_PIN
  #define USART1_RX_PIN PA10
#endif
// USART2 default pins config
#ifndef USART2_TX_PIN
  #define USART2_TX_PIN PA2
#endif
#ifndef USART2_RX_PIN
  #define USART2_RX_PIN PA3
#endif
// USART3 default pins config
#ifndef USART3_TX_PIN
  #define USART3_TX_PIN PB10
#endif
#ifndef USART3_RX_PIN
  #define USART3_RX_PIN PB11
#endif
// UART4 default pins config
#ifndef UART4_TX_PIN
  #define UART4_TX_PIN  PC10
#endif
#ifndef UART4_RX_PIN
  #define UART4_RX_PIN  PC11
#endif
// UART5 default pins config
#ifndef UART5_TX_PIN
  #define UART5_TX_PIN  PC12
#endif
#ifndef UART5_RX_PIN
  #define UART5_RX_PIN  PD2
#endif
// USART6 default pins config
#ifndef USART6_TX_PIN
  #define USART6_TX_PIN PG14
#endif
#ifndef USART6_RX_PIN
  #define USART6_RX_PIN PG9
#endif

static volatile uint32_t * const rcc_uart_rst[_UART_CNT] = {
  &RCC->APB2RSTR,
  &RCC->APB1RSTR,
  &RCC->APB1RSTR,
  &RCC->APB1RSTR,
  &RCC->APB1RSTR,
  &RCC->APB2RSTR,
};

static volatile uint32_t * const rcc_uart_en[_UART_CNT] = {
  &RCC->APB2ENR,
  &RCC->APB1ENR,
  &RCC->APB1ENR,
  &RCC->APB1ENR,
  &RCC->APB1ENR,
  &RCC->APB2ENR,
};

static const uint32_t rcc_uart_bit[_UART_CNT] = {
  0x00000010,  // RCC_APB2  bit 4
  0x00020000,  // RCC_APB1  bit 17
  0x00040000,  // RCC_APB1  bit 18
  0x00080000,  // RCC_APB1  bit 19
  0x00100000,  // RCC_APB1  bit 20
  0x00000020,  // RCC_APB2  bit 5
};

static USART_TypeDef * const uart[_UART_CNT] = {
  USART1,  // TX--PA9  RX--PA10
  USART2,  // TX--PA2  RX--PA3
  USART3,  // TX--PB10 RX--PB11
  UART4,   // TX--PC10 RX--PC11
  UART5,   // TX--PC12 RX--PD2
  USART6,  // TX--PG14 RX--PG9
};

static const uint16_t uart_tx[_UART_CNT] = {USART1_TX_PIN, USART2_TX_PIN, USART3_TX_PIN, UART4_TX_PIN, UART5_TX_PIN, USART6_TX_PIN};  // TX
static const uint16_t uart_rx[_UART_CNT] = {USART1_RX_PIN, USART2_RX_PIN, USART3_RX_PIN, UART4_RX_PIN, UART5_RX_PIN, USART6_RX_PIN};  // RX

static inline void UART_GPIO_Init(uint8_t port)
{
  uint8_t UART_AF_NUM[_UART_CNT] = {GPIO_AF_USART1, GPIO_AF_USART2, GPIO_AF_USART3, GPIO_AF_UART4, GPIO_AF_UART5, GPIO_AF_USART6};

  GPIO_InitSet(uart_tx[port], MGPIO_MODE_AF_PP, UART_AF_NUM[port]);
  GPIO_InitSet(uart_rx[port], MGPIO_MODE_AF_PP, UART_AF_NUM[port]);
}

static inline void UART_GPIO_DeInit(uint8_t port)
{
  // set tx/rx to input
  GPIO_InitSet(uart_tx[port], MGPIO_MODE_IPN, 0);
  GPIO_InitSet(uart_rx[port], MGPIO_MODE_IPN, 0);
}

static inline void UART_Protocol_Init(uint8_t port, uint32_t baud)
{
  USART_InitTypeDef USART_InitStructure;

  *rcc_uart_en[port] |= rcc_uart_bit[port];  // enable clock

  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_Init(uart[port], &USART_InitStructure);

  USART_Cmd(uart[port],ENABLE);
}

static inline void UART_IRQ_Init(uint8_t port, uint16_t usart_it, FunctionalState idle_interrupt)
{
  uint32_t IRQ_Channel[_UART_CNT] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel[port];
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(uart[port], usart_it, idle_interrupt);  // enable or disable serial line IDLE interrupt
  USART_ClearITPendingBit(uart[port], usart_it);
}

void UART_Config(uint8_t port, uint32_t baud, uint16_t usart_it, bool idle_interrupt)
{
  UART_Protocol_Init(port, baud);
  UART_IRQ_Init(port, usart_it, idle_interrupt ? ENABLE : DISABLE);
  UART_GPIO_Init(port);  // after all initialization is completed, enable IO, otherwise a 0xFF will be sent automatically after power-on
}

void UART_DeConfig(uint8_t port)
{
  UART_GPIO_DeInit(port);

  *rcc_uart_rst[port] |= rcc_uart_bit[port];
  *rcc_uart_rst[port] &= ~rcc_uart_bit[port];  // reset clock
}

void UART_Write(uint8_t port, uint8_t d)
{
  while ((uart[port]->SR & USART_FLAG_TC) == (uint16_t)RESET);  // wait
  uart[port]->DR = ((uint16_t)d & (uint16_t)0x01FF);
}

void UART_Puts(uint8_t port, uint8_t * str)
{
  while (*str)
  {
    while ((uart[port]->SR & USART_FLAG_TC) == (uint16_t)RESET);  // wait
    uart[port]->DR = ((uint16_t)*str++ & (uint16_t)0x01FF);
  }
}
